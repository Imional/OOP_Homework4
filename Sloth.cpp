#include "Sloth.hpp"
#include "Utils.hpp"
#include <vector>

void Sloth_observe(Animal* self) {
    Sloth* s = (Sloth*)self;
    if (s->isSkipping) return;
    Herbivore_observe(self);
}

int Sloth_move(Animal* self) {
    Sloth* s = (Sloth*)self;
    if (s->isSkipping) return 100000; // Skip
    return Herbivore_move(self);
}

Animal* Sloth_giveBirth(Animal* self) {
    Sloth* s = (Sloth*)self;
    
    // Toggle state: if we just moved (not skipping), next turn we skip.
    // If we were skipping, we wake up (set false) and return nullptr (no action this turn).
    if (s->isSkipping) {
        s->isSkipping = false;
        return nullptr;
    }
    
    // We acted this turn, so sleep next turn
    s->isSkipping = true;

    if (self->energy <= self->birthThreshold) return nullptr;
    // Standard birth logic
    std::vector<Location> freeSpots;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = (self->loc.x + dx + FIELD_SIZE) % FIELD_SIZE;
            int ny = (self->loc.y + dy + FIELD_SIZE) % FIELD_SIZE;
            if ((*(self->map))[nx][ny] >= 0) freeSpots.push_back({nx, ny});
        }
    }
    if (freeSpots.empty()) return nullptr;

    Location birthLoc = randomSelection(freeSpots);
    self->energy -= self->birthCost;

    Sloth* baby = new Sloth();
    baby->parentStruct.parent.loc = birthLoc;
    baby->parentStruct.parent.energy = 5;
    baby->parentStruct.parent.movingCost = 1;
    baby->parentStruct.parent.birthThreshold = 15;
    baby->parentStruct.parent.birthCost = 5;
    baby->parentStruct.parent.energyValue = 5;
    baby->parentStruct.parent.viewRange = 1;
    baby->parentStruct.parent.map = self->map;
    baby->parentStruct.parent.typeID = 1;
    baby->parentStruct.parent.viewArray = nullptr;
    baby->isSkipping = false; // Newborns act on first turn

    baby->parentStruct.parent.observe = Sloth_observe;
    baby->parentStruct.parent.move = Sloth_move;
    baby->parentStruct.parent.giveBirth = Sloth_giveBirth;

    (*(self->map))[birthLoc.x][birthLoc.y] = -1200000;
    return (Animal*)baby;
}
