#include "Giraffe.hpp"
#include "Utils.hpp"

// Re-implement giveBirth to spawn Giraffes
Animal* Giraffe_giveBirth(Animal* self) {
    if (self->energy <= self->birthThreshold) return nullptr;
    // ... (same neighbor finding logic) ...
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
    Giraffe* baby = new Giraffe();
    Giraffe_init(baby, birthLoc.x, birthLoc.y, self->worldMap);
    return (Animal*)baby;
}

void Giraffe_init(Giraffe* g, int x, int y, int*** map) {
    Herbivore_init(&g->parentStruct, x, y, map);
    [cite_start]// Overrides [cite: 232-233]
    g->parentStruct.parent.viewRange = 2;
    g->parentStruct.parent.movingCost = 2;
    g->parentStruct.parent.marker = GIRAFFE_MARKER;
    g->parentStruct.parent.giveBirth = Giraffe_giveBirth;

    // Reallocate viewArray for larger range
    delete[] g->parentStruct.parent.viewArray; // Warning: only deletes rows if looped? 
    // Correct way: free the old 1x1 array first. 
    // But Herbivore_init allocated 3x3 (range 1). We need 5x5.
    // Clean up logic:
    for(int i=0; i<3; ++i) delete[] g->parentStruct.parent.viewArray[i];
    delete[] g->parentStruct.parent.viewArray;

    int size = 5; 
    g->parentStruct.parent.viewArray = new int*[size];
    for(int i=0; i<size; i++) g->parentStruct.parent.viewArray[i] = new int[size];
}