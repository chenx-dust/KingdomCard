#include <vector>
#include <format>
#include <spdlog/spdlog.h>
#include "Utility.h"
#include "basic/Player.h"

namespace util {
    std::optional<kc::PlayerPtr> FindPlayerById(const std::vector<kc::PlayerPtr>& players, uint16_t id) {
        for (const auto& player : players) {
            if (player->id == id) {
                return player;
            }
        }
        return std::nullopt;
    }

    bool sendCommand(const kc::PlayerPtr& player, CommandType commandType, const std::string &message) {
        try {
            BasicMessage msg;
            msg.set_type(commandType);
            msg.set_player_id(player->id);
            msg.set_message(message);
            zmq::message_t connect_msg(msg.ByteSizeLong());
            msg.SerializeToArray(connect_msg.data(), static_cast<int>(connect_msg.size()));
            {
                std::lock_guard<std::mutex> lock(player->mtx);
                player->socket.send(connect_msg, zmq::send_flags::none);
            }
        } catch (std::exception &e) {
            spdlog::warn("向玩家{}发送指令失败, 原因是: {}", player->id, e.what());
            return false;
        }
        return true;
    }

    std::optional<CommandType> recvCommand(const kc::PlayerPtr& player, std::string& message) {
        try {
            zmq::message_t msg;
            zmq::recv_result_t size;
            {
                std::lock_guard<std::mutex> lock(player->mtx);
                size = player->socket.recv(msg);
            }
            if (!size.has_value()) {
                spdlog::debug("服务器收到了一个空消息");
                throw std::runtime_error("服务器收到了一个空消息");
            }
            message = std::string(static_cast<char *>(msg.data()), msg.size());
            BasicMessage parsedMessage;
            bool result = parsedMessage.ParseFromString(message);
            if (!result) {
                spdlog::debug("无法解析消息内容, 消息文本为: {}", message);
                throw std::runtime_error("无法解析消息内容");
            }
            return parsedMessage.type();
        } catch (std::exception &e) {
            spdlog::warn("从玩家 {} 接受消息失败, 原因是: {}", player->id, e.what());
            return std::nullopt;
        }
    }

    std::optional<CommandType> recvCommand(const kc::PlayerPtr& player) {
        std::string message;
        return recvCommand(player, message);
    }
}
