
#include <algorithm>
#include <random>
#include <spdlog/spdlog.h>
#include "Player.h"
#include "basic/Utility.h"
#include "command.pb.h"

namespace kc {
    /// @brief 根据 id 获取玩家手牌中的一张牌
    CardPtr& Player::getCard(size_t cid) {
        for (auto &card: handCards) {
            if (card->id == cid) {
                return card;
            }
        }
        throw std::invalid_argument("玩家没有这张牌");
    }

    /// @brief 根据 id 从玩家手牌中移除一张牌
    CardPtr Player::removeCard(size_t cid) {
        for (auto it = handCards.begin(); it != handCards.end(); ++it) {
            if ((*it)->id == cid) {
                CardPtr card = std::move(*it);
                handCards.erase(it);
                return std::move(card);
            }
        }
        throw std::invalid_argument("玩家没有这张牌");
    }

    /// @brief 为玩家添加一组新的手牌, 并且通知玩家
    void Player::newCardList(std::vector<CardPtr> &&cards) {
        NewCard cmd;
        for (auto &card: cards) {
            cmd.add_newcards()->CopyFrom(util::to_pb(*card));
            handCards.emplace_back(std::move(card));
        }
        util::sendCommand(this, CommandType::NEW_CARD, cmd.SerializeAsString());
    }

    /// @brief 弃掉多余生命点的牌, 并且通知玩家
    std::vector<CardPtr> Player::discardMoreCard() {
        DiscardCard cmd;
        std::vector<CardPtr> discardCards;
        // 洗牌
        std::shuffle(handCards.begin(), handCards.end(), std::default_random_engine(std::random_device()()));
        int64_t cardNum = handCards.size() - health;
        for (size_t i = 0; i < cardNum; ++i) {
            spdlog::info("玩家 {} 弃掉了 id: {} type: {}", id, handCards[0]->id, CardName[handCards[0]->type]);
            cmd.add_discardedcards()->CopyFrom(util::to_pb(*handCards[0]));
            discardCards.emplace_back(std::move(handCards[0]));
            handCards.erase(handCards.begin());
        }
        util::sendCommand(this, CommandType::DISCARD_CARD, cmd.SerializeAsString());
        return std::move(discardCards);
    }

    /// @brief 死亡归还牌组
    std::vector<CardPtr> Player::die() {
        std::vector<CardPtr> hc_temp = std::move(handCards);
        handCards.clear();
        alive = false;
        health = 0;
        return std::move(hc_temp);
    }

    /// @brief 判断玩家是否有某种牌
    bool Player::hasCard(CardType type) {
        for (auto &card: handCards)
            if (card->type == type)
                return true;
        return false;
    }

    /// @brief 判断玩家是否有某些牌中的一种
    bool Player::hasCard(const std::set<CardType>& type) {
        for (auto i : type)
            if (hasCard(i))
                return true;
        return false;
    }

    /// @brief 根据序号删除卡牌
    CardPtr Player::removeCardByNum(size_t num) {
        if (num >= handCards.size())
            throw std::invalid_argument("玩家没有这张牌");
        CardPtr card = std::move(handCards[num]);
        handCards.erase(handCards.begin() + num);
        return std::move(card);
    }
}
