#ifndef CARNIVORE_HPP
#define CARNIVORE_HPP

#include "Animal.hpp"

struct Carnivore {
    Animal parent;
};

void Carnivore_init(Carnivore* c, int x, int y, int*** map);

#endif