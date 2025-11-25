#ifndef HERBIVORE_HPP
#define HERBIVORE_HPP

#include "Animal.hpp"

struct Herbivore {
    Animal parent;
};

void Herbivore_init(Herbivore* h, int x, int y, int*** map);

#endif