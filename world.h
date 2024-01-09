#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include<unistd.h>
#include "list.h"
#include "stack.h"

#ifndef WORLD_H
#define WORLD_H

typedef struct World World;
typedef struct Flower Flower;
typedef struct Location Location;

struct World {
    unsigned int id;
    unsigned int size;
    long time;
    List *flowers;
};

struct Location {
    int x;
    int y;
};

struct Flower {
    unsigned int initialCapacity;
    unsigned int capacity;
    unsigned int honeyGiven;
    Location *location;
};

World *createWorld(int id, int size);

Location *createLocation(int x, int y);

Flower *createFlower(Location *location, unsigned int initialCapacity, unsigned int capacity, unsigned int honeyGiven);

void addFlower(World *world, Flower *flower);

void tickWorld();

bool isOutOfWorld(Location *location, World *world);

double distanceSquared(Location *location1, Location *location2);

double distance(Location *location1, Location *location2);

#endif