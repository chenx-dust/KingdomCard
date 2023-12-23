//
// Created by 10315 on 2023/12/19.
//

#ifndef KINGDOM_CARD_COMMANDS_H
#define KINGDOM_CARD_COMMANDS_H



enum SIGNALS {

    // server signals
    GAME_START,
    GAME_STATUS,
    NOTICE_CARD,
    NOTICE_DYING,
    NOTICE_DEAD,
    GAME_OVER,
    YOUR_TURN,
    NEW_CARD,
    DISCARD_CARD,
    ACTION_PLAY,
    ACTION_PASS,

    CONNECT_REP,
    CONNECT_ACK,
    KICK,

    // client signals
    CONNECT_REQ,
    CONNECT_INTERRUPTED,
    HAND_CARD,
};



#endif //KINGDOM_CARD_COMMANDS_H
