#include <SDL2/SDL.h>
#include <stdbool.h>
#include "list.h"
#include "location.h"

#ifndef HIVE_H
#define HIVE_H

#define EGG_LIFE_TIME 3
#define LARVA_LIFE_TIME 9
#define NYMPH_LIFE_TIME 21
#define QUEEN_MAX_LIFE_TIME 2190

#define VIEW_DISTANCE 30

typedef struct Hive Hive;
typedef struct Bee Bee;


enum Role {
    QUEEN,
    WORKER,
    DRONE,
    EGG,
    LARVA,
    NYMPH
};

struct Bee {
    int id;
    int health;
    int lifeTime;
    int honey;
    int *nextLocations;
    int idleTime;
    bool disease;
    bool dead;
    bool mustGoToHive;
    bool goingToFlower;
    bool inHive;
    Location *location;
    enum Role role;
};

struct Hive {
    int id;
    int height;
    int width;
    int beesCount;
    long time;
    List *bees;
    List *locations;
};

Hive *createHive(int id, int x, int y, int height, int width);

Bee *createBee(int id, Location *location, enum Role role, int health, int lifeTime, int idleTime,int *nextLocations);

Location *getNextLocation(Bee *bee);

Location *getLocationFromList(List *locations, int position);

void addBee(Hive *hive, Bee *bee);

void addLocation(Hive *hive, Location *location);

void tryEvolve(Bee *bee);

void triggerDisease(Bee *bee);

bool isIdling(Bee *bee);

#endif