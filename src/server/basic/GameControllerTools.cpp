
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

    void GameController::startCommand() {
        /// @brief 发送开始游戏消息
        for (const auto &player : players) {
            GameStart cmd;
            cmd.set_playeridentity(util::to_pb(player->getIdentity()));
            util::sendCommand(player, CommandType::GAME_START, cmd.SerializeAsString());
        }
    }

    std::vector<size_t> GameController::getPlayerList(bool exclude_current) const {
        /// @brief 获取玩家 id 列表
        size_t curr_id = players[currIdx]->id;
        std::vector<size_t> player_list;
        for (const auto &player : players)
            if (exclude_current && player->id != curr_id)
                player_list.emplace_back(player->id);
        return player_list;
    }

    void GameController::broadcast(CommandType commandType, const std::string &msg) {
        /// @brief 广播消息
        /// @param target 目标玩家 id 列表
        /// @param commandType 消息类型
        /// @param msg 消息内容
        for (const auto &player : players) {
            util::sendCommand(player, commandType, msg);
        }
    }

    size_t GameController::nextPlayerIdx() {
        /// @brief 获取下一个玩家的 id
        size_t idx = currIdx;
        for (int i = 1; i <= players.size(); ++i) {
            idx = (currIdx + i) % players.size();
            if (players[idx]->isAlive()) {
                currIdx = idx;
                spdlog::info("下一个玩家: {}", players[idx]->id);
                return idx;
            }
        }
        throw std::runtime_error("没有下一个玩家");
    }

    void GameController::bcStatus() {
        /// @brief 广播游戏状态
        GameStatus cmd;
        cmd.set_totalplayers(players.size());
        for (const auto& player : players) {
            Player_pb player_pb = util::to_pb(*player);
            cmd.add_players()->CopyFrom(player_pb);
        }
        cmd.set_currentturnplayerid(players[currIdx]->id);
        broadcast(CommandType::GAME_STATUS, cmd.SerializeAsString());
    }

    CardPtr GameController::drawCard() {
        /// @brief 抽牌
        CardPtr card = std::move(*cards.begin());
        cards.erase(cards.begin());
        return std::move(card);
    }

    Player& GameController::findPlayerById(size_t id) {
        /// @brief 根据 id 查找玩家
        for (const auto& player : players) {
            if (player->id == id) {
                return *player;
            }
        }
        throw std::invalid_argument("玩家 id 不存在");
    }

    std::any GameController::waitForCard(const std::vector<size_t> &target) {
        /// @brief 等待玩家出牌
        /// @param target 目标玩家 id 列表
        /// @return CardAction / DiscardAction
        std::vector<zmq::pollitem_t> poll_items(target.size());
        for (size_t id : target) {
            Player& rslt = findPlayerById(id);
            poll_items.emplace_back(zmq::pollitem_t{rslt.socket, 0, ZMQ_POLLIN, 0});
        }
        while (TURN_TIME_LIMIT - turn_timer.getTime() > std::chrono::microseconds(0)) {
            zmq::poll(poll_items, std::chrono::duration_cast<std::chrono::milliseconds>(
                    TURN_TIME_LIMIT - turn_timer.getTime()));
            for (size_t i = 0; i < target.size(); ++i) {
                if (poll_items[i].revents & ZMQ_POLLIN) {
                    try {
                        std::string msg;
                        std::optional<CommandType> rslt = util::recvCommand(players[target[i]], msg);
                        if (!rslt.has_value())
                            throw std::runtime_error("接收到空消息");
                        if (rslt.value() == CommandType::ACTION_PLAY) {
                            ActionPlay cmd;
                            cmd.ParseFromString(msg);
                            spdlog::info("玩家 {} 出牌: {} {}", target[i], cmd.card().id(),
                                         CardName[cmd.card().type()]);
                            CardAction action{
                                    cmd.card().id(),
                                    util::to_kc(cmd.card().type()),
                                    target[i],
                                    cmd.targetplayerid(),
                                    cmd.drawcardnumber()
                            };
                            bcCard(action);     // 广播出牌
                            return action;
                        } else if (rslt.value() == CommandType::ACTION_PASS) {
                            ActionPass cmd;
                            cmd.ParseFromString(msg);
                            std::set<size_t> card_ids;
                            for (const auto &card: cmd.discardedcards()) {
                                card_ids.emplace(card.id());
                            }
                            spdlog::info("玩家 {} 弃牌", target[i]);
                            return DiscardAction{
                                    target[i],
                                    std::move(card_ids)
                            };
                        } else
                            throw std::runtime_error("错误的消息类型");
                    } catch (std::exception &e) {
                        spdlog::error("玩家 {} 发送错误信息: {}", target[i], e.what());
                        continue;
                    }
                }
            }
        }
        return std::nullopt;
    }

    bool GameController::isNearby(size_t target_id) {
        /// @brief 判断目标玩家是否在当前玩家的左右
        /// @param target_id 目标玩家 id
        /// @return 是否在左右
        // 要求死的玩家跳过, 直接继续寻找
        for (int i = 1; i < players.size(); ++i) {
            size_t idx_l = (currIdx + players.size() - i) % players.size();
            if (players[idx_l]->isAlive()) {
                if (players[idx_l]->id == target_id)
                    return true;
                break;
            }
        }
        for (int i = 1; i < players.size(); ++i) {
            size_t idx_r = (currIdx + i) % players.size();
            if (players[idx_r]->isAlive()) {
                if (players[idx_r]->id == target_id)
                    return true;
                break;
            }
        }
        return false;
    }

    void GameController::bcCard(const CardAction& action) {
        /// @brief 广播出牌动作
        NoticeCard cmd;
        Card_pb card_pb;
        card_pb.set_id(action.card_id);
        card_pb.set_type(util::to_pb(action.type));
        cmd.set_allocated_card(&card_pb);
        cmd.set_targetplayerid(action.target_id);
        cmd.set_drawcardnumber(action.draw_card_num);
        broadcast(CommandType::NOTICE_CARD, cmd.SerializeAsString());
    }

    std::optional<CardAction> GameController::waitForReact(const std::vector<size_t> &target,
                                         const std::set<CardType> &type) {
        /// @brief 等待玩家反应, 仅在目标玩家可反应时返回
        /// @param target_id 目标玩家 id 列表
        /// @param card_type 可以反应的牌的类型
        /// @return 反应的牌的类型
        std::vector<zmq::pollitem_t> poll_items(target.size());
        std::vector<bool> pass(target.size(), false);
        for (size_t id : target) {
            Player& rslt = findPlayerById(id);
            if (rslt.hasCard(type))
                poll_items.emplace_back(zmq::pollitem_t{rslt.socket, 0, ZMQ_POLLIN, 0});
        }
        zmq::poll(poll_items, std::chrono::duration_cast<std::chrono::milliseconds>(REACT_TIME_LIMIT));
        for (size_t i = 0; i < target.size(); ++i) {
            if (poll_items[i].revents & ZMQ_POLLIN) {
                try {
                    std::string msg;
                    std::optional<CommandType> rslt = util::recvCommand(players[target[i]], msg);
                    if (!rslt.has_value())
                        throw std::runtime_error("接收到空消息");
                    if (rslt.value() == CommandType::ACTION_PLAY) {
                        ActionPlay cmd;
                        cmd.ParseFromString(msg);
                        if (type.find(util::to_kc(cmd.card().type())) == type.end())
                            throw std::runtime_error("错误的反应牌类型");
                        spdlog::info("玩家 {} 出牌: {} {}", target[i], cmd.card().id(), CardName[cmd.card().type()]);
                        CardAction action {
                                cmd.card().id(),
                                util::to_kc(cmd.card().type()),
                                target[i],
                                cmd.targetplayerid(),
                                cmd.drawcardnumber()
                        };
                        bcCard(action);     // 广播出牌
                        return action;
                    }
                    else if (rslt.value() == CommandType::ACTION_PASS) {
                        // 跳过判断
                        pass[i] = true;
                        bool is_pass = true;
                        for (bool p : pass)
                            if (!p) {
                                is_pass = false;
                                break;
                            }
                        if (is_pass)
                            return std::nullopt;
                    }
                    else
                        throw std::runtime_error("错误的消息类型");
                } catch (std::exception &e) {
                    spdlog::error("玩家 {} 发送错误信息: {}", target[i], e.what());
                    continue;
                }
            }
        }
        return std::nullopt;
    }

    void GameController::removeCard(size_t player_id, size_t card_id, std::optional<CardType> type_check) {
        /// @brief 从玩家手牌中移除一张牌
        /// @param player_id 玩家 id
        /// @param card_id 牌 id
        /// @param type_check 牌类型检查
        CardPtr card = findPlayerById(player_id).removeCard(card_id);
        if (type_check.has_value() && card->type != type_check.value())
            throw std::invalid_argument("出牌类型不匹配");
        spdlog::debug("玩家 {} 移除手牌: {} {}", player_id, card_id, CardName[card->type]);
        cards.emplace_back(std::move(card));
    }

    void GameController::removeCard(const kc::CardAction &action) {
        /// @brief 根据出牌移除卡牌
        removeCard(action.source_id, action.card_id, action.type);
    }

    void GameController::damage(size_t player_id, size_t damage) {
        /// @brief 对玩家造成伤害
        /// @param player_id 玩家 id
        /// @param damage 伤害值
        Player& target = findPlayerById(player_id);
        if (!target.isAlive())
            throw std::invalid_argument("玩家已死亡");
        if (target.getHealth() - damage <= 0) {
            // 公告濒死状态
            NoticeDying cmd_dying;
            cmd_dying.set_playerid(player_id);
            broadcast(CommandType::NOTICE_DYING, cmd_dying.SerializeAsString());
            // 等待玩家反应
            std::optional<CardAction> action = waitForReact(getPlayerList(),
                                                            {CardType::PEACH, CardType::PEACH_GARDEN_OATH});
            if (action.has_value()) {
                if (action.value().type == CardType::PEACH) {
                    spdlog::info("玩家 {} 使用桃, 救了玩家 {}", action.value().source_id, player_id);
                    removeCard(action.value());
                    target.setHealth(1);
                }
                else if (action.value().type == CardType::PEACH_GARDEN_OATH) {
                    spdlog::info("玩家 {} 使用桃园结义, 救了玩家 {}", action.value().source_id, player_id);
                    removeCard(action.value());
                    target.setHealth(0);
                    for (auto& player : players)
                        if (player->getHealth() < player->getMaxHealth())
                            player->setHealth(player->getHealth() + 1);
                }
            }
            else {
                std::vector<CardPtr> card_to_add = target.die();
                for (auto& card : card_to_add)
                    cards.emplace_back(std::move(card));
                // 公告死亡
                NoticeDead cmd_dead;
                cmd_dead.set_playerid(player_id);
                broadcast(CommandType::NOTICE_DEAD, cmd_dead.SerializeAsString());
                spdlog::info("玩家 {} 死亡", player_id);
            }
        }
        else {
            target.setHealth(target.getHealth() - damage);
            spdlog::info("玩家 {} 受到 {} 点伤害, 剩余 {} 点生命值", player_id, damage, target.getHealth());
        }
    }
}

