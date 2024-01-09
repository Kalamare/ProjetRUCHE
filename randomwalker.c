#include "randomwalker.h"

Flower *checkNearbyFlowers(List* flowers, Bee *bee) {
    ListElement *actualElement = flowers->firstElement;
    Flower *flower;
    Location *beeLocation = bee->location;

    while (actualElement != NULL) {
        flower = actualElement->value;
        Location *flowerLocation = flower->location;

        if (distanceSquared(beeLocation, flowerLocation) <= pow(FLOWERS_VIEW_DISTANCE, 2)) {
            return flower;
        }
        actualElement = actualElement->nextElement;
    }
    return NULL;
}

Bee *checkNearbyBees(List* bees, Bee *bee, enum Role role) {
    ListElement *actualElement = bees->firstElement;
    Bee *nearBee;
    Location *beeLocation = bee->location;
    bool isQueen = bee->role == QUEEN;

    while (actualElement != NULL) {
        nearBee = actualElement->value;
        Location *nearBeeLocation = nearBee->location;

        if (bee->id != nearBee->id && nearBee->role == role && distanceSquared(beeLocation, nearBeeLocation) <= pow(isQueen ? QUEEN_COUPLING_RANGE : BEES_VIEW_DISTANCE, 2) && nearBee->id != bee->id) {
            return nearBee;
        }
        actualElement = actualElement->nextElement;
    }
    return NULL;
}

Location *getNearLocation(World* world, Hive* hive, Bee *bee, int radius, int attempts) {

    if (attempts >= 1000) {
        return NULL;
    }

    Location *beeLocation = bee->location;
    bool xPositive = rand() % 2 == 0;
    bool yPositive = rand() % 2 == 0;

    int newRandomXRadius = rand() % radius;
    int newRandomYRadius = rand() % radius;

    int newX = (xPositive ? beeLocation->x + newRandomXRadius : beeLocation->x - newRandomXRadius);
    int newY = (yPositive ? beeLocation->y + newRandomYRadius : beeLocation->y - newRandomYRadius);

    Location *nearLocation = createLocation(newX, newY);

    if(bee->role == QUEEN && distanceSquared(hive->locations->firstElement->value, nearLocation) > pow(QUEEN_MAX_RANDOM_WALK_RANGE, 2)){
        free(nearLocation);
        return getNearLocation(world, hive, bee, radius, attempts + 1);
    }

    if (nearLocation->x < 0 || nearLocation->x >= WORLD_SIZE || nearLocation->y < 0 ||nearLocation->y >= WORLD_SIZE || distanceSquared(beeLocation, nearLocation) <= pow(10, 2) || isHiveAround(world, nearLocation, 30)) {
        free(nearLocation);
        return getNearLocation(world, hive, bee, radius, attempts + 1);
    }
    return nearLocation;
}

void determineNextLocations(World* world, Hive* hive, Bee *bee) {
    //printf("Determining next locations for bee %d\n", bee->id);
    Location *nearLocation = getNearLocation(world, hive, bee, 50, 0);

    if (nearLocation != NULL) {
        randomPath(bee, nearLocation);
    }
    //  printf("Added %d next locations to bee %d\n", newPath[0] + newPath[1], bee->id);
}

void randomPath(Bee *bee, Location *locationToGo) {
    int differenceX = locationToGo->x - bee->location->x;
    int differenceY = locationToGo->y - bee->location->y;
    /* printf("Location to go: %d, %d\n", locationToGo->x, locationToGo->y);
     printf("Bee location: %d, %d\n", bee->location->x, bee->location->y);
     printf("Difference: %d, %d\n", differenceX, differenceY);*/

    bee->nextLocations[0] = differenceX;
    bee->nextLocations[1] = differenceY;
}