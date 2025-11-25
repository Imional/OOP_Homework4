#include "Wolf.hpp"
#include "Utils.hpp"

Animal* Wolf_giveBirth(Animal* self) {
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
    Wolf* baby = new Wolf();
    Wolf_init(baby, birthLoc.x, birthLoc.y, self->worldMap);
    return (Animal*)baby;
}

void Wolf_init(Wolf* w, int x, int y, int*** map) {
    Carnivore_init(&w->parentStruct, x, y, map);
    w->parentStruct.parent.marker = WOLF_MARKER;
    w->parentStruct.parent.giveBirth = Wolf_giveBirth;
    [cite_start]// Same stats as Carnivore [cite: 246]
}