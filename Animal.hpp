#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include "Utils.hpp"
#include <vector>

// Forward declaration
struct Animal;

// Function pointer typedefs
typedef void (*ObserveFunc)(Animal* self);
typedef int (*MoveFunc)(Animal* self); // Returns index of eaten animal, or special code
typedef Animal* (*GiveBirthFunc)(Animal* self);

struct Animal {
    [cite_start]// Attributes [cite: 93-107, 135-144]
    Location loc;
    int energy;
    int movingCost;
    int birthThreshold;
    int birthCost;
    int energyValue;
    int viewRange;
    
    // 2D array for scores. 
    // We will allocate this dynamically based on viewRange.
    int** viewArray;
    
    // Pointer to the global map
    int*** worldMap; // Pointer to the raw 2D array (FieldGrid is int[50][50])

    [cite_start]// Behaviors [cite: 146-163]
    ObserveFunc observe;
    MoveFunc move;
    GiveBirthFunc giveBirth;

    // Helper to identify type for map encoding (e.g., HERBIVORE_MARKER)
    int marker; 
};

// Generic behaviors implemented in Animal.cpp
int Animal_move(Animal* self);
Animal* Animal_giveBirth(Animal* self);

// Helper to free memory
void Animal_delete(Animal* self);

#endif // ANIMAL_HPP