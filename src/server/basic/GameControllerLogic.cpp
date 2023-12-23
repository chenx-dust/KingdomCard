
#include "GameController.h"

#include <random>
#include <algorithm>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include "basic/Utility.h"
#include "basic_message.pb.h"
#include "basic_object.pb.h"
#include "command.pb.h"

namespace kc {
    /// @brief 开始游戏
    void GameController::start() {
        init();
        isStarted = true;
        // 主循环
        while (isStarted) {
            newTurn();
            nextPlayerIdx();
            if (checkWin())
                isStarted = false;
        }
    }

    /// @brief 初始化游戏
    void GameController::init() {
        // 初始化角色
        {
            size_t player_num = players.size();
            if (player_num < 4 || player_num > 10) {
                throw std::invalid_argument("玩家数量不合法");
            }
            // 随机排列角色
            std::shuffle(players.begin(), players.end(), std::default_random_engine(std::random_device()()));
            // 选取首位为主公
            lordId = players[0]->id;
            // 为角色分配身份
            size_t idx = 0;
            for (int idt = 0; idt < 4; ++idt)
                for (int num = 0; num < ID_COUNT[player_num - 4][idt]; ++num)
                    players[idx++]->setIdentity(static_cast<PlayerIdentity>(idt + 1));
            // 按照 id 排序
            std::sort(players.begin(), players.end(), [](const PlayerPtr &a, const PlayerPtr &b) {
                return a->id < b->id;
            });
        }
        startCommand();
        // 初始化牌组
        {
            for (int tp = 0; tp < CARD_TYPE_COUNT; ++tp)
                for (int num = 0; num < CARD_COUNT[tp]; ++num)
                    cards.emplace_back(Card::generate(static_cast<CardType>(tp)));
            // 洗牌
            std::shuffle(cards.begin(), cards.end(), std::default_random_engine(std::random_device()()));
            for (const auto &card : cards)
                spdlog::debug("id: {} type: {}", card->id, CardName[card->type]);
            // 分配给角色
            for (const auto &player : players) {
                std::vector<CardPtr> card_to_add;
                for (int i = 0; i < 4; ++i)
                    card_to_add.emplace_back(drawCard());
                spdlog::info("玩家 {} 初始牌组:", player->id);
                for (const auto &card : card_to_add)
                    spdlog::info("id: {} type: {}", card->id, CardName[card->type]);
                player->newCardList(std::move(card_to_add));
            }
        }
        {
            // 调试输出
            spdlog::info("玩家数量: {}", players.size());
            spdlog::info("主公: {}", lordId);
            spdlog::info("角色分配: ");
            for (const auto &player : players) {
                spdlog::info("玩家 {} 身份: {}", player->id, PlayerIdentityName[player->getIdentity()]);
                spdlog::debug("玩家 {} 手牌: ", player->id);
                for (const auto &card : player->getCards())
                    spdlog::debug("id: {} type: {}", card->id, CardName[card->type]);
            }
        }
    }

