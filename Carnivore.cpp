#include "Carnivore.hpp"
#include "Utils.hpp"

void Carnivore_init(Carnivore* c, int x, int y, int*** map);

Animal* Carnivore_giveBirth_Impl(Animal* self) {
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

    Carnivore* baby = new Carnivore();
    Carnivore_init(baby, birthLoc.x, birthLoc.y, self->worldMap);
    return (Animal*)baby;
}

void Carnivore_observe(Animal* self) {
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
            [cite_start]// [cite: 190-194]
            if (dx == 0 && dy == 0) {
                score = -1000;
            } else if (m == CARNIVORE_MARKER || m == TIGER_MARKER || m == WOLF_MARKER) {
                // Another Carnivore
                score = -10000;
            } else if (m == HERBIVORE_MARKER || m == GIRAFFE_MARKER || m == SLOTH_MARKER) {
                // Herbivore (Prey)
                score = 5; [cite_start]// energyValue [cite: 191]
            } else {
                // Empty/Grass
                score = 0;
            }
            self->viewArray[r][c] = score;
        }
    }
}

void Carnivore_init(Carnivore* c, int x, int y, int*** map) {
    c->parent.loc = {x, y};
    c->parent.energy = 5;
    c->parent.movingCost = 2; [cite_start]// [cite: 189]
    c->parent.birthThreshold = 40; [cite_start]// [cite: 187]
    c->parent.birthCost = 30; [cite_start]// [cite: 188]
    c->parent.energyValue = 5;
    c->parent.viewRange = 1;
    c->parent.worldMap = map;
    c->parent.marker = CARNIVORE_MARKER;

    int width = 2 * c->parent.viewRange + 1;
    c->parent.viewArray = new int*[width];
    for (int i = 0; i < width; ++i) c->parent.viewArray[i] = new int[width];

    c->parent.observe = Carnivore_observe;
    c->parent.move = Animal_move;
    c->parent.giveBirth = Carnivore_giveBirth_Impl;
}