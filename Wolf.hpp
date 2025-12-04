#ifndef WOLF_HPP
#define WOLF_HPP
#include "Carnivore.hpp"

struct Wolf {
    Carnivore parentStruct;
};

Animal* Wolf_giveBirth(Animal* self);

#endif // WOLF_HPP
