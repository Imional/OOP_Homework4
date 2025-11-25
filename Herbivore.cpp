#include "Herbivore.hpp"
#include "Utils.hpp"

// Forward declaration needed for the birth function
void Herbivore_init(Herbivore* h, int x, int y, int*** map);

// Specific birth implementation
Animal* Herbivore_giveBirth_Impl(Animal* self) {
    // Logic copies Animal_giveBirth but creates a Herbivore
    if (self->energy <= self->birthThreshold) return nullptr;

    std::vector<Location> freeSpots;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            Location n = wrapLocation(self->loc.x + dx, self->loc.y + dy);
            if ((*self->worldMap)[n.x][n.y] > 0) freeSpots.push_back(n);
        }
    }

    if (freeSpots.empty()) return nullptr;

    Location birthLoc = randomSelection(freeSpots);
    self->energy -= self->birthCost;

    Herbivore* baby = new Herbivore();
    Herbivore_init(baby, birthLoc.x, birthLoc.y, self->worldMap);
    return (Animal*)baby;
}

void Herbivore_observe(Animal* self) {
    int range = self->viewRange;
    int width = 2 * range + 1;

    for (int r = 0; r < width; ++r) {
        for (int c = 0; c < width; ++c) {
            int dx = c - range;
            int dy = r - range;
            Location target = wrapLocation(self->loc.x + dx, self->loc.y + dy);
            int content = (*self->worldMap)[target.x][target.y];

            int score = 0;
            [cite_start]// [cite: 178-185]
            if (dx == 0 && dy == 0) {
                score = -1000; // Standing
            } else if (content < 0) {
                score = -10000; // Occupied by animal
            } else {
                score = content; // Grass amount
                
                // Check neighbors for predators
                bool predatorNearby = false;
                for (int nx = -1; nx <= 1; ++nx) {
                    for (int ny = -1; ny <= 1; ++ny) {
                        if (nx == 0 && ny == 0) continue;
                        Location neighbor = wrapLocation(target.x + nx, target.y + ny);
                        int nContent = (*self->worldMap)[neighbor.x][neighbor.y];
                        // Check for any Carnivore marker (-20, -21, -22)
                        int m = nContent / 100000; 
                        if (m == CARNIVORE_MARKER || m == TIGER_MARKER || m == WOLF_MARKER) {
                            predatorNearby = true;
                        }
                    }
                }
                if (predatorNearby) score -= 100; [cite_start]// Adjusted -100 if predator nearby [cite: 180]
                // Note: PDF example says -95 (5 grass - 100).
            }
            self->viewArray[r][c] = score;
        }
    }
}

void Herbivore_init(Herbivore* h, int x, int y, int*** map) {
    h->parent.loc = {x, y};
    h->parent.energy = 5;
    h->parent.movingCost = 1; [cite_start]// [cite: 177]
    h->parent.birthThreshold = 15; [cite_start]// [cite: 176]
    h->parent.birthCost = 5;
    h->parent.energyValue = 5;
    h->parent.viewRange = 1;
    h->parent.worldMap = map;
    h->parent.marker = HERBIVORE_MARKER;

    int width = 2 * h->parent.viewRange + 1;
    h->parent.viewArray = new int*[width];
    for (int i = 0; i < width; ++i) h->parent.viewArray[i] = new int[width];

    h->parent.observe = Herbivore_observe;
    h->parent.move = Animal_move;
    h->parent.giveBirth = Herbivore_giveBirth_Impl;
}