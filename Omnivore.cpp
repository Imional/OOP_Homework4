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
#include <cmath>
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

// Helper to spawn additional animals (1.2/1.3)
void spawnExtraAnimals(std::vector<Animal*>& animals, int count, int type) {
    // Find free cells
    std::vector<Location> freeCells;
    for(int x=0; x<FIELD_SIZE; ++x) {
        for(int y=0; y<FIELD_SIZE; ++y) {
             // Simple check: positive grass or 0.
             // Existing animals are < 0.
             if (globalMap[x][y] >= 0) freeCells.push_back({x, y});
        }
    }
    
    for(int i=0; i<count; ++i) {
        if(freeCells.empty()) break;
        Location loc = randomSelection(freeCells);
        // Remove chosen loc from freeCells (inefficient but safe for init)
        for(size_t k=0; k<freeCells.size(); ++k) {
            if(freeCells[k].x == loc.x && freeCells[k].y == loc.y) {
                freeCells.erase(freeCells.begin() + k);
                break;
            }
        }
        
        Animal* a = nullptr;
        int marker = 0;
        
        if (type == 1) { // Giraffe
            Giraffe* g = new Giraffe();
            a = (Animal*)g;
            a->loc = loc; a->energy=5; a->movingCost=2; a->birthThreshold=15; a->birthCost=5;
            a->energyValue=5; a->viewRange=2; a->map=&globalMap; a->typeID=1; a->viewArray=nullptr;
            a->observe = Herbivore_observe; a->move = Herbivore_move; a->giveBirth = Giraffe_giveBirth;
            marker = -11;
        } else if (type == 2) { // Sloth
            Sloth* s = new Sloth();
            s->isSkipping = false;
            a = (Animal*)s;
            a->loc = loc; a->energy=5; a->movingCost=1; a->birthThreshold=15; a->birthCost=5;
            a->energyValue=5; a->viewRange=1; a->map=&globalMap; a->typeID=1; a->viewArray=nullptr;
            a->observe = Sloth_observe; a->move = Sloth_move; a->giveBirth = Sloth_giveBirth;
            marker = -12;
        } else if (type == 3) { // Tiger
            Tiger* t = new Tiger();
            a = (Animal*)t;
            a->loc = loc; a->energy=5; a->movingCost=4; a->birthThreshold=40; a->birthCost=30;
            a->energyValue=5; a->viewRange=2; a->map=&globalMap; a->typeID=2; a->viewArray=nullptr;
            a->observe = Carnivore_observe; a->move = Carnivore_move; a->giveBirth = Tiger_giveBirth;
            marker = -21;
        } else if (type == 4) { // Wolf
            Wolf* w = new Wolf();
            a = (Animal*)w;
            a->loc = loc; a->energy=5; a->movingCost=2; a->birthThreshold=40; a->birthCost=30;
            a->energyValue=5; a->viewRange=1; a->map=&globalMap; a->typeID=2; a->viewArray=nullptr;
            a->observe = Carnivore_observe; a->move = Carnivore_move; a->giveBirth = Wolf_giveBirth;
            marker = -22;
        } else if (type == 5) { // Omnivore
            Omnivore* o = new Omnivore();
            a = (Animal*)o;
            a->loc = loc; a->energy=5; a->movingCost=1; a->birthThreshold=40; a->birthCost=20;
            a->energyValue=5; a->viewRange=1; a->map=&globalMap; a->typeID=3; a->viewArray=nullptr;
            a->observe = Omnivore_observe; a->move = Omnivore_move; a->giveBirth = Omnivore_giveBirth;
            marker = -30;
        }
        
        if(a) {
            animals.push_back(a);
            a->animalID = animals.size()-1; // temporary
            // Encode: Marker * 10000 + Index
            int encoded = marker * 10000 - (int)(animals.size()-1); 
            globalMap[loc.x][loc.y] = encoded;
        }
    }
}

