#include <vector>
#include <spdlog/spdlog.h>
#include "Utility.h"
#include "basic/Player.h"
#include "basic/GameController.h"

namespace util {

    bool sendCommand(kc::Player& player, CommandType commandType, const std::string &message,
                     std::chrono::milliseconds timeout) {
        try {
            BasicMessage msg;
            msg.set_type(commandType);
            msg.set_player_id(player.id);
            msg.set_message(message);
            zmq::message_t connect_msg(msg.ByteSizeLong());
            msg.SerializeToArray(connect_msg.data(), static_cast<int>(connect_msg.size()));
            {
                std::lock_guard<std::mutex> lock(player.mtx);
                player.socket.set(zmq::sockopt::sndtimeo, static_cast<int>(timeout.count()));
                player.socket.set(zmq::sockopt::linger, 0);
                player.socket.send(connect_msg, zmq::send_flags::none);
            }
        } catch (std::exception &e) {
            spdlog::warn("向玩家{}发送指令失败, 原因是: {}", player.id, e.what());
            return false;
        }
        return true;
    }

    bool sendCommand(const kc::PlayerPtr& player, CommandType commandType, const std::string &message,
                     std::chrono::milliseconds timeout) {
        return sendCommand(*player, commandType, message, timeout);
    }

    bool sendCommand(kc::Player *player, CommandType commandType, const std::string &message,
                     std::chrono::milliseconds timeout) {
        return sendCommand(*player, commandType, message, timeout);
    }

    std::optional<CommandType> recvCommand(kc::Player& player, std::string& message,
                                           std::chrono::milliseconds timeout) {
        try {
            zmq::message_t msg;
            zmq::recv_result_t size;
            {
                std::lock_guard<std::mutex> lock(player.mtx);
                player.socket.set(zmq::sockopt::rcvtimeo, static_cast<int>(timeout.count()));
                player.socket.set(zmq::sockopt::linger, 0);
                size = player.socket.recv(msg);
            }
            if (!size.has_value()) {
                spdlog::debug("服务器收到了一个空消息");
                throw std::runtime_error("服务器收到了一个空消息");
            }
            BasicMessage parsedMessage;
            bool result = parsedMessage.ParseFromString(msg.to_string());
            if (!result) {
                spdlog::debug("无法解析消息内容, 消息文本为: {}", msg.to_string());
                throw std::runtime_error("无法解析消息内容");
            }
            message = parsedMessage.message();
            return parsedMessage.type();
        } catch (std::exception &e) {
            spdlog::error("从玩家 {} 接受消息失败, 原因是: {}", player.id, e.what());
            return std::nullopt;
        }
    }

    std::optional<CommandType> recvCommand(const kc::PlayerPtr& player, std::string& message,
                                           std::chrono::milliseconds timeout) {
        return recvCommand(*player, message, timeout);
    }

    std::optional<CommandType> recvCommand(const kc::PlayerPtr& player) {
        std::string message;
        return recvCommand(player, message);
    }

    PlayerIdentity_pb to_pb(kc::PlayerIdentity identity) {
        return static_cast<PlayerIdentity_pb>(identity - 1);
    }

    CardType_pb to_pb(kc::CardType type) {
        return static_cast<CardType_pb>(type);
    }

    TurnType_pb to_pb(kc::TurnType type) {
        return static_cast<TurnType_pb>(type);
    }

    kc::PlayerIdentity to_kc(PlayerIdentity_pb identity) {
        return static_cast<kc::PlayerIdentity>(identity + 1);
    }

    kc::CardType to_kc(CardType_pb type) {
        return static_cast<kc::CardType>(type);
    }

    kc::TurnType to_kc(TurnType_pb type) {
        return static_cast<kc::TurnType>(type);
    }

    Player_pb to_pb(const kc::Player& player) {
        Player_pb pb;
        pb.set_id(player.id);
        pb.set_hp(player.getHealth());
        pb.set_maxhp(player.getMaxHealth());
        pb.set_cardcnt(player.getCardCount());
        return pb;
    }

    Card_pb to_pb(const kc::Card& card) {
        Card_pb pb;
        pb.set_id(card.id);
        pb.set_type(to_pb(card.type));
        return pb;
    }

    void Timer::start() {
        startTime = std::chrono::steady_clock::now();
        isTiming = true;
    }

    void Timer::pause() {
        total += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime);
        isTiming = false;
    }

    void Timer::reset() {
        total = std::chrono::microseconds(0);
        isTiming = false;
    }

    std::chrono::microseconds Timer::getTime() const {
        if (isTiming)
            return total + std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - startTime);
        else
            return total;
    }
}
