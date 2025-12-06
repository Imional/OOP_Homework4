#include "Herbivore.hpp"
#include "Utils.hpp"
#include <vector>

void Herbivore_observe(Animal* self) {
    int range = self->viewRange;
    int size = 2 * range + 1;
    if (!self->viewArray) {
        self->viewArray = new int*[size];
        for(int i=0; i<size; ++i) self->viewArray[i] = new int[size];
    }
    int** map = *(self->map);
    for(int i=0; i<size; ++i) {
        for(int j=0; j<size; ++j) {
            int gx = (self->loc.x + (i - range) + FIELD_SIZE) % FIELD_SIZE;
            int gy = (self->loc.y + (j - range) + FIELD_SIZE) % FIELD_SIZE;
            int val = map[gx][gy];
            int score = 0;
            if (i==range && j==range) score = -1000;
            else if (val < 0) score = -10000; 
            else {
                score = val; // Grass amount
                bool danger = false;
                for(int dx=-1; dx<=1; ++dx){
                    for(int dy=-1; dy<=1; ++dy){
                        if(dx==0 && dy==0) continue;
                        int nx = (gx + dx + FIELD_SIZE) % FIELD_SIZE;
                        int ny = (gy + dy + FIELD_SIZE) % FIELD_SIZE;
                        // Check for predators (Carnivore=-20..., Tiger=-21..., Wolf=-22...)
                        // Note: Omnivore is -30... (if implemented)
                        if (map[nx][ny] <= -2000000) danger = true;
                    }
                }
                if (danger) score -= 100;
            }
            self->viewArray[i][j] = score;
        }
    }
}

int Herbivore_move(Animal* self) {
    int range = self->viewRange;
    int size = 2 * range + 1;
    std::vector<Location> candidates;
    int maxVal = -2000000000;

    for(int i=0; i<size; ++i) {
        for(int j=0; j<size; ++j) {
            if (self->viewArray[i][j] > maxVal) {
                maxVal = self->viewArray[i][j];
                candidates.clear();
                candidates.push_back({i, j});
            } else if (self->viewArray[i][j] == maxVal) {
                candidates.push_back({i, j});
            }
        }
    }

    Location targetRel = randomSelection(candidates);
    // Relative to center (range, range)
    if (targetRel.x == range && targetRel.y == range) {
        // Staying still: "Grass energy on the tile... is not consumed."
        // Do not update map value.
        // Do not consume energy here (handled in main).
        return 100000;
    }

    int newX = (self->loc.x + (targetRel.x - range) + FIELD_SIZE) % FIELD_SIZE;
    int newY = (self->loc.y + (targetRel.y - range) + FIELD_SIZE) % FIELD_SIZE;

    // "The spot that the animal was standing begins growing grass again (grass amount = 1)"
    (*(self->map))[self->loc.x][self->loc.y] = 1;

    // Eat grass at new location
    if ((*(self->map))[newX][newY] > 0) {
        self->energy += (*(self->map))[newX][newY];
    }

    self->loc.x = newX;
    self->loc.y = newY;
    
    // Mark as occupied (using temp ID, re-indexed at end of turn)
    // We use a generic placeholder or keep the old ID logic, it will be overwritten in cleanup.
    // To allow others to see it this turn, we must set it to negative.
    // We use -1000000 - self->animalID (even if ID is old, it signifies occupancy).
    (*(self->map))[newX][newY] = -1000000 - self->animalID;

    return 100000;
}

Animal* Herbivore_giveBirth(Animal* self) {
    if (self->energy <= self->birthThreshold) return nullptr;

    std::vector<Location> freeSpots;
    for(int dx=-1; dx<=1; ++dx){
        for(int dy=-1; dy<=1; ++dy){
            if(dx==0 && dy==0) continue;
            int nx = (self->loc.x + dx + FIELD_SIZE) % FIELD_SIZE;
            int ny = (self->loc.y + dy + FIELD_SIZE) % FIELD_SIZE;
            if ((*(self->map))[nx][ny] >= 0) freeSpots.push_back({nx, ny});
        }
    }

    if (freeSpots.empty()) return nullptr;

    Location birthLoc = randomSelection(freeSpots); // Logic per prompt Figure 1 correction
    
    // "pays the corresponding energy cost"
    self->energy -= self->birthCost;

    Herbivore* baby = new Herbivore();
    baby->parent.loc = birthLoc;
    baby->parent.energy = 5; // "Newly born Animal always starts with energy = 5"
    baby->parent.movingCost = 1;
    baby->parent.birthThreshold = 15;
    baby->parent.birthCost = 5;
    baby->parent.energyValue = 5;
    baby->parent.viewRange = 1;
    baby->parent.map = self->map;
    baby->parent.typeID = 1;
    baby->parent.viewArray = nullptr;
    baby->parent.observe = Herbivore_observe;
    baby->parent.move = Herbivore_move;
    baby->parent.giveBirth = Herbivore_giveBirth;

    // Mark map for baby immediately so others don't birth there this turn
    (*(self->map))[birthLoc.x][birthLoc.y] = -1000000; 

    return (Animal*)baby;
}
