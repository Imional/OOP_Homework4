#ifndef CARNIVORE_HPP
#define CARNIVORE_HPP
#include "Animal.hpp"

struct Carnivore {
    Animal parent;
};

void Carnivore_observe(Animal* self);
int Carnivore_move(Animal* self);
Animal* Carnivore_giveBirth(Animal* self);

#endif // CARNIVORE_HPP
