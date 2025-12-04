#ifndef OMNIVORE_HPP
#define OMNIVORE_HPP
#include "Animal.hpp"

struct Omnivore {
    Animal parent;
};

void Omnivore_observe(Animal* self);
int Omnivore_move(Animal* self);
Animal* Omnivore_giveBirth(Animal* self);

#endif // OMNIVORE_HPP
