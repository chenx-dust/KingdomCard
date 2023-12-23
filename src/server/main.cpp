#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <iostream>
#include "communication/GameServer.h"

int main()
{
    spdlog::set_level(spdlog::level::debug);
    zmq::context_t context(1);
    kc::GameServer server(context, 13364);
    server.waitForConnection();
    spdlog::info("等待连接成功");
    spdlog::info("可用命令:\n"
                 "\tstart: 立即开始游戏\n"
                 "\tmax <start_num>: 最大等待人数\n"
                 "\tlist: 列出所有玩家\n"
                 "\tcheck: 检查玩家是否在线\n"
                 "\tkick <player_id>: 踢出玩家 player_id\n"
                 "\texit: 退出服务器");
    std::string command;
    while (std::cin >> command) {
        if (command == "start") {
            if (server.isReady()) {
                server.start();
                break;
            } else {
                spdlog::error("玩家人数不足");
            }
        } else if (command == "max") {
            unsigned start_num;
            std::cin >> start_num;
            server.setWaitingPlayerNum(start_num);
        } else if (command == "list") {
            server.listPlayers();
        } else if (command == "check") {
            server.checkAndKick();
        } else if (command == "kick") {
            int player_id;
            std::cin >> player_id;
            server.kickPlayer(player_id);
        } else if (command == "exit") {
            break;
        } else {
            spdlog::warn("未知命令: {}", command);
        }
    }
    return 0;
}
