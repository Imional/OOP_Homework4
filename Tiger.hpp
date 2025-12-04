#ifndef TIGER_HPP
#define TIGER_HPP
#include "Carnivore.hpp"

struct Tiger {
    Carnivore parentStruct;
};

Animal* Tiger_giveBirth(Animal* self);

#endif // TIGER_HPP