int main() {
    globalMap = new int*[FIELD_SIZE];
    for (int i = 0; i < FIELD_SIZE; ++i) {
        globalMap[i] = new int[FIELD_SIZE];
    }

    Simulation_prepare(studentID, &globalMap);

    std::vector<Animal*> animals;
    Simulation_spawnInitialAnimals(&globalMap, animals);

    // Add Problem 1.2 and 1.3 Animals
    // Note: SimulationHelper spawns 200 Herbivores and 25 Carnivores.
    // We add others:
    spawnExtraAnimals(animals, GIRAFFE_COUNT, 1);
    spawnExtraAnimals(animals, SLOTH_COUNT, 2);
    spawnExtraAnimals(animals, TIGER_COUNT, 3);
    spawnExtraAnimals(animals, WOLF_COUNT, 4);
    spawnExtraAnimals(animals, 50, 5); // 50 Omnivores for Extra Credit

    // Re-Index everything to be safe
    for(size_t i=0; i<animals.size(); ++i) {
        animals[i]->animalID = i;
        // Determine marker based on type pointer or heuristics
        // Simplified: use generic type ID or careful encoding.
        // For this assignment, we rely on the object's logic to maintain marker consistency
        // But initial map set needs to be correct.
        int marker = -10;
        // Logic to recover marker (simplified for length constraints)
        // We assume spawn functions set it correctly. 
        // We just update index:
        int val = globalMap[animals[i]->loc.x][animals[i]->loc.y];
        int baseMarker = val / 10000; 
        if(baseMarker == 0) baseMarker = (animals[i]->typeID == 2 ? -20 : -10); // fallback
        globalMap[animals[i]->loc.x][animals[i]->loc.y] = baseMarker * 10000 - i;
    }

    for (int iter = 0; iter < SIMULATE_ITERATION_COUNT; ++iter) {
        std::vector<Animal*> babies;
        std::vector<int> deadIndices;

        for (size_t i = 0; i < animals.size(); ++i) {
            Animal* anim = animals[i];
            
            // Skip if already dead/eaten this turn
            if (anim->energy <= -10000) continue;

            // 1. Observe
            anim->observe(anim);

            // 2. Move
            int res = anim->move(anim);

            bool starved = false;
            // Decode result
            if (res < 0 && res != -100000) { // Ate and died
                res = -res;
                starved = true;
            } else if (res == -100000) { // Just died
                starved = true;
                res = 100000;
            }

            // Mark eaten animal
            if (res != 100000 && res < (int)animals.size()) {
                 animals[res]->energy = -20000; // Mark eaten
                 deadIndices.push_back(res);
            }

            if (starved) {
                anim->energy = -20000;
                deadIndices.push_back(i);
            } else {
                // 3. Breed
                Animal* baby = anim->giveBirth(anim);
                if (baby) babies.push_back(baby);
            }
        }

        // Cleanup
        std::sort(deadIndices.begin(), deadIndices.end(), std::greater<int>());
        auto last = std::unique(deadIndices.begin(), deadIndices.end());
        deadIndices.erase(last, deadIndices.end());

        for (int idx : deadIndices) {
            if (idx < (int)animals.size()) {
                Animal* dead = animals[idx];
                // Reset map if it points to this dead animal
                // Note: The killer might have overwritten the map spot already.
                // We check if map matches this animal ID.
                int val = globalMap[dead->loc.x][dead->loc.y];
                // ID is encoded in last 4 digits (negative)
                // e.g. -200123. ID is 123. 
                // Decode:
                int storedIdx = -(val % 10000); 
                if (storedIdx == dead->animalID) {
                    globalMap[dead->loc.x][dead->loc.y] = 1; // Grow grass
                }
                
                dead->cleanup();
                delete dead;
                animals.erase(animals.begin() + idx);
            }
        }

        // Add Babies
        for(Animal* b : babies) animals.push_back(b);

        // Re-Index Map
        for(size_t i=0; i<animals.size(); ++i) {
            animals[i]->animalID = i;
            int x = animals[i]->loc.x;
            int y = animals[i]->loc.y;
            int val = globalMap[x][y];
            int marker = val / 10000; 
            if (marker == 0) marker = (animals[i]->typeID == 2 ? -20 : -10); // Safety
            globalMap[x][y] = marker * 10000 - i;
        }

        // Grow Grass
        for(int i=0; i<FIELD_SIZE; ++i) {
            for(int j=0; j<FIELD_SIZE; ++j) {
                if(globalMap[i][j] > 0 && globalMap[i][j] < 5) globalMap[i][j]++;
            }
        }
    }

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
