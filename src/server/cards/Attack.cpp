
#include "cards/Attack.h"
#include "basic/Player.h"

void Attack::useActively(Player &target) {
    target.minusHealthPoint(1);
}
