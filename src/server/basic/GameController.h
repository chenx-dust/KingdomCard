
#ifndef KINGDOMCARD_GAMECONTROLLER_H
#define KINGDOMCARD_GAMECONTROLLER_H

#include <vector>

#include "basic/Player.h"
#include "basic/Card.h"

class GameController {
private:
    std::vector<Player> players;
    std::vector<CardPtr> card_base;
    std::vector<CardPtr> card_trash;
public:
    void init(std::vector<Player>&& players);
    void startTurn();
    std::vector<CardPtr>& getCardBase();
    std::vector<CardPtr>& getCardTrash();
};


#endif //KINGDOMCARD_GAMECONTROLLER_H
