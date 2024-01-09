#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef LOCATION_H
#define LOCATION_H

typedef struct Location Location;

struct Location {
    int x;
    int y;
};

Location *createLocation(int x, int y);

double distanceSquared(Location *location1, Location *location2);

double distance(Location *location1, Location *location2);

#endif