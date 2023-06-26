#ifndef KINGDOMCARD_CARD_H
#define KINGDOMCARD_CARD_H

#include <memory>

class Player;

enum TargetType {
    SELF,
    SINGLE,
    ALL
};

enum CardType {
    ATTACK,
    DODGE,
    HEAL,
    SPECIAL
};

class Card {
public:
    /// \brief 卡牌是否可以主动使用
    bool const isActiveUsable;
    /// \brief 卡牌是否可以被动使用
    bool const isPassiveUsable;
    // TODO: 判定牌

    /// \brief 卡牌类型
    enum CardType const cardType;
    /// \brief 卡牌目标类型
    enum TargetType const targetType;
    // TODO: 对于桃的回血加入判定

    /// \brief 主动使用卡牌
     virtual void useActively(Player& target) { assert(0); };
     virtual void useActively(std::vector<Player>& targets) { assert(0); };
    /// \brief 被动使用卡牌
     virtual void usePassively(Player& target) { assert(0); };
     virtual void usePassively(std::vector<Player>& targets) { assert(0); };

     Card(bool isActiveUsable, bool isPassiveUsable, enum CardType cardType, enum TargetType targetType) :
        isActiveUsable(isActiveUsable), isPassiveUsable(isPassiveUsable), cardType(cardType), targetType(targetType) {};
};

typedef std::unique_ptr<Card> CardPtr;

#endif //KINGDOMCARD_CARD_H
