#include "Sloth.hpp"
#include "Utils.hpp"

int Sloth_move(Animal* self) {
    Sloth* s = (Sloth*)self;
    [cite_start]// [cite: 240-241] Move one turn, skip next.
    if (s->canMove) {
        s->canMove = false;
        return Animal_move(self);
    } else {
        s->canMove = true;
        return 100000; // Skip turn, no move cost, no eat
    }
}

Animal* Sloth_giveBirth(Animal* self) {
    // Standard logic spawning Sloth
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

    Sloth* baby = new Sloth();
    Sloth_init(baby, birthLoc.x, birthLoc.y, self->worldMap);
    // Baby Sloth waits for next turn? Prompt says "first turn after birth". 
    // Init sets canMove = true.
    return (Animal*)baby;
}

void Sloth_init(Sloth* s, int x, int y, int*** map) {
    Herbivore_init(&s->parentStruct, x, y, map);
    s->canMove = true; [cite_start]// [cite: 241]
    s->parentStruct.parent.move = Sloth_move;
    s->parentStruct.parent.giveBirth = Sloth_giveBirth;
    s->parentStruct.parent.marker = SLOTH_MARKER;
}