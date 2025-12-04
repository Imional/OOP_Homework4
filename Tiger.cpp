#include "Tiger.hpp"
#include "Utils.hpp"
#include <vector>

Animal* Tiger_giveBirth(Animal* self) {
    if (self->energy <= self->birthThreshold) return nullptr;
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

    Tiger* baby = new Tiger();
    baby->parentStruct.parent.loc = birthLoc;
    baby->parentStruct.parent.energy = 5;
    baby->parentStruct.parent.movingCost = 4; // Tiger Stat
    baby->parentStruct.parent.birthThreshold = 40;
    baby->parentStruct.parent.birthCost = 30;
    baby->parentStruct.parent.energyValue = 5;
    baby->parentStruct.parent.viewRange = 2; // Tiger Stat
    baby->parentStruct.parent.map = self->map;
    baby->parentStruct.parent.typeID = 2;
    baby->parentStruct.parent.viewArray = nullptr;

    baby->parentStruct.parent.observe = Carnivore_observe;
    baby->parentStruct.parent.move = Carnivore_move;
    baby->parentStruct.parent.giveBirth = Tiger_giveBirth;

    (*(self->map))[birthLoc.x][birthLoc.y] = -2100000;
    return (Animal*)baby;
}
