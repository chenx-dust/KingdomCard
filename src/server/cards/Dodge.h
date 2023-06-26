
#ifndef KINGDOMCARD_DODGE_H
#define KINGDOMCARD_DODGE_H

#include "basic/Card.h"

class Attack : virtual public Card {
public:
    Attack() : Card(false, true, DODGE, SELF) {};
};

#endif //KINGDOMCARD_DODGE_H
