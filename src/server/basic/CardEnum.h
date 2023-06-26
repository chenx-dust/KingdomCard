
#ifndef KINGDOMCARD_CARDENUM_H
#define KINGDOMCARD_CARDENUM_H

#include <memory>
#include <vector>
#include <functional>

#include "basic/Card.h"

// 生成卡牌的 lambda
#define GenL(TYPE) []() -> CardPtr { return std::make_unique<TYPE>(); }

extern size_t const card_num;
extern std::vector<std::function<CardPtr()>> const card_lambdas;



#endif //KINGDOMCARD_CARDENUM_H
