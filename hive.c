#include "hive.h"
#include "randomwalker.h"

/**
 * Check if the bee is idling
 * @param bee
 * @return
 */
bool isIdling(Bee *bee) {
    return bee->idleTime > 0;
}

/**
 * Check if the bee is going to a flower
 * @param bee
 * @return
 */
bool isGoingToFlower(Bee *bee) {
    bool isGoingToFlower = bee->flowerToGo != -1;
    return isGoingToFlower;
}

/**
 * Check if the queen can go coupling
 * @param queen
 * @return
 */
bool canQueenGoCoupling(Bee* queen){
    return queen != NULL && queen->lifeTime - queen->health >= DAYS_BEFORE_GO_COUPLING;
}

/**
 * Check if the queen can start laying eggs
 * @param queen
 * @return
 */
bool canQueenStartLayingEggs(Bee* queen){
    return queen->couplingTimes >= COUPLING_TIMES_BEFORE_LAY_EGGS;
}

/**
 * Get the location from the list by position
 * @param locations
 * @param position
 * @return
 */
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

/**
 * Get the next location of the bee from the next locations array cached
 * @param bee
 * @return
 */
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

/**
 * Method to create a hive
 * @param id
 * @param x
 * @param y
 * @param height
 * @param width
 * @return
 */
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
            addLocation(hive, location); // Add the location to the hive
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

/**
 * Method to create a lake
 * @param id
 * @param x
 * @param y
 * @param height
 * @param width
 * @return
 */
Lake *createLake(int id, int x, int y, int height, int width) {
    Lake *lake = malloc(sizeof(Lake));
    lake->id = id;
    lake->height = height;
    lake->width = width;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Location *location = createLocation(x + j, y + i);
            addLakeLocation(lake, location);
        }
    }
    return lake;
}

/**
 * Method to create a bee
 * @param id
 * @param location
 * @param role
 * @param food
 * @param health
 * @param lifeTime
 * @param idleTime
 * @param nextLocations
 * @return
 */
