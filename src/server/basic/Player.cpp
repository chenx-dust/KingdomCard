//
// Created by 陈希峻 on 2023/6/26.
//

#include "basic/Player.h"

[[nodiscard]] unsigned int Player::getHealthPoint() const {
    // nodiscard 表示该函数的返回值不应该被忽略
    return health_point;
}

[[nodiscard]] unsigned int Player::getMaxHealthPoint() const {
    // nodiscard 表示该函数的返回值不应该被忽略
    return max_hp;
}

void Player::setHealthPoint(unsigned int healthPoint) {
    health_point = healthPoint;
}

void Player::setMaxHealthPoint(unsigned int maxHp) {
    max_hp = maxHp;
}

void Player::minusHealthPoint(unsigned int healthPoint) {
    if (healthPoint > health_point)
    {
        // 假如玩家生命值小于0，那么玩家进入濒死状态
        health_point = 0;
        dying();
    }
    else
        health_point -= healthPoint;
}

void Player::plusHealthPoint(unsigned int healthPoint) {
    if (healthPoint + health_point > max_hp)
        health_point = max_hp;
    else
        health_point += healthPoint;
}

[[nodiscard]] enum PlayerIdentity Player::getIdentity() const {
    return identity;
}

void Player::setIdentity(enum PlayerIdentity identity) {
    Player::identity = identity;
}

[[nodiscard]] bool Player::isDead() const {
    return is_dead;
}

void Player::addCard(CardPtr card) {
    hand_cards.push_back(std::move(card));
}

void Player::giveUpCard() {
    if (hand_cards.size() > health_point) {
        // 假如玩家手牌数大于生命值，那么玩家弃掉多余手牌
        // TODO: 弃牌选择
    }
}

void Player::dying() {
    // TODO: 玩家进入濒死状态
    is_dead = true;
}
