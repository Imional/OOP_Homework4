#include "Omnivore.hpp"
#include "Utils.hpp"

// We need a marker for Omnivore. Since Utils.hpp is fixed, 
// we will define it here or assume the student adds it.
// I will define it locally to be safe.
const int OMNIVORE_MARKER = -30; 

Animal* Omnivore_giveBirth(Animal* self) {
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
    Omnivore* baby = new Omnivore();
    Omnivore_init(baby, birthLoc.x, birthLoc.y, self->worldMap);
    return (Animal*)baby;
}

void Omnivore_observe(Animal* self) {
    int range = self->viewRange;
    int width = 2 * range + 1;

    for (int r = 0; r < width; ++r) {
        for (int c = 0; c < width; ++c) {
            int dx = c - range;
            int dy = r - range;
            Location target = wrapLocation(self->loc.x + dx, self->loc.y + dy);
            int content = (*self->worldMap)[target.x][target.y];
            int m = content / 100000;

            int score = 0;
            if (dx == 0 && dy == 0) {
                score = -1000;
            } else if (m <= -20) {
                // Avoid Carnivores (-20, -21, -22)
                score = -10000;
            } else if (m <= -10) {
                // Herbivores (-10 to -12). Eat them.
                score = 50; 
            } else {
                // Grass. Eat it.
                score = content * 2; 
            }
            self->viewArray[r][c] = score;
        }
    }
}

void Omnivore_init(Omnivore* o, int x, int y, int*** map) {
    o->parent.loc = {x, y};
    o->parent.energy = 5;
    o->parent.movingCost = 2; 
    o->parent.birthThreshold = 25; 
    o->parent.birthCost = 10;
    o->parent.energyValue = 5;
    o->parent.viewRange = 1;
    o->parent.worldMap = map;
    o->parent.marker = OMNIVORE_MARKER; 

    int size = 3;
    o->parent.viewArray = new int*[size];
    for(int i=0; i<size; i++) o->parent.viewArray[i] = new int[size];

    o->parent.observe = Omnivore_observe;
    o->parent.move = Animal_move;
    o->parent.giveBirth = Omnivore_giveBirth;
}