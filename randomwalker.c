#include "randomwalker.h"

Location *checkNearbyFlowers(World *world, Bee *bee) {
    if (bee->mustGoBack || bee->goingToFlower) {
        return NULL;
    }
    ListElement *actualElement = world->flowers->firstElement;
    Flower *flower;
    Location *beeLocation = bee->location;

    while (actualElement != NULL) {
        flower = actualElement->value;
        Location *flowerLocation = flower->location;

        if (distanceSquared(beeLocation, flowerLocation) <= pow(15, 2)) {
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

    Location *nearLocation = createLocation(
            (xPositive ? beeLocation->x + rand() % radius : beeLocation->x - rand() % radius),
            (yPositive ? beeLocation->y + rand() % radius : beeLocation->y - rand() % radius));

    if (nearLocation->x < 0 || nearLocation->x >= world->size || nearLocation->y < 0 ||
        nearLocation->y >= world->size) {
        free(nearLocation);
        return getNearLocation(world, bee, radius);
    }
    return nearLocation;
}

void determineNextLocations(World *world, Bee *bee) {
    if (bee->mustGoBack || bee->goingToFlower) {
        return;
    }
    //printf("Determining next locations for bee %d\n", bee->id);
    Location *nearLocation = getNearLocation(world, bee, 30);
    randomPath(world, bee, nearLocation);
    //  printf("Added %d next locations to bee %d\n", newPath[0] + newPath[1], bee->id);
}

int *randomPath(World *world, Bee *bee, Location *locationToGo) {
    int differenceX = locationToGo->x - bee->location->x;
    int differenceY = locationToGo->y - bee->location->y;
    /* printf("Location to go: %d, %d\n", locationToGo->x, locationToGo->y);
     printf("Bee location: %d, %d\n", bee->location->x, bee->location->y);
     printf("Difference: %d, %d\n", differenceX, differenceY);*/

    bee->nextLocations[0] = differenceX;
    bee->nextLocations[1] = differenceY;

    return bee->nextLocations;
}