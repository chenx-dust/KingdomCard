#include <spdlog/spdlog.h>

#include <memory>
#include "GameServer.h"
#include "basic/Utility.h"
#include "basic/Player.h"
#include "basic/GameController.h"
#include "basic_message.pb.h"

// TODO: 切换成利用 monitor 监控连接状态

namespace kc {
    GameServer::GameServer(zmq::context_t &context, const uint16_t port) : context(context) {
        /// @brief 服务器构造函数
        /// @param context ZeroMQ 上下文
        /// @param port 服务器端口号
        potentialPort = port;
        bridgeRepSocket = zmq::socket_t(context, ZMQ_REP);
        bool bindSuccess = false;
        while (!bindSuccess) {
            try {
                // 开放登入端口
                bridgeRepSocket.bind("tcp://*:" + std::to_string(potentialPort));
                bindSuccess = true;
                potentialPort++;
            } catch (zmq::error_t &e) {
                // 假如失败则端口号+1
                if (e.num() == EADDRINUSE && potentialPort < 65535)
                    potentialPort++;
                else
                    throw e;
            }
        }
        spdlog::info("服务器已开放端口: {}", potentialPort - 1);
    }

    GameServer::~GameServer() {
        /// @brief 服务器析构函数
        // 关闭所有套接字
        bridgeRepSocket.close();
        for (auto &player: players) {
            player->socket.close();
        }
    }

    void GameServer::waitForConnection() {
        /// @brief 等待客户端连接
        if (isWaiting)
            return;
        spdlog::info("服务器等待客户端连接");
        isWaiting = true;
        connectionThread = std::thread([&]() {
            while (isWaiting) {
                try {
                    zmq::message_t message;
                    zmq::recv_result_t size = bridgeRepSocket.recv(message);
                    if (!size.has_value()) {
                        spdlog::debug("服务器收到了一个空消息");
                        continue;
                    }
                    std::string msg = std::string(static_cast<char *>(message.data()), message.size());
                    BasicMessage parsedMessage;
                    bool result = parsedMessage.ParseFromString(msg);
                    if (!result) {
                        spdlog::debug("无法解析消息内容, 消息文本为: {}", msg);
                        continue;
                    }
                    if (parsedMessage.type() == CommandType::CONNECT_REQ) {
                        spdlog::info("客户端连接, 下发连接信息");
                        connectWithClient();
                    } else
                        spdlog::debug("错误的消息类型: {}", parsedMessage.GetTypeName());
                } catch (std::exception &e) {
                    spdlog::error("服务器等待连接时发生错误: {}", e.what());
                }
                if (players.size() >= MAX_PLAYER_NUM)
                    isWaiting = false;
            }
            spdlog::debug("结束等待");
        });
    }

    void GameServer::connectWithClient() {
        /// @brief 与客户端建立连接
        /// @return 玩家对象指针
        bool bindSuccess = false;
        zmq::socket_t socket(context, ZMQ_PAIR);
        // 开放与玩家连接的端口
        while (!bindSuccess) {
            try {
                socket.bind("tcp://*:" + std::to_string(potentialPort));
                bindSuccess = true;
                potentialPort++;
            } catch (zmq::error_t &e) {
                if (e.num() == EADDRINUSE && potentialPort < 65535)
                    potentialPort++;
                else
                    throw e;
            }
        }
        spdlog::debug("服务器对玩家 {} 端口: {}", assignedId, potentialPort - 1);
        // 发送连接信息
        ConnectResponse connect_r;
        connect_r.set_port(potentialPort - 1);
        connect_r.set_player_id(assignedId);
        BasicMessage connect_m;
        connect_m.set_type(CommandType::CONNECT_REP);
        connect_m.set_message(connect_r.SerializeAsString());
        zmq::message_t connect_msg(connect_m.ByteSizeLong());
        connect_m.SerializeToArray(connect_msg.data(), static_cast<int>(connect_msg.size()));
        bridgeRepSocket.send(connect_msg, zmq::send_flags::none);
        // 验证玩家连接
        socket.set(zmq::sockopt::rcvtimeo, 1000); // 设置超时时间为1s
        PlayerPtr player = std::make_shared<Player>(assignedId++, std::move(socket));
        util::RecvResult rslt = util::recvCommand(player);
        if (rslt.has_value() && rslt.value() == CommandType::CONNECT_ACK) {
            spdlog::info("玩家 {} 连接成功", player->id);
            // 创建玩家对象
            std::lock_guard<std::mutex> lock(mtx);
            players.emplace_back(std::move(player));
        } else {
            if (rslt.has_value())
                spdlog::warn("玩家 {} 连接失败, 消息类型错误: {}", player->id, CommandType_Name(rslt.value()));
            else
                spdlog::warn("玩家 {} 连接失败, 其他错误原因", player->id);
        }
    }

    bool GameServer::isReady() {
        /// @brief 判断服务器是否准备就绪
        /// @return 服务器是否准备就绪
        checkAndKick();
        return players.size() >= MIN_PLAYER_NUM;
    }

    void GameServer::checkAndKick() {
        /// @brief 检查连通性并踢出掉线的玩家
        spdlog::debug("检查玩家连通性");
        recheck:
        for (auto it = players.begin(); it != players.end(); it++) {
            // 发送验证连接请求
            bool s_rslt = util::sendCommand(*it, CommandType::CONNECT_ACK);
            util::RecvResult r_rslt = util::recvCommand(*it);
            if (!s_rslt || !r_rslt.has_value() || r_rslt.value() != CommandType::CONNECT_ACK) {
                if (r_rslt.has_value())
                    spdlog::warn("玩家 {} 掉线, 消息类型错误: {}", (*it)->id, CommandType_Name(r_rslt.value()));
                else
                    spdlog::warn("玩家 {} 掉线, 其他错误原因", (*it)->id);
                // 踢出掉线玩家
                util::sendCommand(*it, CommandType::KICK);
                std::lock_guard<std::mutex> lock(mtx);
                std::lock_guard<std::mutex> lock_m((*it)->mtx);
                (*it)->socket.close();
                players.erase(it);
                goto recheck;
            }
        }
    }

    void GameServer::start() {
        /// @brief 开始游戏
        if (!isReady()) {
            spdlog::warn("人数不足, 无法开始游戏");
            return;
        }
        spdlog::info("开始游戏");
        // 移交 GameController 控制
        GameController controller(players);
        controller.start();
    }

}

