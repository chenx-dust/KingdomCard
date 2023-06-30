
#ifndef KINGDOMCARD_CARD_H
#define KINGDOMCARD_CARD_H

#include <cinttypes>
#include <memory>
#include <string>

namespace kc {
    size_t const CARD_TYPE_COUNT = 12;
    size_t const CARD_COUNT[] = { 6, 6, 6,
                                  4, 4, 4, 4, 4, 4, 4, 4,
                                  6};
    std::string const CardName[] = { "杀", "闪", "桃",
                                     "过河拆桥", "顺手牵羊", "决斗", "万箭齐发", "南蛮入侵", "无中生有", "五谷丰登", "桃园结义",
                                     "无懈可击" };

    enum CardType {
        SLASH,              // 杀
        DODGE,              // 闪
        PEACH,              // 桃
        DISMANTLE,          // 过河拆桥
        STEAL,              // 顺手牵羊
        DUEL,               // 决斗
        ARCHERY_VOLLEY,     // 万箭齐发
        BARBARIAN,          // 南蛮入侵
        SLEIGHT_OF_HAND,    // 无中生有
        HARVEST_FEAST,      // 五谷丰登
        PEACH_GARDEN_OATH,  // 桃园结义
        UNRELENTING         // 无懈可击
    };

    class Card;

    typedef std::unique_ptr<Card> CardPtr;

    class Card {
    private:
        Card(uint16_t id, CardType type) : id(id), type(type) {}

        static uint16_t idCounter;
    public:
        Card(Card const &) = delete;

        uint16_t const id;
        CardType const type;

        CardPtr static generate(CardType type) {
            return std::unique_ptr<Card>(new Card(idCounter++, type));
        }
    };
}

#endif //KINGDOMCARD_CARD_H
