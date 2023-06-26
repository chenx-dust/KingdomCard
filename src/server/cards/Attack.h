
#ifndef KINGDOMCARD_ATTACK_H
#define KINGDOMCARD_ATTACK_H


#include "basic/Card.h"

class Attack : virtual public Card {
public:
    Attack() : Card(true, false, ATTACK, SINGLE) {};
    void useActively(Player& target) override;
};


#endif //KINGDOMCARD_ATTACK_H
