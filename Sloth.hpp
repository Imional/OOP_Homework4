#ifndef SLOTH_HPP
#define SLOTH_HPP
#include "Herbivore.hpp"

struct Sloth {
    Herbivore parentStruct;
    bool isSkipping; // State to track turn skipping
};

void Sloth_observe(Animal* self);
int Sloth_move(Animal* self);
Animal* Sloth_giveBirth(Animal* self);

#endif // SLOTH_HPP
