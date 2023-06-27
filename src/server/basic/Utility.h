
#ifndef KINGDOMCARD_UTILITY_H
#define KINGDOMCARD_UTILITY_H

#include "basic/Player.h"
#include "basic_message.pb.h"

namespace util {
    CommandType getCommandType(const BasicMessage& message);
    bool sendCommand(const kc::PlayerPtr& player, CommandType commandType, const std::string& message = "");
    typedef std::optional<CommandType> RecvResult;
    RecvResult recvCommand(const kc::PlayerPtr& player);
    RecvResult recvCommand(const kc::PlayerPtr& player, std::string& message);
}

#endif //KINGDOMCARD_UTILITY_H
