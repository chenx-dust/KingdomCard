//
// Created by 10315 on 2023/12/21.
//

#ifndef KINGDOM_CARD_UTILS_H
#define KINGDOM_CARD_UTILS_H

#include <string>

namespace UTILS {
    std::string const CardName[] = { "杀", "闪", "桃",
                                     "过河拆桥", "顺手牵羊", "决斗", "万箭齐发", "南蛮入侵", "无中生有", "五谷丰登", "桃园结义",
                                     "无懈可击" };

    std::string const PlayerIdentityName[] = {
            "主公",
            "忠臣",
            "反贼",
            "内奸"
    };

    class Player {
        int id;
        int hp;
        int mp;
        int cardCount;
    };

    class utils {
    public:
        static int PeopleCount;
    };
}

#endif //KINGDOM_CARD_UTILS_H
