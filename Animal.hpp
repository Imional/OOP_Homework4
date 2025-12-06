#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include "Utils.hpp"

struct Animal {
    Location loc;
    int energy;
    int movingCost;
    int birthThreshold;
    int birthCost;
    int energyValue;
    int viewRange;
    
    int** viewArray;
    int*** map;
    
    int animalID; // Used for map tracking
    int typeID;   // 1=Herb, 2=Carn, etc.

    void (*observe)(Animal* self);
    int (*move)(Animal* self); // Returns index of eaten animal, or 100000.
    Animal* (*giveBirth)(Animal* self);

    void cleanup() {
        if (viewArray) {
            int size = 2 * viewRange + 1;
            for(int i=0; i<size; ++i) delete[] viewArray[i];
            delete[] viewArray;
            viewArray = nullptr;
        }
    }
};

#endif
