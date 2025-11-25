#ifndef GIRAFFE_HPP
#define GIRAFFE_HPP
#include "Herbivore.hpp"

struct Giraffe { Herbivore parentStruct; };
void Giraffe_init(Giraffe* g, int x, int y, int*** map);

#endif