#ifndef KINGDOMCARD_PLAYER_H
#define KINGDOMCARD_PLAYER_H

#include <vector>

#include "basic/Card.h"

enum PlayerIdentity {
    LORD,       // 主公
    MINISTER,   // 忠臣
    REBEL,      // 反贼
    SPY         // 内奸
};

class Player {
private:
    unsigned int health_point;
    unsigned int max_hp;
    enum PlayerIdentity identity;
    bool is_dead = false;
    std::vector<CardPtr> hand_cards;
public:
    Player() = default;
    explicit Player(unsigned int healthPoint) : health_point(healthPoint), max_hp(healthPoint) {}
    /// \brief 返回玩家的生命值
    [[nodiscard]] unsigned int getHealthPoint() const;
    /// \brief 返回玩家的最大生命值
    [[nodiscard]] unsigned int getMaxHealthPoint() const;
    /// \brief 设置玩家的生命值
    void setHealthPoint(unsigned int healthPoint);
    /// \brief 设置玩家的最大生命值
    void setMaxHealthPoint(unsigned int maxHp);
    /// \brief 减少玩家的生命值
    void minusHealthPoint(unsigned int healthPoint);
    /// \brief 增加玩家的生命值
    void plusHealthPoint(unsigned int healthPoint);
    /// \brief 返回玩家的身份
    [[nodiscard]] enum PlayerIdentity getIdentity() const;
    /// \brief 设置玩家的身份
    void setIdentity(enum PlayerIdentity identity);
    /// \brief 玩家获得一张手牌
    void addCard(CardPtr card);
    /// \brief 玩家弃掉多余手牌
    void giveUpCard();
    /// \brief 玩家进入濒死状态
    void dying();
    /// \brief 玩家是否已经死亡
    [[nodiscard]] bool isDead() const;
};


#endif //KINGDOMCARD_PLAYER_H
