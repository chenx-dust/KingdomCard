
#include "cards/Heal.h"
#include "basic/Player.h"

void Heal::useActively(Player& target) {
    target.plusHealthPoint(1);
}
