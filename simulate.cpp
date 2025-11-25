#include "Utils.hpp"
#include "SimulationHelper.hpp"
#include "Animal.hpp"
#include "Herbivore.hpp"
#include "Carnivore.hpp"
#include "Giraffe.hpp"
#include "Sloth.hpp"
#include "Tiger.hpp"
#include "Wolf.hpp"
#include "Omnivore.hpp" // Extra Credit 1.3

#include <iostream>
#include <vector>
#include <algorithm>

/*****************************************
YOU MUST EDIT THE STUDENT ID BELOW!!!
******************************************/
std::string studentID = "2025150009";
/*****************************************
YOU MUST EDIT THE STUDENT ID ABOVE!!!
******************************************/

const int SIMULATE_ITERATION_COUNT = 100000;
extern int globalMap[FIELD_SIZE][FIELD_SIZE]; // Defined in SimulationHelper usually, or we define here if needed.
int globalMap[FIELD_SIZE][FIELD_SIZE];

int main() {
    // 1. Prepare Simulation
    // Pass the global map to the helper to initialize grass and random seed
    Simulation_prepare(studentID, globalMap);

    std::vector<Animal*> animals;

    // 2. Spawn Animals
    // This function (implemented in modified SimulationHelper.cpp below) 
    // will now spawn Herbivores, Carnivores, Giraffes, Sloths, Tigers, Wolves, and Omnivores.
    Simulation_spawnInitialAnimals(globalMap, animals);

    // 3. Main Simulation Loop
    for (int iter = 0; iter < SIMULATE_ITERATION_COUNT; ++iter) {
        std::vector<Animal*> newBabies;
        std::vector<int> deadIndices;

        // --- A. ACTION PHASE ---
        for (size_t i = 0; i < animals.size(); ++i) {
            Animal* currentAnimal = animals[i];

            // 1. Observe
            currentAnimal->observe(currentAnimal);

            // 2. Move
            // Returns: 100000 (nothing), -100000 (died hunger), or index of eaten animal
            int actionResult = currentAnimal->move(currentAnimal);

            bool i_died = false;

            if (actionResult != 100000) {
                if (actionResult < 0) {
                    // Current animal died of hunger
                    deadIndices.push_back(i);
                    i_died = true;
                } else {
                    [cite_start]// Current animal ate the animal at index `actionResult` [cite: 155]
                    deadIndices.push_back(actionResult);
                    // Grant energy for eating (Standard survival logic, though not explicitly quantified in PDF text, implied by "preference is energyValue")
                    currentAnimal->energy += 5; 
                }
            }

            // 3. Give Birth
            // Only if the current animal is still alive
            if (!i_died) {
                // We need to check if 'i' was already eaten by a previous mover.
                bool alreadyDead = false;
                for(int d : deadIndices) { if(d == (int)i) alreadyDead = true; }
                
                if (!alreadyDead) {
                    Animal* baby = currentAnimal->giveBirth(currentAnimal);
                    if (baby != nullptr) {
                        newBabies.push_back(baby);
                    }
                }
            }
        }

        // --- B. CLEANUP PHASE ---
        // Sort dead indices descending to remove safely
        std::sort(deadIndices.rbegin(), deadIndices.rend());
        // Remove duplicates (in case of weird edge cases)
        deadIndices.erase(std::unique(deadIndices.begin(), deadIndices.end()), deadIndices.end());

        for (int idx : deadIndices) {
            if (idx >= 0 && idx < (int)animals.size()) {
                Animal* dead = animals[idx];
                
                // Cleanup Map: Set location to Grass (1) if it still holds the dead animal's marker
                // This prevents "ghost" barriers.
                int mx = dead->loc.x;
                int my = dead->loc.y;
                int content = globalMap[mx][my];
                // Check if map actually points to this specific animal index
                // Decoding logic: abs(content % 100000) == idx
                if (content < 0 && std::abs(content % 100000) == idx) {
                    globalMap[mx][my] = 1; // Reset to grass
                }

                // Free memory
                if (dead->viewArray) {
                    int width = 2 * dead->viewRange + 1;
                    for(int k=0; k < width; ++k) delete[] dead->viewArray[k];
                    delete[] dead->viewArray;
                }
                delete dead;
                
                animals.erase(animals.begin() + idx);
            }
        }

        // --- C. NEW ARRIVALS ---
        for (Animal* baby : newBabies) {
            animals.push_back(baby);
        }

        // --- D. MAP REFRESH ---
        // Indices in `animals` vector have shifted due to erase.
                
        // Safest: Iterate vector and force-update map. 
        for (size_t k = 0; k < animals.size(); ++k) {
            Animal* a = animals[k];
            // Re-encode: Marker * 100000 + index
            // Value = Marker * 100000
            // If marker < 0 (always is), Value is like -1000000.
            int base = a->marker * 100000;
            // The encoding in Utils is: baseValue + (marker < 0 ? -index : index)
            int val = base + (a->marker < 0 ? -((int)k) : ((int)k));
            globalMap[a->loc.x][a->loc.y] = val;
        }

        // --- E. GRASS GROWTH ---
        for (int x = 0; x < FIELD_SIZE; ++x) {
            for (int y = 0; y < FIELD_SIZE; ++y) {
                if (globalMap[x][y] > 0 && globalMap[x][y] < 5) {
                    globalMap[x][y]++; [cite_start]// Grow grass [cite: 201]
                }
            }
        }
    }

    // --- F. FINAL SCORE CALCULATION ---
    long long finalVal = 0;
    for (int i = 1; i < FIELD_SIZE * FIELD_SIZE; ++i) {
        int x = (i - 1) % FIELD_SIZE;
        int y = (i - 1) / FIELD_SIZE;
        finalVal += std::abs(i * globalMap[x][y]);
        finalVal %= 100000000;
    }

    std::cout << "test1 result: " << finalVal << std::endl;

    return 0;
}