    /// @brief 新的回合
    void GameController::newTurn() {
        turn_timer.reset();
        // 发牌
        std::vector<CardPtr> card_to_add;
        card_to_add.emplace_back(drawCard());
        card_to_add.emplace_back(drawCard());
        spdlog::info("玩家 {} 回合开始, 发牌", players[currIdx]->id);
        for (const auto &card : card_to_add)
            spdlog::info("id: {} type: {}", card->id, CardName[card->type]);
        players[currIdx]->newCardList(std::move(card_to_add));

        bcStatus();

        bool isContinue = true;
        while (isContinue) {
            // 发送回合进行消息
            YourTurn cmd_yt;
            cmd_yt.set_remainingtime((TURN_TIME_LIMIT - turn_timer.getTime()).count() / 1000.0f);
            util::sendCommand(players[currIdx], CommandType::YOUR_TURN, cmd_yt.SerializeAsString());
            turn_timer.start();     // 开始计时

            auto rslt = waitForCard({currIdx});
            if (rslt.type() == typeid(CardAction)) {
                spdlog::info("玩家 {} 出牌", players[currIdx]->id);
                auto action = std::any_cast<CardAction>(rslt);
                // 处理出牌
                dealWithCard(action);
                turn_timer.pause();
            }
            else if (rslt.type() == typeid(DiscardAction)) {
                auto action = std::any_cast<DiscardAction>(rslt);
                spdlog::info("玩家 {} 弃牌", players[currIdx]->id);
                // 验证弃牌
                try {
                    for (const auto &card_id : action.card_ids)
                        removeCard(players[currIdx]->id, card_id);
                    if (players[currIdx]->getCards().size() > players[currIdx]->getHealth())
                        throw std::invalid_argument("弃牌数量过少");
                } catch (std::exception &e) {
                    spdlog::error("玩家 {} 弃牌异常: {}", players[currIdx]->id, e.what());
                    // 强制弃牌
                    std::vector<CardPtr> dCards = players[currIdx]->discardMoreCard();
                    for (auto &card : dCards)
                        cards.emplace_back(std::move(card));
                    return;
                }
                isContinue = false;
            }
            else {
                spdlog::info("玩家 {} 回合未出牌, 强制结束", players[currIdx]->id);
                std::vector<CardPtr> dCards = players[currIdx]->discardMoreCard();
                for (auto &card : dCards)
                    cards.emplace_back(std::move(card));
                isContinue = false;
            }
        }
        // 洗牌
        std::shuffle(cards.begin(), cards.end(), std::default_random_engine(std::random_device()()));
    }

