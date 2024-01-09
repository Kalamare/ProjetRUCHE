#include "hive.h"
#include "randomwalker.h"

bool isIdling(Bee *bee) {
    return bee->idleTime > 0;
}

bool isGoingToFlower(Bee *bee) {
    return bee->flowerToGo != NULL;
}

bool canQueenGoCoupling(Bee* queen){
    return queen->lifeTime - queen->health >= DAYS_BEFORE_GO_COUPLING;
}

bool canQueenStartLayingEggs(Bee* queen){
    return queen->couplingTimes >= COUPLING_TIMES_BEFORE_LAY_EGGS;
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

    /* printf("-- Hive created --\n");
     printf("> Id: %d\n", hive->id);
     printf("> Height: %d\n", hive->height);
     printf("> Width: %d\n", hive->width);
     printf("> Time: %ld\n", hive->time);
     printf("> Bees count: %d\n", hive->beesCount);
     printf("-------------------\n");*/

    return hive;
}

Bee *createBee(int id, Location *location, enum Role role, int food, int health, int lifeTime, int idleTime,
               int *nextLocations) {
    Bee *bee = malloc(sizeof(Bee));
    bee->id = id;
    bee->location = location;
    bee->role = role;
    bee->health = health;
    bee->food = food;
    bee->lifeTime = lifeTime;
    bee->idleTime = idleTime;
    bee->nextLocations = nextLocations;

    /*  printf("-- Bee created --\n");
      printf("> Id: %d\n", bee->id);
      printf("> Health: %d\n", bee->health);
      printf("> Honey: %d\n", bee->nectar);
      printf("> Location: %d, %d\n", bee->location->x, bee->location->y);
      printf("> Role: %d\n", bee->role);
      printf("> Life time: %d\n", bee->lifeTime);
      printf("> Idle time: %d\n", bee->idleTime);
      printf("> Next locations: %p\n", bee->nextLocations);
      printf("-------------------\n");*/

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

Flower *createFlower(int id, Location *location, int initialCapacity, int capacity, int honeyGiven) {
    Flower *flower = malloc(sizeof(Flower));
    flower->id = id;
    flower->location = location;
    flower->initialCapacity = initialCapacity;
    flower->capacity = capacity;
    flower->nectarGiven = honeyGiven;

    /*printf("-- Flower created --\n");
    printf("> Location: %d, %d\n", location->x, location->y);
    printf("> Initial capacity: %d\n", flower->initialCapacity);
    printf("> Capacity: %d\n", flower->capacity);
    printf("> Honey given: %d\n", flower->nectarGiven);
    printf("-------------------\n");*/

    return flower;
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

void triggerFoodLoss(Hive *hive, Bee *bee) {
    if (bee->food == 0) {
        bee->health -= 3;
        return;
    }
    bool lostFood = rand() % 3000 == 0;

    if (!lostFood) {
        return;
    }
    bee->food--;
    //printf("Bee %d lost 1 food\n", bee->id);

    if (!bee->mustGoToHive && bee->food <= (DEFAULT_FOOD * FEED_TRESHOLD)) {
       // printf("Bee %d is hungry\n", bee->id);
        List *hiveLocations = hive->locations;
        Location *randomHiveLocation = getLocationFromList(hiveLocations, rand() % hiveLocations->size);
        bee->mustGoToHive = true;
        randomPath(bee, randomHiveLocation);
    }
}

void triggerDisease(Bee *bee) {
    if (bee->disease) {
        return;
    }
    bool isDiseased = rand() % 400 == 0;

    if (isDiseased) {
        //printf("Bee %d is diseased\n", bee->id);
        bee->disease = true;
    }
}

void removeFlowerById(List *flowers, int id) {
    ListElement *actualElement = flowers->firstElement;
    Flower *flower;

    while (actualElement != NULL) {
        flower = actualElement->value;

        if (flower->id == id) {
            removeElement(flowers, actualElement);
            return;
        }
        actualElement = actualElement->nextElement;
    }
}

void handleEggsLaying(Bee *queen, Hive* hive) {
    bool canLayEggs = rand() % 3 == 0;

    if (!canLayEggs) {
        return;
    }
    int eggsCount =  MAX_EGGS_PER_DAY - (rand() % (MAX_EGGS_PER_DAY / 2));

    for (int i = 0; i < eggsCount; i++) {
        Location *location = createLocation(queen->location->x, queen->location->y);
        Bee *egg = createBee(++hive->beesCount, location, EGG, DEFAULT_FOOD, WORKER_MAX_LIFE_TIME, WORKER_MAX_LIFE_TIME, 0, malloc(sizeof(int) * 2));
        addBee(hive, egg);
    }
    printf("Queen %d laid %d eggs\n", queen->id, eggsCount);
}


void handleBeeFeeding(Bee *bee, Hive *hive) {
    if (bee->food > (DEFAULT_FOOD * FEED_TRESHOLD) || hive->nectar == 0) {
        return;
    }
    int toFeed = (int) (DEFAULT_FOOD * FEED_AMOUNT_RATIO);

    if (toFeed > hive->nectar) {
        toFeed = hive->nectar;
    }
    bee->food += toFeed;
    hive->nectar -= toFeed;
    //printf("Bee %d fed %d\n", bee->id, toFeed);
}

void handleNectarTransfer(Bee *bee, Hive *hive) {
    if (bee->nectar == 0 || hive->nectar >= MAX_NECTAR_CAPACITY) {
        return;
    }

    if (hive->nectar + bee->nectar > MAX_NECTAR_CAPACITY) {
        bee->nectar = MAX_NECTAR_CAPACITY - hive->nectar;
        hive->nectar = MAX_NECTAR_CAPACITY;
        printf("Max nectar capacity reached\n");
        return;
    }
    hive->nectar += bee->nectar;
    /* printf("Bee %d transferred %d nectar to hive %d\n", bee->id, bee->nectar, hive->id);
     printf("Hive %d nectar: %d\n", hive->id, hive->nectar);*/
    bee->nectar = 0;
}

void triggerHarvest(List *flowers, Bee *bee) {
    Flower *flower = bee->flowerToGo;

    if (flower == NULL) {
        return;
    }
    bool killFlower = false;
    int honeyGiven = flower->nectarGiven - (rand() % (flower->nectarGiven - 2));

    if (honeyGiven <= 0 || flower->capacity < honeyGiven) {
        honeyGiven = flower->capacity;
        killFlower = true;
    }

    flower->capacity -= honeyGiven;
    bee->nectar += honeyGiven;

    if (killFlower) {
        printf("Bee %d killed flower %d\n", bee->id, flower->id);
        removeFlowerById(flowers, flower->id);
    }
    bee->flowerToGo = NULL;
}

void tryEvolve(Bee *bee) {
    bool canEvolve = rand() % 10 == 0;

    if (!canEvolve) {
        return;
    }
    bool evolved = false;

    switch (bee->role) {
        case EGG:
            if (bee->lifeTime - bee->health >= EGG_LIFE_TIME - (rand() %  EGG_LIFE_TIME / 3)) {
                bee->role = LARVA;
                evolved = true;
            }
            break;
        case LARVA:
            if (bee->lifeTime - bee->health >= LARVA_LIFE_TIME - (rand() %  LARVA_LIFE_TIME / 3)) {
                bee->role = NYMPH;
                evolved = true;
            }
            break;
        case NYMPH:
            if (bee->lifeTime - bee->health >= NYMPH_LIFE_TIME - (rand() %  NYMPH_LIFE_TIME / 3)) {
                bee->role = rand() % 2 == 0 ? DRONE : WORKER;
                evolved = true;
            }
            break;
        default:
            break;
    }

    if (evolved) {
        //printf("Bee %d evolved to %d\n", bee->id, bee->role);
        bee->health = bee->lifeTime;
    }
}