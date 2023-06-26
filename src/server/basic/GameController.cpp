
#include <random>
#include "basic/GameController.h"

unsigned int const id_count[7][4] = {
        {1, 1, 1, 1},
        {1, 1, 2, 1},
        {1, 1, 3, 1},
        {1, 2, 3, 1},
        {1, 2, 4, 1},
        {1, 3, 4, 1},
        {1, 3, 4, 2}
};

void GameController::init(std::vector<Player>&& players_list) {
    players = std::move(players_list);
    // 初始化角色
    {
        size_t player_num = players.size();
        if (player_num < 4 || player_num > 10) {
            throw std::invalid_argument("玩家数量不合法");
        }
        std::shuffle(players.begin(), players.end(), std::default_random_engine(std::random_device()()));
        for (int idt = 0; idt < 4; ++idt)
            for (int num = 0; num < id_count[player_num - 4][idt]; ++num)
                players[num].setIdentity(static_cast<PlayerIdentity>(idt));
        std::shuffle(players.begin(), players.end(), std::default_random_engine(std::random_device()()));
    }
    // 初始化牌组
    {

    }
}