    /// @brief 处理卡牌效果
    /// @param action 玩家出牌动作
    void GameController::dealWithCard(const CardAction& action) {
        removeCard(action);
        if (action.type == CardType::SLASH) {
            if (action.target_id == currIdx)
                throw std::invalid_argument("不能对自己使用杀");
            else if (!isNearby(action.target_id))
                throw std::invalid_argument("目标不在攻击范围内");
            else if (!findPlayerById(action.target_id).isAlive())
                throw std::invalid_argument("目标已经死亡");
            std::optional<CardAction> rslt = waitForReact({action.target_id}, {CardType::DODGE});
            if (rslt.has_value()) {
                spdlog::info("玩家 {} 对玩家 {} 使用杀, 已闪避", players[currIdx]->id, action.target_id);
                removeCard(rslt.value());
            }
            else {
                spdlog::info("玩家 {} 对玩家 {} 使用杀", players[currIdx]->id, action.target_id);
                damage(action.target_id);
            }
        }
        else if (action.type == CardType::PEACH) {
            if (players[currIdx]->getHealth() + 1 > players[currIdx]->getMaxHealth())
                throw std::invalid_argument("玩家满血不能使用桃");
            players[currIdx]->setHealth(players[currIdx]->getHealth() + 1);
        }
        else if (action.type == CardType::DISMANTLE) {
            if (action.target_id == currIdx)
                throw std::invalid_argument("不能对自己使用过河拆桥");
            else if (!findPlayerById(action.target_id).isAlive())
                throw std::invalid_argument("目标已经死亡");
            std::optional<CardAction> rslt = waitForReact({action.target_id}, {CardType::UNRELENTING});
            if (rslt.has_value()) {
                spdlog::info("玩家 {} 对玩家 {} 使用过河拆桥, 已无懈可击", players[currIdx]->id, action.target_id);
                removeCard(rslt.value());
            }
            else {
                spdlog::info("玩家 {} 对玩家 {} 使用过河拆桥", players[currIdx]->id, action.target_id);
                CardPtr card = findPlayerById(action.target_id).removeCardByNum(action.draw_card_num);
                spdlog::info("id: {} type: {}", card->id, card->type);
                cards.emplace_back(std::move(card));
            }
        }
        else if (action.type == CardType::STEAL) {
            if (action.target_id == currIdx)
                throw std::invalid_argument("不能对自己使用顺手牵羊");
            else if (!isNearby(action.target_id))
                throw std::invalid_argument("目标不在攻击范围内");
            else if (!findPlayerById(action.target_id).isAlive())
                throw std::invalid_argument("目标已经死亡");
            std::optional<CardAction> rslt = waitForReact({action.target_id}, {CardType::UNRELENTING});
            if (rslt.has_value()) {
                spdlog::info("玩家 {} 对玩家 {} 使用顺手牵羊, 已无懈可击", players[currIdx]->id, action.target_id);
                removeCard(rslt.value());
            }
            else {
                spdlog::info("玩家 {} 对玩家 {} 使用顺手牵羊", players[currIdx]->id, action.target_id);
                CardPtr card = findPlayerById(action.target_id).removeCardByNum(action.draw_card_num);
                spdlog::info("id: {} type: {}", card->id, card->type);
                players[currIdx]->addCard(std::move(card));
            }
        }
        else if (action.type == CardType::DUEL) {
            if (action.target_id == currIdx)
                throw std::invalid_argument("不能对自己使用决斗");
            else if (!isNearby(action.target_id))
                throw std::invalid_argument("目标不在攻击范围内");
            else if (!findPlayerById(action.target_id).isAlive())
                throw std::invalid_argument("目标已经死亡");
            std::optional<CardAction> rslt = waitForReact({action.target_id}, {CardType::UNRELENTING, CardType::SLASH});
            if (rslt.has_value()) {
                if (rslt.value().type == CardType::UNRELENTING) {
                    spdlog::info("玩家 {} 对玩家 {} 使用决斗, 已无懈可击", players[currIdx]->id, action.target_id);
                    removeCard(rslt.value());
                }
                else {
                    spdlog::info("玩家 {} 对玩家 {} 使用决斗, 已应战", players[currIdx]->id, action.target_id);
                    removeCard(rslt.value());
                    while (true) {
                        std::optional<CardAction> d_rslt1 = waitForReact({players[currIdx]->id}, {CardType::SLASH});
                        if (d_rslt1.has_value()) {
                            spdlog::info("玩家 {} 在与玩家 {} 决斗中打出杀", players[currIdx]->id, action.target_id);
                            removeCard(d_rslt1.value());
                        } else {
                            spdlog::info("玩家 {} 在与玩家 {} 决斗中失败而受伤", players[currIdx]->id, action.target_id);
                            damage(players[currIdx]->id);
                            break;
                        }
                        std::optional<CardAction> d_rslt2 = waitForReact({action.target_id}, {CardType::SLASH});
                        if (d_rslt2.has_value()) {
                            spdlog::info("玩家 {} 在与玩家 {} 决斗中打出杀", action.target_id, players[currIdx]->id);
                            removeCard(d_rslt1.value());
                        } else {
                            spdlog::info("玩家 {} 在与玩家 {} 决斗中失败而受伤", action.target_id, players[currIdx]->id);
                            damage(action.target_id);
                            break;
                        }
                    }
                }
            }
            else {
                spdlog::info("玩家 {} 对玩家 {} 使用决斗, 未应战而受伤", players[currIdx]->id, action.target_id);
                damage(action.target_id);
            }
        }
        else if (action.type == CardType::ARCHERY_VOLLEY) {
            spdlog::info("玩家 {} 使用万箭齐发", players[currIdx]->id);
            for (size_t i = 1; i < players.size(); ++i) {
                size_t idx = (currIdx + i) % players.size();
                spdlog::info("玩家 {} 被万箭齐发攻击", players[idx]->id);
                std::optional<CardAction> rslt = waitForReact({action.target_id},
                                                              {CardType::UNRELENTING, CardType::DODGE});
                if (rslt.has_value()) {
                    spdlog::info("玩家 {} 对万箭齐发使用 {}", players[idx]->id, CardName[rslt.value().type]);
                    removeCard(rslt.value());
                } else {
                    spdlog::info("玩家 {} 因万箭齐发受伤", players[idx]->id);
                    damage(players[idx]->id);
                }
            }
        }
        else if (action.type == CardType::BARBARIAN) {
            spdlog::info("玩家 {} 使用南蛮入侵", players[currIdx]->id);
            for (size_t i = 1; i < players.size(); ++i) {
                size_t idx = (currIdx + i) % players.size();
                spdlog::info("玩家 {} 被南蛮入侵攻击", players[idx]->id);
                std::optional<CardAction> rslt = waitForReact({action.target_id},
                                                              {CardType::UNRELENTING, CardType::SLASH});
                if (rslt.has_value()) {
                    spdlog::info("玩家 {} 对南蛮入侵使用 {}", players[idx]->id, CardName[rslt.value().type]);
                    removeCard(rslt.value());
                } else {
                    spdlog::info("玩家 {} 因南蛮入侵受伤", players[idx]->id);
                    damage(players[idx]->id);
                }
            }
        }
        else if (action.type == CardType::SLEIGHT_OF_HAND) {
            spdlog::info("玩家 {} 使用无中生有", players[currIdx]->id);
            std::optional<CardAction> rslt = waitForReact(getPlayerList(), {CardType::UNRELENTING});
            if (rslt.has_value()) {
                spdlog::info("玩家 {} 使用无中生有, 被玩家 {} 无懈可击", players[currIdx]->id, action.source_id);
                removeCard(rslt.value());
            }
            else {
                spdlog::info("玩家 {} 使用无中生有", players[currIdx]->id);
                std::vector<CardPtr> card_to_add(2);
                card_to_add.emplace_back(drawCard());
                card_to_add.emplace_back(drawCard());
                for (const auto &card : card_to_add)
                    spdlog::info("id: {} type: {}", card->id, CardName[card->type]);
                players[currIdx]->newCardList(std::move(card_to_add));
            }
        }
        else if (action.type == CardType::HARVEST_FEAST) {
            spdlog::info("玩家 {} 使用五谷丰登", players[currIdx]->id);
            for (auto& player : players) {
                spdlog::debug("玩家 {} 受到五谷丰登", player->id);
                std::vector<CardPtr> card_to_add(2);
                card_to_add.emplace_back(drawCard());
                card_to_add.emplace_back(drawCard());
                for (const auto &card : card_to_add)
                    spdlog::debug("id: {} type: {}", card->id, CardName[card->type]);
                player->newCardList(std::move(card_to_add));
            }
        }
        else if (action.type == CardType::PEACH_GARDEN_OATH) {
            spdlog::info("玩家 {} 使用桃园结义", players[currIdx]->id);
            for (auto& player : players)
                if (player->getHealth() < player->getMaxHealth())
                    player->setHealth(player->getHealth() + 1);
        }
        else {
            throw std::invalid_argument("错误的卡牌使用");
        }
    }

