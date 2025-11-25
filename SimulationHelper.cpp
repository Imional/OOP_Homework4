#include "SimulationHelper.hpp"
#include "Utils.hpp"

// Include all animal headers
#include "Animal.hpp"
#include "Herbivore.hpp"
#include "Carnivore.hpp"
#include "Giraffe.hpp"
#include "Sloth.hpp"
#include "Tiger.hpp"
#include "Wolf.hpp"
#include "Omnivore.hpp"

#include <iostream>
#include <vector>

constexpr int kIndexDigitBase = 100000; // Updated to match 100,000 limit
constexpr int kMarkerEncodingMultiplier = kIndexDigitBase;

// Helper to encode/decode
int encodeMarkerWithIndex(int marker, int index) {
    int clampedIndex = index % kIndexDigitBase;
    int baseValue = marker * kMarkerEncodingMultiplier;
    return baseValue + (marker < 0 ? -clampedIndex : clampedIndex);
}

int decodeMarkerFromEncoded(int encodedValue) {
    if (encodedValue == 0) return 0;
    return encodedValue / kMarkerEncodingMultiplier;
}

void refreshEncodedIndices(FieldGrid globalMap, const std::vector<Animal*>& animals) {
    for (size_t i = 0; i < animals.size(); ++i) {
        Animal* animal = animals[i];
        Location loc = animal->loc;
        // Re-stamp the map with the new index
        globalMap[loc.x][loc.y] = encodeMarkerWithIndex(animal->marker, static_cast<int>(i));
    }
}

void shuffleAnimals(std::vector<Animal*>& animals) {
    for (int i = static_cast<int>(animals.size()) - 1; i > 0; --i) {
        int j = randomRange(i + 1);
        if (i == j) continue;
        Animal* temp = animals[i];
        animals[i] = animals[j];
        animals[j] = temp;
    }
}

void prepareFreeCells(std::vector<Location>& freeCells) {
    freeCells.clear();
    freeCells.reserve(FIELD_SIZE * FIELD_SIZE);
    for (int y = 0; y < FIELD_SIZE; ++y) {
        for (int x = 0; x < FIELD_SIZE; ++x) {
            freeCells.push_back({x, y});
        }
    }
    shuffleLocations(freeCells);
}

// We need a pointer to the global map for Animal initialization
// We can use a lambda capture in the spawnSpecies calls below.

template <typename CreateAnimalFn>
void spawnSpecies(int count, int marker, FieldGrid globalMap,
                  std::vector<Location>& freeCells,
                  std::vector<Animal*>& animals,
                  CreateAnimalFn&& createAnimal) {
    for (int i = 0; i < count; ++i) {
        if (freeCells.empty()) break;
        Location loc = freeCells.back();
        freeCells.pop_back();
        
        Animal* animal = createAnimal();
        if (animal == nullptr) continue;
        
        animal->loc = loc;
        animals.push_back(animal);
        
        int index = static_cast<int>(animals.size()) - 1;
        globalMap[loc.x][loc.y] = encodeMarkerWithIndex(marker, index);
    }
}

void Simulation_clearContext(FieldGrid globalMap) {
    for (int x = 0; x < FIELD_SIZE; ++x) {
        for (int y = 0; y < FIELD_SIZE; ++y) {
            globalMap[x][y] = 0;
        }
    }
}

void Simulation_initializeGrass(FieldGrid globalMap) {
    for (int x = 0; x < FIELD_SIZE; ++x) {
        for (int y = 0; y < FIELD_SIZE; ++y) {
            int value = randomRange(5) + 1;
            globalMap[x][y] = value;
        }
    }
}

void Simulation_prepare(const std::string& studentID, FieldGrid globalMap) {
    Simulation_clearContext(globalMap);
    initRandom(studentID);
    Simulation_initializeGrass(globalMap);
}

// THIS IS THE CRITICAL FUNCTION TO SPAWN ALL TYPES
void Simulation_spawnInitialAnimals(FieldGrid globalMap, std::vector<Animal*>& animals) {
    animals.clear();
    animals.reserve(1000); // Reserve enough space

    std::vector<Location> freeCells;
    prepareFreeCells(freeCells);

    // We need a helper to pass int*** to init functions
    // We construct a row-pointer array
    static int* mapRows[FIELD_SIZE];
    for(int i=0; i<FIELD_SIZE; ++i) mapRows[i] = globalMap[i];
    static int** mapPtr = mapRows;

    // --- 1.1 Animals ---
    spawnSpecies(HERBIVORE_COUNT, HERBIVORE_MARKER, globalMap, freeCells, animals, []() {
        Herbivore* h = new Herbivore();
        Herbivore_init(h, 0, 0, &mapPtr);
        return (Animal*)h;
    });

    spawnSpecies(CARNIVORE_COUNT, CARNIVORE_MARKER, globalMap, freeCells, animals, []() {
        Carnivore* c = new Carnivore();
        Carnivore_init(c, 0, 0, &mapPtr);
        return (Animal*)c;
    });

    // --- 1.2 Animals ---
    spawnSpecies(GIRAFFE_COUNT, GIRAFFE_MARKER, globalMap, freeCells, animals, []() {
        Giraffe* g = new Giraffe();
        Giraffe_init(g, 0, 0, &mapPtr);
        return (Animal*)g;
    });

    spawnSpecies(SLOTH_COUNT, SLOTH_MARKER, globalMap, freeCells, animals, []() {
        Sloth* s = new Sloth();
        Sloth_init(s, 0, 0, &mapPtr);
        return (Animal*)s;
    });

    spawnSpecies(TIGER_COUNT, TIGER_MARKER, globalMap, freeCells, animals, []() {
        Tiger* t = new Tiger();
        Tiger_init(t, 0, 0, &mapPtr);
        return (Animal*)t;
    });

    spawnSpecies(WOLF_COUNT, WOLF_MARKER, globalMap, freeCells, animals, []() {
        Wolf* w = new Wolf();
        Wolf_init(w, 0, 0, &mapPtr);
        return (Animal*)w;
    });

    // --- 1.3 Extra Credit: Omnivores ---
    // Spawning 20 Omnivores (Arbitrary count > 2 for extra credit balance)
    spawnSpecies(20, -30, globalMap, freeCells, animals, []() {
        Omnivore* o = new Omnivore();
        Omnivore_init(o, 0, 0, &mapPtr);
        return (Animal*)o;
    });

    shuffleAnimals(animals);
    refreshEncodedIndices(globalMap, animals);
}