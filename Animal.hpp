#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include "Utils.hpp"

struct Animal {
    // Attributes
    Location loc;
    int energy;
    int movingCost;
    int birthThreshold;
    int birthCost;
    int energyValue;
    int viewRange;
    
    int** viewArray; // 2D array allocated dynamically
    int*** map;      // Pointer to the global map pointer

    // Helpers for ID tracking (needed for map updates)
    int animalID;    
    int typeID;      // 0:Herb, 1:Carn, 2:Omni (used for basic ID generation)

    // Behavior Function Pointers
    void (*observe)(Animal* self);
    int (*move)(Animal* self); // Returns index of eaten animal, 100000 if none.
    Animal* (*giveBirth)(Animal* self);

    // Helper to free viewArray
    void cleanup() {
        if (viewArray) {
            int size = 2 * viewRange + 1;
            for (int i = 0; i < size; ++i) {
                delete[] viewArray[i];
            }
            delete[] viewArray;
            viewArray = nullptr;
        }
    }
};

#endif // ANIMAL_HPP
