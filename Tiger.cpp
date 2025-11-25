#include "Tiger.hpp"
#include "Utils.hpp"

Animal* Tiger_giveBirth(Animal* self) {
    if (self->energy <= self->birthThreshold) return nullptr;
    // ... neighbor check ...
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
    Tiger* baby = new Tiger();
    Tiger_init(baby, birthLoc.x, birthLoc.y, self->worldMap);
    return (Animal*)baby;
}

void Tiger_init(Tiger* t, int x, int y, int*** map) {
    Carnivore_init(&t->parentStruct, x, y, map);
    [cite_start]// [cite: 244-245]
    t->parentStruct.parent.viewRange = 2;
    t->parentStruct.parent.movingCost = 4;
    t->parentStruct.parent.marker = TIGER_MARKER;
    t->parentStruct.parent.giveBirth = Tiger_giveBirth;

    // Fix viewArray size (Carnivore init made it 3x3)
    for(int i=0; i<3; ++i) delete[] t->parentStruct.parent.viewArray[i];
    delete[] t->parentStruct.parent.viewArray;
    int size = 5;
    t->parentStruct.parent.viewArray = new int*[size];
    for(int i=0; i<size; i++) t->parentStruct.parent.viewArray[i] = new int[size];
}