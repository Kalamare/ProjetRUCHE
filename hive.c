#include "hive.h"

bool isIdling(Bee *bee) {
    return bee->idleTime > 0;
}

Location *getLocationFromList(List *locations, int position) {
    if (locations == NULL) {
        return NULL;
    }
    ListElement *actualElement = locations->firstElement;

    for (int i = 0; i < position; i++) {
        if (actualElement->nextElement == NULL) {
            return NULL;
        }
        actualElement = actualElement->nextElement;
    }
    return actualElement->value;
}

Location *getNextLocation(Bee *bee) {
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


Hive *createHive(int id, int x, int y, int height, int width) {
    Hive *hive = malloc(sizeof(Hive));
    hive->id = id;
    hive->height = height;
    hive->width = width;
    hive->time = 0;
    hive->beesCount = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Location *location = createLocation(x + j, y + i);
            addLocation(hive, location);
        }
    }

    printf("-- Hive created --\n");
    printf("> Id: %d\n", hive->id);
    printf("> Height: %d\n", hive->height);
    printf("> Width: %d\n", hive->width);
    printf("> Time: %ld\n", hive->time);
    printf("> Bees count: %d\n", hive->beesCount);
    printf("-------------------\n");

    return hive;
}

Bee *createBee(int id, Location *location, enum Role role, int health, int lifeTime, int idleTime, int *nextLocations) {
    Bee *bee = malloc(sizeof(Bee));
    bee->id = id;
    bee->location = location;
    bee->role = role;
    bee->health = health;
    bee->lifeTime = lifeTime;
    bee->idleTime = idleTime;
    bee->nextLocations = nextLocations;

    printf("-- Bee created --\n");
    printf("> Id: %d\n", bee->id);
    printf("> Health: %d\n", bee->health);
    printf("> Honey: %d\n", bee->honey);
    printf("> Location: %d, %d\n", bee->location->x, bee->location->y);
    printf("> Role: %d\n", bee->role);
    printf("> Life time: %d\n", bee->lifeTime);
    printf("> Idle time: %d\n", bee->idleTime);
    printf("> Next locations: %p\n", bee->nextLocations);
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

void triggerDisease(Bee *bee) {
    if (bee->disease) {
        return;
    }
    bool isDiseased = rand() % 400 == 0;

    if (isDiseased) {
        printf("Bee %d is diseased\n", bee->id);
        bee->disease = true;
    }
}

void tryEvolve(Bee *bee) {
    bool canEvolve = rand() % 10 == 0;

    if (!canEvolve) {
        return;
    }
    bool evolved = false;

    switch (bee->role) {
        case EGG:
            if (bee->lifeTime - bee->health >= EGG_LIFE_TIME) {
                bee->role = LARVA;
                evolved = true;
            }
            break;
        case LARVA:
            if (bee->lifeTime - bee->health >= LARVA_LIFE_TIME) {
                bee->role = NYMPH;
                evolved = true;
            }
            break;
        case NYMPH:
            if (bee->lifeTime - bee->health >= NYMPH_LIFE_TIME) {
                bee->role = WORKER;
                evolved = true;
            }
            break;
        default:
            break;
    }

    if (evolved) {
        printf("Bee %d evolved to %d\n", bee->id, bee->role);
        bee->health = bee->lifeTime;
    }
}