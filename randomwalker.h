#include <stdio.h>
#include <stdlib.h>
#include "world.h"

#ifndef RANDOMWALKER_H
#define RANDOMWALKER_H

void determineNextLocations(World* world, Hive* hive, Bee *bee);

Flower *checkNearbyFlowers(List* flowers, Bee *bee);

Bee* checkNearbyBees(List* bees, Bee *bee, enum Role role);

Location *getNearLocation(World* world,Hive* hive, Bee *bee, int radius, int attempts);

void randomPath(Bee *bee, Location *locationToGo);

#endif