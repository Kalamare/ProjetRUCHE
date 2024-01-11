#include "randomwalker.h"

/**
 * Check if there is a flower nearby the bee
 * @param flowers
 * @param bee
 * @return
 */
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

/**
 * Check if there is a bee with specified role nearby the bee
 * @param bees
 * @param bee
 * @param role
 * @return
 */
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

/**
 * Get random location near the bee
 * @param world
 * @param hive
 * @param bee
 * @param radius
 * @param attempts
 * @return
 */
Location *getNearLocation(World* world, Hive* hive, Bee *bee, int radius, int attempts) {

    if (attempts >= 1000) {
        return NULL;
    }

    Location *beeLocation = bee->location;
    bool xPositive = rand() % 2 == 0; // x positive or negative
    bool yPositive = rand() % 2 == 0; // y positive or negative

    int newRandomXRadius =  rand() % radius; // random number between 0 and radius
    int newRandomYRadius = rand() % radius; // random number between 0 and radius

    int newX = (xPositive ? beeLocation->x + newRandomXRadius : beeLocation->x - newRandomXRadius); // new x location
    int newY = (yPositive ? beeLocation->y + newRandomYRadius : beeLocation->y - newRandomYRadius); // new y location

    Location *nearLocation = createLocation(newX, newY);

    bool finded = true;

    if (bee->role == QUEEN && distanceSquared(hive->locations->firstElement->value, nearLocation) > pow(QUEEN_MAX_RANDOM_WALK_RANGE, 2)){
        finded = false;
    } else if(bee->role == DRONE && distanceSquared(hive->locations->firstElement->value, nearLocation) > pow(DRONE_MAX_RANDOM_WALK_RANGE, 2)){
        finded = false;
    } else if (nearLocation->x < 0 || nearLocation->x >= world->width || nearLocation->y < 0 || nearLocation->y >= world->height || distanceSquared(beeLocation, nearLocation) <= pow(10, 2) || isHiveAround(world, nearLocation, 30)) {
        finded = false;
    }

    if (!finded){ // if the location is not valid
        free(nearLocation); // free memory
        return getNearLocation(world, hive, bee, radius, attempts + 1); // try again
    }
    return nearLocation;
}

/**
 * Determine next locations for the bee
 * @param world
 * @param hive
 * @param bee
 */
void determineNextLocations(World* world, Hive* hive, Bee *bee) {
    //printf("Determining next locations for bee %d\n", bee->id);
    Location *nearLocation = getNearLocation(world, hive, bee, SEARCH_LOCATION_RADIUS, 0); // get random location near the bee

    if (nearLocation != NULL) {
        randomPath(bee, nearLocation); // random path to the location
    }
    //  printf("Added %d next locations to bee %d\n", newPath[0] + newPath[1], bee->id);
}

/**
 * Random path to the location
 * @param bee
 * @param locationToGo
 */
void randomPath(Bee *bee, Location *locationToGo) {
    int differenceX = locationToGo->x - bee->location->x;
    int differenceY = locationToGo->y - bee->location->y;
    /* printf("Location to go: %d, %d\n", locationToGo->x, locationToGo->y);
     printf("Bee location: %d, %d\n", bee->location->x, bee->location->y);
     printf("Difference: %d, %d\n", differenceX, differenceY);*/

    bee->nextLocations[0] = differenceX; // set next x location changes in the array
    bee->nextLocations[1] = differenceY; // set next y location changes in the array
}