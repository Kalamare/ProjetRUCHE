#include "location.h"

Location *createLocation(int x, int y) {
    Location *location = malloc(sizeof(Location));
    location->x = x;
    location->y = y;

    /*printf("-- Location created --\n");
    printf("> X: %d\n", location->x);
    printf("> Y: %d\n", location->y);
    printf("-------------------\n");*/

    return location;
}

double distanceSquared(Location *location1, Location *location2) {
    return pow(location1->x - location2->x, 2) + pow(location1->y - location2->y, 2);
}

double distance(Location *location1, Location *location2) {
    return sqrt(distanceSquared(location1, location2));
}