#ifndef GIRAFFE_HPP
#define GIRAFFE_HPP
#include "Herbivore.hpp"

struct Giraffe {
    Herbivore parentStruct;
};

Animal* Giraffe_giveBirth(Animal* self);

#endif // GIRAFFE_HPP
