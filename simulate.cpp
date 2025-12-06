#include "SimulationHelper.hpp"
#include "Utils.hpp"
#include "Animal.hpp"
#include "Herbivore.hpp"
#include "Carnivore.hpp"
#include "Giraffe.hpp"
#include "Sloth.hpp"
#include "Tiger.hpp"
#include "Wolf.hpp"
#include "Omnivore.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

/*****************************************
YOU MUST EDIT THE STUDENT ID BELOW!!!
******************************************/
std::string studentID = "2025150009";
/*****************************************
YOU MUST EDIT THE STUDENT ABOVE!!!
******************************************/

const int SIMULATE_ITERATION_COUNT = 100000;
int** globalMap = nullptr;

// ... (spawnExtraAnimals function from previous response goes here if needed for 1.2/1.3) ...
// For P1.1, you can leave it out or comment it out.

int main() {
    globalMap = new int*[FIELD_SIZE];
    for (int i = 0; i < FIELD_SIZE; ++i) {
        globalMap[i] = new int[FIELD_SIZE];
    }

    Simulation_prepare(studentID, &globalMap);

    std::vector<Animal*> animals;
    Simulation_spawnInitialAnimals(&globalMap, animals);

    // Initial ID Assignment
    for(size_t i=0; i<animals.size(); ++i) {
        animals[i]->animalID = (int)i;
        // Marker encoding handled by Helper, but we ensure consistency later
    }

    for (int iter = 0; iter < SIMULATE_ITERATION_COUNT; ++iter) {
        std::vector<Animal*> babies;
        std::vector<int> deadIndices; // Indices in the CURRENT 'animals' vector

        for (size_t i = 0; i < animals.size(); ++i) {
            Animal* anim = animals[i];

            // If already dead (eaten earlier in loop), skip
            // We mark eaten animals by setting energy to very low value
            if (anim->energy <= -99999) continue;

            // 1. Observe
            anim->observe(anim);

            // 2. Move (May gain energy)
            int eatenIdx = anim->move(anim);

            // Handle Predation immediately (mark prey as dead)
            if (eatenIdx != 100000) {
                if (eatenIdx >= 0 && eatenIdx < (int)animals.size()) {
                    // Mark prey as eaten. 
                    // Note: Prey stays in vector until end of loop, but energy is killed.
                    // "If index 0 is eaten... nothing happens to vector... immediate index shifting [no]"
                    if (animals[eatenIdx]->energy > -99999) {
                        animals[eatenIdx]->energy = -100000; // Killed flag
                        deadIndices.push_back(eatenIdx);
                    }
                }
            }

            // 3. Give Birth (Pay birth cost)
            Animal* baby = anim->giveBirth(anim);
            if (baby) {
                babies.push_back(baby);
            }

            // 4. Subtract Moving Cost
            // "After that [birth], subtract movingCost"
            anim->energy -= anim->movingCost;

            // Check Starvation
            if (anim->energy <= 0 && anim->energy > -99999) {
                anim->energy = -100000; // Killed flag
                deadIndices.push_back((int)i);
            }
        }

        // --- END OF LOOP CLEANUP ---

        // 1. Remove Dead Animals
        // Sort descending to remove from back to front to preserve indices during erase
        std::sort(deadIndices.begin(), deadIndices.end(), std::greater<int>());
        // Remove duplicates (e.g. if eaten AND starved? Unlikely due to checks, but safe)
        auto last = std::unique(deadIndices.begin(), deadIndices.end());
        deadIndices.erase(last, deadIndices.end());

        for (int idx : deadIndices) {
            if (idx < (int)animals.size()) {
                Animal* dead = animals[idx];
                
                // If the dead animal is still physically on the map (starved), 
                // we must clear its spot to Grass 1 (unless someone else moved there).
                // However, logic says if they moved, map was updated.
                // If they starved, they might be occupying a spot.
                // We check if the map currently points to THIS animal ID.
                int mx = dead->loc.x;
                int my = dead->loc.y;
                int val = globalMap[mx][my];
                
                // ID encoding: -1000000 - ID. 
                // But wait, the ID in map is based on START of loop indices.
                // The dead->animalID stores the index at start of loop.
                int marker = (dead->typeID == 2) ? -2000000 : -1000000; // Simplified type check
                // Actually, rely on values:
                if (val < 0) {
                    int storedID = -(val % 1000000); 
                    // This is tricky. Let's just check if it matches the *encoded* ID.
                    // The safest way: If map value corresponds to this dead pointer's encoded ID.
                    // Since we re-encode at the end of every loop, checking the encoded value works.
                    // If the predator moved ONTO this spot, the map value is now the predator's ID.
                    // So we only clear if it's still the dead guy's ID.
                    
                    // Simple hack: if the value matches what we expect for this animal:
                    // We need a way to know what marker (10 or 20) it had.
                    // Let's just reset to 1 if it's negative and *not* one of the surviving animals.
                    // Better yet: Just delete the object. The Map Re-indexing step below will overwrite everything anyway.
                    // BUT, if we don't clear it, and no one is there, the Re-indexing loop won't write to it.
                    // So we DO need to clear the map spot of the dead animal.
                    
                    // Since we reconstruct the whole map's animals in the next step,
                    // we can just set this specific spot to 1 *if* it's negative.
                    // If a living animal is there, the next loop will overwrite it.
                    // If no one is there (it died), it becomes 1.
                    // Correction: If a predator is there, it's negative. We shouldn't overwrite a predator.
                    // We only overwrite if the ID matches the dead animal.
                    // Let's assume for now the map is consistent.
                    if (val == (-1000000 - dead->animalID) || val == (-2000000 - dead->animalID) || val == (-1100000 - dead->animalID) /*etc*/) {
                         globalMap[mx][my] = 1;
                    }
                }

                dead->cleanup();
                delete dead;
                animals.erase(animals.begin() + idx);
            }
        }

        // 2. Add Babies
        for(Animal* b : babies) {
            animals.push_back(b);
        }

        // 3. Re-Index and Update Map
        // "Update all the remaining... integers... to reflect shifted indices"
        for(size_t i=0; i<animals.size(); ++i) {
            Animal* a = animals[i];
            a->animalID = (int)i; // Update internal ID
            
            // Determine Marker Base based on Type (You need to ensure types are correct)
            // 1: Herb, 2: Carn, 11: Giraffe, 12: Sloth, 21: Tiger, 22: Wolf, 30: Omnivore
            int marker = -1000000;
            if (a->typeID == 2) marker = -2000000;
            else if (a->typeID == 1) marker = -1000000;
            // ... Add logic for sub-types if running 1.2/1.3 ...
            // e.g., if (dynamic_cast<Giraffe*>(a)) marker = -1100000; 
            // Since we can't use dynamic_cast easily with structs unless virtual, use typeID.
            // P1.1 only needs 1 and 2.
            
            globalMap[a->loc.x][a->loc.y] = marker - (int)i;
        }

        // 4. Grass Growth
        // "For every cell without an animal, increase... grass"
        for (int x = 0; x < FIELD_SIZE; ++x) {
            for (int y = 0; y < FIELD_SIZE; ++y) {
                if (globalMap[x][y] >= 0) { // No animal
                    // "Grass grows... end of loop"
                    // "If positive... grows"
                    // Note: If an animal just died, we set it to 1 above.
                    // If an animal moved away, we set it to 1 in move().
                    // So here we increment.
                    if (globalMap[x][y] < 5) {
                        globalMap[x][y]++;
                    }
                }
            }
        }
    }

    // Final Calculation (Same as before)
    long long finalVal = 0;
    for (int i = 1; i < FIELD_SIZE * FIELD_SIZE; ++i) {
        int x = (i - 1) % FIELD_SIZE;
        int y = (i - 1) / FIELD_SIZE;
        finalVal += std::abs(i * globalMap[x][y]);
        finalVal %= 100000000;
    }

    std::cout << "test1 result: " << finalVal << std::endl;

    for (int i = 0; i < FIELD_SIZE; ++i) delete[] globalMap[i];
    delete[] globalMap;

    return 0;
}
