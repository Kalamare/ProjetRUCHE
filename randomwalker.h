#include <stdio.h>
#include <stdlib.h>
#include "hive.h"

#ifndef RANDOMWALKER_H
#define RANDOMWALKER_H

void determineNextLocations(World *world, Bee *bee);

Location *checkNearbyFlowers(World *world, Bee *bee);

Location *getNearLocation(World *world, Bee *bee, int radius);

int *randomPath(World *world,Bee *bee, Location *locationToGo);

#endif