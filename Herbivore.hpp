#ifndef HERBIVORE_HPP
#define HERBIVORE_HPP
#include "Animal.hpp"

struct Herbivore {
    Animal parent;
};

void Herbivore_observe(Animal* self);
int Herbivore_move(Animal* self);
Animal* Herbivore_giveBirth(Animal* self);

#endif // HERBIVORE_HPP
