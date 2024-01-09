#include "hive.h"
#include "randomwalker.h"

bool isIdling(Bee *bee) {
    return bee->idleTime > 0;
}


bool hasNextLocations(Bee *bee) {
    int *nextLocations = bee->nextLocations;
    return nextLocations != NULL && (nextLocations[0] != 0 || nextLocations[1] != 0);
}

Location *getNextLocation(Bee *bee, bool random) {
    int *nextLocations = bee->nextLocations;
    Location *nextLocation = createLocation(bee->location->x, bee->location->y);
    bool isX = rand() % 2 == 0;

    if (isX && nextLocations[0] != 0) {
        if (nextLocations[0] > 0) {
            nextLocation->x += 1;
            nextLocations[0]--;
        } else if (nextLocations[0] < 0) {
            nextLocation->x -= 1;
            nextLocations[0]++;
        }
        return nextLocation;
    }

    if (nextLocations[1] != 0) {
        if (nextLocations[1] > 0) {
            nextLocation->y += 1;
            nextLocations[1]--;
        } else if (nextLocations[1] < 0) {
            nextLocation->y -= 1;
            nextLocations[1]++;
        }
        return nextLocation;
    }
    return nextLocation;
}


void moveBee(World *world, Bee *bee, unsigned int frameRate) {
    if (isIdling(bee) && world->time % frameRate != 0) {
        bee->idleTime--;
        return;
    }

    if (hasNextLocations(bee)) {
        Location *nextLocation = getNextLocation(bee, !bee->goingToFlower);
        bee->location = nextLocation; // Set new location to bee
        printf("Bee %d moved to %d, %d\n", bee->id, nextLocation->x, nextLocation->y);

        if (!hasNextLocations(bee) && bee->goingToFlower) {
            bee->goingToFlower = false;
            bee->mustGoBack = true;
        }
        //  printf("Bee already has next locations, moving to %d, %d\n", bee->location->x, bee->location->y);
        return;
    }

    Location *flowerLocation = checkNearbyFlowers(world, bee);

    if (flowerLocation != NULL) {
        printf("Bee %d found flower at %d, %d\n", bee->id, flowerLocation->x, flowerLocation->y);
        printf("Bee %d honey: %d\n", bee->id, bee->honey);

        randomPath(world, bee, flowerLocation);
        printf("Bee %d path: %d, %d\n", bee->id, bee->nextLocations[0], bee->nextLocations[1]);
        bee->goingToFlower = true;
    } else {
        determineNextLocations(world, bee);
    }
    // printf("Bee %d moved to %d, %d\n", bee->id, bee->location->x, bee->location->y);


}

void tickBees(World *world, Hive *hive, unsigned int frameRate) {
    ListElement *actualElement = hive->bees->firstElement;
    Bee *bee;

    while (actualElement != NULL) {
        bee = actualElement->value;

        if (bee->dead) {
            actualElement = actualElement->nextElement;
            continue;
        }

        if (world->time % frameRate == 0) {
            bee->lifeTime--;
            // printf("Bee %d life time: %d\n", bee->id, bee->lifeTime);

            if (bee->lifeTime <= 0) {
                bee->dead = true;
                printf("Bee %d died\n", bee->id);
                actualElement = actualElement->nextElement;
                continue;
            }
        }
        moveBee(world, bee, frameRate);
        actualElement = actualElement->nextElement;
    }
}

Hive *createHive(int id, int height, int width) {
    Hive *hive = malloc(sizeof(Hive));
    hive->id = id;
    hive->height = height;
    hive->width = width;
    hive->time = 0;
    hive->beesCount = 0;

    printf("-- Hive created --\n");
    printf("> Id: %d\n", hive->id);
    printf("> Height: %d\n", hive->height);
    printf("> Width: %d\n", hive->width);
    printf("> Time: %ld\n", hive->time);
    printf("> Bees count: %d\n", hive->beesCount);
    printf("-------------------\n");

    return hive;
}

Bee *
createBee(int id, int health, unsigned honey, Location *location, enum Role role, bool disease, bool dead, int lifeTime,
          long idleTime, int *nextLocations, bool mustGoBack, bool goingToFlower) {
    Bee *bee = malloc(sizeof(Bee));
    bee->id = id;
    bee->health = health;
    bee->honey = honey;
    bee->location = location;
    bee->role = role;
    bee->disease = disease;
    bee->dead = dead;
    bee->lifeTime = lifeTime;
    bee->idleTime = idleTime;
    bee->nextLocations = nextLocations;
    bee->mustGoBack = mustGoBack;
    bee->goingToFlower = goingToFlower;

    printf("-- Bee created --\n");
    printf("> Id: %d\n", bee->id);
    printf("> Health: %d\n", bee->health);
    printf("> Honey: %d\n", bee->honey);
    printf("> Location: %d, %d\n", bee->location->x, bee->location->y);
    printf("> Role: %d\n", bee->role);
    printf("> Disease: %d\n", bee->disease);
    printf("> Dead: %d\n", bee->dead);
    printf("> Life time: %d\n", bee->lifeTime);
    printf("> Idle time: %ld\n", bee->idleTime);
    printf("> Next locations: %p\n", bee->nextLocations);
    printf("> Must go back: %d\n", bee->mustGoBack);
    printf("> Going to flower: %d\n", bee->goingToFlower);
    printf("-------------------\n");

    return bee;
}

void addBee(Hive *hive, Bee *bee) {
    if (hive->bees == NULL) {
        hive->bees = createList();
    }
    ListElement *listElement = malloc(sizeof(ListElement));
    listElement->value = bee;
    listElement->nextElement = NULL;
    insertElement(hive->bees, listElement);
}

void addLocation(Hive *hive, Location *location) {
    if (hive->locations == NULL) {
        hive->locations = createList();
    }
    ListElement *listElement = malloc(sizeof(ListElement));
    listElement->value = location;
    listElement->nextElement = NULL;
    insertElement(hive->locations, listElement);
}