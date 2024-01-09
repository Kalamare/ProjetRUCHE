#include "world.h"

#ifndef HIVE_H
#define HIVE_H

typedef struct Hive Hive;
typedef struct Bee Bee;

enum Role {
    QUEEN,
    WORKER,
    DRONE
};

struct Bee {
    int id;
    int health;
    unsigned int honey;
    Location* location;
    enum Role role;
    bool disease;
    bool dead;
    int lifeTime;
    long idleTime;
    int* nextLocations;
    bool mustGoBack;
    bool goingToFlower;
};

struct Hive {
    int id;
    int height;
    int width;
    int beesCount;
    long time;
    List* bees;
    List* locations;
};

Hive* createHive(int id, int height, int width);

Bee* createBee(int id, int health, unsigned honey, Location* location, enum Role role, bool disease, bool dead, int lifeTime, long idleTime, int* nextLocations, bool mustGoBack, bool goingToFlower);

void addBee(Hive* hive, Bee* bee);

void addLocation(Hive* hive, Location* location);

void tickBees(World* world, Hive* hive, unsigned int frameRate);

void moveBee(World* world, Bee* bee, unsigned int frameRate);

#endif