    /// @brief 检查游戏是否结束
    bool GameController::checkWin() {
        size_t alive[4] = {0};
        for (const auto &player : players) {
            if (player->isAlive())
                ++alive[player->getIdentity() - 1];
        }
        GameOver cmd;
        if (alive[0] == 0) {
            // 反贼胜利
            spdlog::info("反贼胜利");
            cmd.set_victorycamp(PlayerIdentity_pb::REBEL);
            broadcast(CommandType::GAME_OVER, cmd.SerializeAsString());
            return true;
        } else if (alive[0] > 0 && alive[2] == 0) {
            // 主公胜利
            spdlog::info("主公胜利");
            cmd.set_victorycamp(PlayerIdentity_pb::LORD);
            broadcast(CommandType::GAME_OVER, cmd.SerializeAsString());
            return true;
        } else if (alive[0] == 0 && alive[1] == 0 && alive[2] == 0 && alive[3] > 0) {
            // 内奸胜利
            broadcast(CommandType::GAME_OVER, "内奸胜利");
            cmd.set_victorycamp(PlayerIdentity_pb::SPY);
            broadcast(CommandType::GAME_OVER, cmd.SerializeAsString());
            return true;
        }
        else if (alive[0] == 0 && alive[1] == 0 && alive[2] == 0 && alive[3] == 0) {
            throw std::runtime_error("没有活着的玩家");
        }
        return false;
    }
}
