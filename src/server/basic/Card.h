
#ifndef KINGDOMCARD_CARD_H
#define KINGDOMCARD_CARD_H

#include <cinttypes>
#include <memory>

namespace kc {
    enum CardType {
        SLASH,              // 杀
        DODGE,              // 闪
        PEACH,              // 桃
        BRIDGE_DESTRUCTION, // 过河拆桥
        STEAL,              // 顺手牵羊
        DUEL,               // 决斗
        RAIN_OF_ARROWS,     // 万箭齐发
        BARBARIAN_INVASION, // 南蛮入侵
        SLEIGHT_OF_HAND,    // 无中生有
        HARVEST_FEAST,      // 五谷丰登
        PEACH_GARDEN,       // 桃园结义
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

        void static init() {
            idCounter = 0;
        }

        CardPtr static generate(CardType type) {
            return std::unique_ptr<Card>(new Card(idCounter++, type));
        }
    };
}

#endif //KINGDOMCARD_CARD_H
