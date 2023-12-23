
#ifndef KINGDOMCARD_GAMESERVER_H
#define KINGDOMCARD_GAMESERVER_H

#include <thread>
#include <vector>
#include <zmq.hpp>
#include "basic/Player.h"

namespace kc {
    class GameServer {
    private:
        zmq::context_t &context;
        zmq::socket_t bridgeRepSocket;      // 用于通告客户端连接的套接字
        std::thread connectionThread;       // 用于等待客户端连接的线程
        std::vector<PlayerPtr> players;     // 玩家列表
        std::mutex mtx;                     // 用于保护玩家列表的互斥量
        uint16_t potentialPort;
        bool isWaiting = false;
        uint16_t assignedId = 0;
        uint16_t waitingPlayerNum = MAX_PLAYER_NUM;
    public:
        GameServer() = delete;

        GameServer(const GameServer &) = delete;

        GameServer(zmq::context_t &context, uint16_t port);

        ~GameServer();

        void waitForConnection();

        void connectWithClient();

        [[nodiscard]] bool isReady();
        void checkAndKick();

        void listPlayers();

        void kickPlayer(uint16_t player_id);

        void setWaitingPlayerNum(uint16_t num);

        void start();
    };
}

#endif //KINGDOMCARD_GAMESERVER_H
