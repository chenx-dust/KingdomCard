
#ifndef KINGDOMCARD_UTILITY_H
#define KINGDOMCARD_UTILITY_H

#include <optional>
#include "basic/Player.h"
#include "basic/Card.h"
#include "basic_message.pb.h"
#include "basic_object.pb.h"
#include "command.pb.h"

namespace kc {
    class CardAction;
    enum class TurnType {
        ACTIVE,               // 主动出牌
        PASSIVE,              // 被动出牌
        PASSIVE_SLASH,        // 被动或者杀
        PASSIVE_DODGE,        // 被动或者闪
        DUELING,              // 决斗中
        DODGE_WAIT,           // 闪等待
        DYING,                // 濒死
    };
    const std::unordered_map<TurnType, std::set<CardType>> TurnCardsAvailable = {
            {TurnType::ACTIVE, {CardType::SLASH, CardType::DISMANTLE, CardType::STEAL, CardType::ARCHERY_VOLLEY,
                                       CardType::BARBARIAN, CardType::SLEIGHT_OF_HAND, CardType::HARVEST_FEAST,
                                       CardType::PEACH, CardType::PEACH_GARDEN_OATH, CardType::DUEL}},
            {TurnType::PASSIVE, {CardType::UNRELENTING}},
            {TurnType::PASSIVE_SLASH, {CardType::UNRELENTING, CardType::SLASH}},
            {TurnType::PASSIVE_DODGE, {CardType::UNRELENTING, CardType::DODGE}},
            {TurnType::DUELING, {CardType::SLASH}},
            {TurnType::DODGE_WAIT, {CardType::DODGE}},
            {TurnType::DYING, {CardType::PEACH, CardType::PEACH_GARDEN_OATH}}
    };
}

namespace util {
    bool sendCommand(kc::Player& player, CommandType commandType, const std::string& message = "",
                     std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));
    bool sendCommand(const kc::PlayerPtr& player, CommandType commandType, const std::string& message = "",
                     std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));
    bool sendCommand(kc::Player *player, CommandType commandType, const std::string &message,
                     std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));
    typedef std::optional<CommandType> RecvResult;
    RecvResult recvCommand(const kc::PlayerPtr& player);
    RecvResult recvCommand(const kc::PlayerPtr& player, std::string& message,
                           std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));
    RecvResult recvCommand(kc::Player& player, std::string& message,
                           std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));

    PlayerIdentity_pb to_pb(kc::PlayerIdentity identity);
    CardType_pb to_pb(kc::CardType type);
    Player_pb to_pb(const kc::Player& player);
    Card_pb to_pb(const kc::Card& card);
    TurnType_pb to_pb(kc::TurnType type);
    kc::PlayerIdentity to_kc(PlayerIdentity_pb identity);
    kc::CardType to_kc(CardType_pb type);
    kc::TurnType to_kc(TurnType_pb type);

    class Timer {
    private:
        std::chrono::steady_clock::time_point startTime;
        std::chrono::microseconds total {0};
        bool isTiming {false};
    public:
        void start();
        void pause();
        void reset();
        [[nodiscard]] std::chrono::microseconds getTime() const;
    };
}

#endif //KINGDOMCARD_UTILITY_H
