#ifndef WOLF_HPP
#define WOLF_HPP
#include "Carnivore.hpp"

struct Wolf { Carnivore parentStruct; };

void Wolf_init(Wolf* w, int x, int y, int*** map);

#endif