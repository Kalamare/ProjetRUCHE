#include "randomwalker.h"

Location *checkNearbyFlowers(World *world, Bee *bee) {
    ListElement *actualElement = world->flowers->firstElement;
    Flower *flower;
    Location *beeLocation = bee->location;

    while (actualElement != NULL) {
        flower = actualElement->value;
        Location *flowerLocation = flower->location;

        if (distanceSquared(beeLocation, flowerLocation) <= pow(VIEW_DISTANCE, 2)) {
            return flowerLocation;
        }
        actualElement = actualElement->nextElement;
    }
    return NULL;
}

Location *getNearLocation(World *world, Bee *bee, int radius) {
    Location *beeLocation = bee->location;
    bool xPositive = rand() % 2 == 0;
    bool yPositive = rand() % 2 == 0;

    int newRandomXRadius = rand() % radius;
    int newRandomYRadius = rand() % radius;

    int newX = (xPositive ? beeLocation->x + newRandomXRadius : beeLocation->x - newRandomXRadius);
    int newY = (yPositive ? beeLocation->y + newRandomYRadius : beeLocation->y - newRandomYRadius);

    Location *nearLocation = createLocation(newX, newY);

    if (nearLocation->x < 0 || nearLocation->x >= world->size || nearLocation->y < 0 ||
        nearLocation->y >= world->size) {
        free(nearLocation);
        return getNearLocation(world, bee, radius);
    }
    return nearLocation;
}

void determineNextLocations(World *world, Bee *bee) {
    //printf("Determining next locations for bee %d\n", bee->id);
    Location *nearLocation = getNearLocation(world, bee, 50);
    randomPath(world, bee, nearLocation);
    //  printf("Added %d next locations to bee %d\n", newPath[0] + newPath[1], bee->id);
}

void randomPath(World *world, Bee *bee, Location *locationToGo) {
    int differenceX = locationToGo->x - bee->location->x;
    int differenceY = locationToGo->y - bee->location->y;
    /* printf("Location to go: %d, %d\n", locationToGo->x, locationToGo->y);
     printf("Bee location: %d, %d\n", bee->location->x, bee->location->y);
     printf("Difference: %d, %d\n", differenceX, differenceY);*/

    bee->nextLocations[0] = differenceX;
    bee->nextLocations[1] = differenceY;
}