Bee *createBee(int id, Location *location, enum Role role, int food, int health, int lifeTime, int idleTime, int *nextLocations) {
    Bee *bee = malloc(sizeof(Bee));
    bee->id = id;
    bee->location = location;
    bee->role = role;
    bee->health = health;
    bee->food = food;
    bee->lifeTime = lifeTime;
    bee->idleTime = idleTime;
    bee->nextLocations = nextLocations;
    bee->flowerToGo = -1;

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

/**
 * Method to add a bee to the hive
 * @param hive
 * @param bee
 */
void addBee(Hive *hive, Bee *bee) {
    if (hive->bees == NULL) {
        hive->bees = createList();
    }
    ListElement *listElement = malloc(sizeof(ListElement));
    listElement->value = bee;
    listElement->nextElement = NULL;
    insertElement(hive->bees, listElement);
}

/**
 * Method to create a flower
 * @param id
 * @param location
 * @param initialCapacity
 * @param capacity
 * @param honeyGiven
 * @return
 */
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

/**
 * Method to add a location to the hive
 * @param hive
 * @param location
 */
void addLocation(Hive *hive, Location *location) {
    if (hive->locations == NULL) {
        hive->locations = createList();
    }
    ListElement *listElement = malloc(sizeof(ListElement));
    listElement->value = location;
    listElement->nextElement = NULL;
    insertElement(hive->locations, listElement);
}

/**
 * Method to add a location to the lake
 * @param lake
 * @param location
 */
void addLakeLocation(Lake *lake, Location *location) {
    if (lake->locations == NULL) {
        lake->locations = createList();
    }
    ListElement *listElement = malloc(sizeof(ListElement));
    listElement->value = location;
    listElement->nextElement = NULL;
    insertElement(lake->locations, listElement);
}

/**
 * Trigger the bee food loss
 * @param hive
 * @param bee
 * @param frameRate
 */
void triggerFoodLoss(Hive *hive, Bee *bee, int frameRate) {
    if (bee->food == 0 && rand() % 50 == 0) {
        bee->health -= 3; // Remove 3 health points if the bee has no food
        return;
    }
    bool lostFood = rand() % 3000 * (frameRate / 60) == 0;

    if (!lostFood) {
        return;
    }
    bee->food--;
    //printf("Bee %d lost 1 food\n", bee->id);

    if (!bee->mustGoToHive && bee->food <= (DEFAULT_FOOD * FEED_TRESHOLD)) {
        printf("Bee %d is hungry\n", bee->id);
        List *hiveLocations = hive->locations;
        Location *randomHiveLocation = getLocationFromList(hiveLocations, rand() % hiveLocations->size); // Get a random hive location
        bee->mustGoToHive = true;
        randomPath(bee, randomHiveLocation); // Set the bee to go to the hive
    }
}

/**
 * Trigger if the bee should die faster
 * @param bee
 * @param frameRate
 */
void triggerDisease(Bee *bee, int frameRate) {
    if (bee->disease) {
        return;
    }
    bool isDiseased = rand() % 800 == 0;

    if (isDiseased) {
        //printf("Bee %d is diseased\n", bee->id);
        bee->disease = true; // Set the bee to be diseased
    }
}

/**
 * Remove the flower by id
 * @param flowers
 * @param id
 */
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

/**
 * Handle the egg laying
 * @param queen
 * @param hive
 */
void handleEggsLaying(Bee *queen, Hive* hive) {
    if (queen->mustGoToHive) { // Check if the queen is going to the hive
        return;
    }
    bool canLayEggs = rand() % 4 == 0;

    if (!canLayEggs) {
        return;
    }
    int eggsCount =  MAX_EGGS_PER_DAY - (rand() % (MAX_EGGS_PER_DAY / 2));

    for (int i = 0; i < eggsCount; i++) {
        Location *location = createLocation(queen->location->x, queen->location->y); // Create a new location for the egg
        Bee *egg = createBee(++hive->beesCount, location, EGG, DEFAULT_FOOD, WORKER_MAX_LIFE_TIME, WORKER_MAX_LIFE_TIME, 0, malloc(sizeof(int) * 2));
        addBee(hive, egg); // Add the egg to the hive
    }
    printf("Queen %d laid %d eggs\n", queen->id, eggsCount);
}

/**
 * Handle the feeding of the bee
 * @param bee
 * @param hive
 */
void handleBeeFeeding(Bee *bee, Hive *hive) {
    if (bee->food > (DEFAULT_FOOD * FEED_TRESHOLD) || hive->nectar == 0) {
        //printf("Bee is not hungry or hive has no nectar\n");
        return;
    }
    int toFeed = (int) (DEFAULT_FOOD * FEED_AMOUNT_RATIO) * 2; // Calculate the amount of nectar to feed the bee

    if (toFeed > hive->nectar) { // If the hive has less nectar than the bee needs
        toFeed = hive->nectar;
    }
    bee->food += toFeed; // Feed the bee
    hive->nectar -= toFeed; // Remove the nectar from the hive
    printf("Bee %d fed %d\n", bee->id, toFeed);
}

/**
 * Handle the nectar transfer from the bee to the hive
 * @param bee
 * @param hive
 */
void handleNectarTransfer(Bee *bee, Hive *hive) {
    if (bee->nectar == 0 || hive->nectar >= MAX_NECTAR_CAPACITY) {
        //printf("Bee has no nectar or hive has max nectar capacity\n");
        return;
    }

    if (hive->nectar + bee->nectar > MAX_NECTAR_CAPACITY) { // Check if the hive has enough space to store the nectar
        bee->nectar = MAX_NECTAR_CAPACITY - hive->nectar;
        hive->nectar = MAX_NECTAR_CAPACITY;
        return;
    }
    hive->nectar += bee->nectar; // Transfer the nectar from the bee to the hive
    bee->nectar = 0; // Remove the nectar from the bee
}

/**
 * Get the flower by id
 * @param flowers
 * @param id
 * @return
 */
Flower* getFlowerById(List *flowers, int id) {
    ListElement *actualElement = flowers->firstElement;
    Flower *flower;

    while (actualElement != NULL) {
        flower = actualElement->value;

        if (flower->id == id) {
            return flower;
        }
        actualElement = actualElement->nextElement;
    }
    return NULL;
}

/**
 * Handle the honey harvest
 * @param flowers
 * @param bee
 * @return
 */
int triggerHarvest(List *flowers, Bee *bee) {
    Flower *flower = getFlowerById(flowers, bee->flowerToGo);

    if (flower == NULL || flower->capacity <= 0) {
        return 0;
    }
    bool killFlower = false;

    int honeyGiven = flower->nectarGiven - (rand() % (flower->nectarGiven - 2)); // Calculate the amount of honey to give to the bee

    if (honeyGiven <= 0 || flower->capacity < honeyGiven) { // Check if the flower has enough honey to give to the bee
        honeyGiven = flower->capacity;
        killFlower = true; // If the flower has no honey left, kill it
    }
    flower->capacity -= honeyGiven; // Remove the honey from the flower

    //bee->nectar += honeyGiven;

    if (killFlower) {
        removeFlowerById(flowers, flower->id); // Remove the flower from the list
    }
    return honeyGiven;
}

/**
 * @return a random role for the bee
 */
enum Role chooseRole() {
    int random = rand() % 100;

    if (random <= 15){ // 15% chance to be a worker
        return WORKER;
    }
    if (random <= 35){ // 20% chance to be a queen
        return DRONE;
    }
    return FORAGER; // 65% chance to be a forager
}

/**
 * Evolve the bee if it can
 * @param bee
 */
void tryEvolve(Bee *bee) {
    bool canEvolve = rand() % 2 == 0;

    if (!canEvolve) {
        return;
    }
    bool evolved = false;

    switch (bee->role) {
        case EGG:
            if (bee->lifeTime - bee->health >= EGG_LIFE_TIME - (rand() %  EGG_LIFE_TIME / 3)) { // Check if the bee can evolve
                bee->role = LARVA;
                evolved = true;
            }
            break;
        case LARVA:
            if (bee->lifeTime - bee->health >= LARVA_LIFE_TIME - (rand() %  LARVA_LIFE_TIME / 3)) { // Check if the bee can evolve
                bee->role = NYMPH;
                evolved = true;
            }
            break;
        case NYMPH:
            if (bee->lifeTime - bee->health >= NYMPH_LIFE_TIME - (rand() %  NYMPH_LIFE_TIME / 3)) { // Check if the bee can evolve
                bee->role = chooseRole(); // Choose a random role

                if (bee-> role == DRONE){
                    bee->inHive = true; // If the bee is a drone, put it in the hive
                }
                evolved = true;
            }
            break;
        default:
            break;
    }

    if (evolved) {
        bee->health = bee->lifeTime; // Set the bee health to the max
    }
}