#ifndef OMNIVORE_HPP
#define OMNIVORE_HPP
#include "Animal.hpp"

struct Omnivore { Animal parent; };

void Omnivore_init(Omnivore* o, int x, int y, int*** map);

#endif