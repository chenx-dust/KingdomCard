
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
}

namespace util {
    bool sendCommand(const kc::PlayerPtr& player, CommandType commandType, const std::string& message = "");
    bool sendCommand(kc::Player *player, CommandType commandType, const std::string &message);
    typedef std::optional<CommandType> RecvResult;
    RecvResult recvCommand(const kc::PlayerPtr& player);
    RecvResult recvCommand(const kc::PlayerPtr& player, std::string& message);

    PlayerIdentity_pb to_pb(kc::PlayerIdentity identity);
    CardType_pb to_pb(kc::CardType type);
    Player_pb to_pb(const kc::Player& player);
    Card_pb to_pb(const kc::Card& card);
    kc::PlayerIdentity to_kc(PlayerIdentity_pb identity);
    kc::CardType to_kc(CardType_pb type);

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
