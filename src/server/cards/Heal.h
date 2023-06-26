
#ifndef KINGDOMCARD_HEAL_H
#define KINGDOMCARD_HEAL_H

#include "basic/Card.h"

class Heal : virtual public Card {
public:
    Heal() : Card(true, false, HEAL, SELF) {};
    void useActively(Player& target) override;
};

#endif //KINGDOMCARD_HEAL_H
