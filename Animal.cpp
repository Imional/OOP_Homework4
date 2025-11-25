#include "Animal.hpp"
#include "Utils.hpp"
#include <iostream>
#include <cmath>
#include <vector>

// Helper to decode what is at a location
// Returns the marker (e.g. -10 for Herbivore)
int getMarker(int encodedValue) {
    if (encodedValue == 0) return 0;
    // Map values are encoded as marker * 100000 + index
    return encodedValue / 100000;
}

// Helper to get index from map value
int getIndex(int encodedValue) {
    return std::abs(encodedValue % 100000);
}

int Animal_move(Animal* self) {
    [cite_start]// 1. Find best score in viewArray [cite: 150-152]
    int range = self->viewRange;
    int width = 2 * range + 1;
    int maxScore = -2000000000; // Low number
    std::vector<Location> candidates;

    for (int r = 0; r < width; ++r) {
        for (int c = 0; c < width; ++c) {
            int score = self->viewArray[r][c];
            if (score > maxScore) {
                maxScore = score;
                candidates.clear();
                // Convert viewArray relative coords back to world coords
                int dx = c - range;
                int dy = r - range;
                candidates.push_back(wrapLocation(self->loc.x + dx, self->loc.y + dy));
            } else if (score == maxScore) {
                int dx = c - range;
                int dy = r - range;
                candidates.push_back(wrapLocation(self->loc.x + dx, self->loc.y + dy));
            }
        }
    }

    // 2. Random Selection
    Location target = randomSelection(candidates);

    // 3. Handle result
    int eatenIndex = 100000; [cite_start]// Default: No animal eaten [cite: 154]
    int targetContent = (*self->worldMap)[target.x][target.y];

    // Check if we are eating an animal (negative value that isn't us)
    // We shouldn't eat ourselves, but 'move' usually implies moving to a different cell 
    // or staying put. If staying put, we don't eat.
    if ((target.x != self->loc.x || target.y != self->loc.y) && targetContent < 0) {
         eatenIndex = getIndex(targetContent);
    }

    [cite_start]// 4. Update Map [cite: 151]
    // Old location becomes grass (1)
    (*self->worldMap)[self->loc.x][self->loc.y] = 1;
    
    // Update internal location
    self->loc = target;
    
    // 6. Death check
    if (self->energy <= 0) {
        if (eatenIndex == 100000) return -100000; // Died of hunger, ate nothing
        return -eatenIndex; // Died of hunger, but ate something (rare edge case)
    }

    return eatenIndex;
}

Animal* Animal_giveBirth(Animal* self) {
    [cite_start]// [cite: 157] Check energy and neighbors
    if (self->energy <= self->birthThreshold) return nullptr;

    std::vector<Location> freeSpots;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            Location n = wrapLocation(self->loc.x + dx, self->loc.y + dy);
            // Check if empty (positive implies grass/empty)
            if ((*self->worldMap)[n.x][n.y] > 0) {
                freeSpots.push_back(n);
            }
        }
    }

    if (freeSpots.empty()) return nullptr;

    Location birthLoc = randomSelection(freeSpots);

    // Pay cost
    self->energy -= self->birthCost;

    // The actual instantiation happens in the specific classes (Herbivore, etc.)
    // Specific classes will call a helper or implement this logic themselves.
    return nullptr; 
}

void Animal_delete(Animal* self) {
    if (self->viewArray) {
        int width = 2 * self->viewRange + 1;
        for (int i = 0; i < width; ++i) {
            delete[] self->viewArray[i];
        }
        delete[] self->viewArray;
    }
}