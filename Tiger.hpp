#ifndef TIGER_HPP
#define TIGER_HPP
#include "Carnivore.hpp"

struct Tiger { Carnivore parentStruct; };

void Tiger_init(Tiger* t, int x, int y, int*** map);

#endif