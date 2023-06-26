
#include "basic/CardEnum.h"
#include "cards/Attack.h"

size_t const card_num = 0;
std::vector<std::function<CardPtr()>> const card_lambdas = {
        GenL(Attack)
};
