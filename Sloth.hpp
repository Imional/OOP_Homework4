#ifndef SLOTH_HPP
#define SLOTH_HPP
#include "Herbivore.hpp"

struct Sloth {
    Herbivore parentStruct;
    bool canMove; 
};

void Sloth_init(Sloth* s, int x, int y, int*** map);

#endif