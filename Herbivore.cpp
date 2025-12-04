#include "Herbivore.hpp"
#include "Utils.hpp"
#include <vector>

void Herbivore_observe(Animal* self) {
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
            else if (val < 0) score = -10000; // Occupied
            else {
                score = val; // Grass
                // Check neighbors for predators (-20 to -29 and -30)
                bool danger = false;
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = (gx + dx + FIELD_SIZE) % FIELD_SIZE;
                        int ny = (gy + dy + FIELD_SIZE) % FIELD_SIZE;
                        // Check for Carnivores (-20...), Tigers (-21...), Wolves (-22...), Omnivores (-30...)
                        if (map[nx][ny] <= -2000000) danger = true;
                    }
                }
                if (danger) score -= 100;
            }
            self->viewArray[i][j] = score;
        }
    }
}

int Herbivore_move(Animal* self) {
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

    (*(self->map))[self->loc.x][self->loc.y] = 1; // Old spot grows grass
    self->energy -= self->movingCost;

    if ((*(self->map))[nx][ny] > 0) self->energy += (*(self->map))[nx][ny];

    self->loc = {nx, ny};
    (*(self->map))[nx][ny] = -1000000 - self->animalID; // Temp ID

    if (self->energy <= 0) return -100000;
    return 100000;
}

Animal* Herbivore_giveBirth(Animal* self) {
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

    Herbivore* baby = new Herbivore();
    baby->parent.loc = birthLoc;
    baby->parent.energy = 5;
    baby->parent.movingCost = 1;
    baby->parent.birthThreshold = 15;
    baby->parent.birthCost = 5;
    baby->parent.energyValue = 5;
    baby->parent.viewRange = 1;
    baby->parent.map = self->map;
    baby->parent.typeID = 1;
    baby->parent.viewArray = nullptr;
    baby->parent.observe = Herbivore_observe;
    baby->parent.move = Herbivore_move;
    baby->parent.giveBirth = Herbivore_giveBirth;

    (*(self->map))[birthLoc.x][birthLoc.y] = -1000000;
    return (Animal*)baby;
}
