#include "Carnivore.hpp"
#include "Utils.hpp"
#include <vector>

void Carnivore_observe(Animal* self) {
    int range = self->viewRange;
    int size = 2 * range + 1;
    if (!self->viewArray) {
        self->viewArray = new int*[size];
        for (int i = 0; i < size; ++i) self->viewArray[i] = new int[size];
    }
    int** map = *(self->map);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int gx = (self->loc.x + (i - range) + FIELD_SIZE) % FIELD_SIZE;
            int gy = (self->loc.y + (j - range) + FIELD_SIZE) % FIELD_SIZE;
            int val = map[gx][gy];

            int score = 0;
            if (i == range && j == range) score = -1000;
            else if (val <= -2000000) score = -10000; // Avoid other predators
            else if (val <= -1000000) score = 5; // Eat Herbivores (Energy Value)
            else score = 0;

            self->viewArray[i][j] = score;
        }
    }
}

int Carnivore_move(Animal* self) {
    int range = self->viewRange;
    int size = 2 * range + 1;
    std::vector<Location> candidates;
    int maxScore = -2000000000;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (self->viewArray[i][j] > maxScore) {
                maxScore = self->viewArray[i][j];
                candidates.clear();
                candidates.push_back({i, j});
            } else if (self->viewArray[i][j] == maxScore) {
                candidates.push_back({i, j});
            }
        }
    }

    Location targetRel = randomSelection(candidates);
    int nx = (self->loc.x + (targetRel.x - range) + FIELD_SIZE) % FIELD_SIZE;
    int ny = (self->loc.y + (targetRel.y - range) + FIELD_SIZE) % FIELD_SIZE;

    int eatenIdx = 100000;
    int targetVal = (*(self->map))[nx][ny];
    if (targetVal <= -1000000 && targetVal > -2000000) {
        eatenIdx = -(targetVal + 1000000); // Decode ID
        self->energy += 5; // Gain Energy
    }

    (*(self->map))[self->loc.x][self->loc.y] = 1;
    self->energy -= self->movingCost;
    self->loc = {nx, ny};
    (*(self->map))[nx][ny] = -2000000 - self->animalID;

    if (self->energy <= 0) return -eatenIdx;
    return eatenIdx;
}

Animal* Carnivore_giveBirth(Animal* self) {
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

    Carnivore* baby = new Carnivore();
    baby->parent.loc = birthLoc;
    baby->parent.energy = 5;
    baby->parent.movingCost = 2;
    baby->parent.birthThreshold = 40;
    baby->parent.birthCost = 30;
    baby->parent.energyValue = 5;
    baby->parent.viewRange = 1;
    baby->parent.map = self->map;
    baby->parent.typeID = 2;
    baby->parent.viewArray = nullptr;
    baby->parent.observe = Carnivore_observe;
    baby->parent.move = Carnivore_move;
    baby->parent.giveBirth = Carnivore_giveBirth;

    (*(self->map))[birthLoc.x][birthLoc.y] = -2000000;
    return (Animal*)baby;
}
