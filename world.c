#include "world.h"
#include "randomwalker.h"

bool isOutOfWorld(Location *location, World *world) {
    return location == NULL ||
           (location->x < 0 || location->x >= world->width || location->y < 0 || location->y >= world->height);
}

bool hasNextLocations(Bee *bee) {
    if (bee == NULL) {
        return false;
    }
    int *nextLocations = bee->nextLocations;

    bool hasNextLocations = nextLocations != NULL && (nextLocations[0] != 0 || nextLocations[1] != 0);
    return hasNextLocations;
}

bool isSecondTick(World *world, int frameRate) {
    return world->time % frameRate == 0;
}

bool isLakeAround(World *world, Location *location, int radius) {
    if (world->lakes == NULL) {
        return false;
    }
    ListElement *actualElement = world->lakes->firstElement;
    Lake *lake;

    while (actualElement != NULL) {
        lake = actualElement->value;
        ListElement *actualLocationElement = lake->locations->firstElement;

        while (actualLocationElement != NULL) {
            Location *lakeLocation = actualLocationElement->value;

            if (distanceSquared(location, lakeLocation) <= pow(radius, 2)) {
                return true;
            }
            actualLocationElement = actualLocationElement->nextElement;
        }
        actualElement = actualElement->nextElement;
    }
    return false;
}

bool isFlowerAround(World *world, Location *location, int radius) {
    if (world->flowers == NULL) {
        return false;
    }
    ListElement *actualElement = world->flowers->firstElement;
    Flower *flower;

    while (actualElement != NULL) {
        flower = actualElement->value;
        Location *flowerLocation = flower->location;

        if (distanceSquared(location, flowerLocation) <= pow(radius, 2)) {
            return true;
        }
        actualElement = actualElement->nextElement;
    }
    return false;
}

bool isHiveAround(World *world, Location *location, int radius) {
    if (world->hives == NULL) {
        return false;
    }
    ListElement *actualHiveElement = world->hives->firstElement;
    Hive *hive;

    while (actualHiveElement != NULL) {
        hive = actualHiveElement->value;
        ListElement *actualLocationElement = hive->locations->firstElement;
        Location *hiveLocation;

        while (actualLocationElement != NULL) {
            hiveLocation = actualLocationElement->value;

            if (distanceSquared(location, hiveLocation) <= pow(radius, 2)) {
                return true;
            }
            actualLocationElement = actualLocationElement->nextElement;
        }
        actualHiveElement = actualHiveElement->nextElement;
    }
    return false;
}

bool isBorderAround(World *world, Location *location, int radius) {
    return location->x <= radius || location->x >= world->width - radius || location->y <= radius ||location->y >= world->height - radius;
}

/**
 *
 * @param hive
 * @param bee
 * @param mustGoToHive
 *
 * Method to redirect bee to hive
 */
void goToHive(Hive *hive, Bee *bee, bool mustGoToHive) {
    List *hiveLocations = hive->locations;
    Location *randomHiveLocation = getLocationFromList(hiveLocations, rand() % hiveLocations->size);
    randomPath(bee, randomHiveLocation);

    if (mustGoToHive) {
        bee->mustGoToHive = true;
    }
}

/**
 *
 * @param hive
 * @param bee
 * @param isQueen
 *
 * Method to redirect bee to hive
 */
void goRoleToHive(Hive *hive, enum Role role) {
    ListElement *actualElement = hive->bees->firstElement;

    while (actualElement != NULL) {
        Bee *bee = actualElement->value;

        if (bee->role == role) {
            goToHive(hive, bee, role == QUEEN || role == DRONE);
        }
        actualElement = actualElement->nextElement;
    }
}

/**
 *
 * @param world
 * @param hive
 * @param bee
 * @param frameRate
 *
 * Main method for moving bees
 */

void moveBee(World *world, Hive *hive, Bee *bee, int frameRate) {
    if ((bee->role == QUEEN || bee->role == DRONE) && !canQueenGoCoupling(bee)) {
        return;
    }
    if (bee->role == QUEEN && isSecondTick(world, frameRate) && !canQueenStartLayingEggs(bee)) {
        Bee *nearBee = checkNearbyBees(hive->bees, bee, DRONE);

        if (nearBee != NULL && !nearBee->dead && !isGoingToFlower(nearBee)) { // When queen is near another bee and the other bee is not going to flower
            nearBee->dead = true;
            printf("Drone %d died by coupling with queen\n", nearBee->id);

            if (++bee->couplingTimes >= COUPLING_TIMES_BEFORE_LAY_EGGS) {
                goToHive(hive, bee, true);
                goRoleToHive(hive, DRONE);
            }
        }
    }

    if (isIdling(bee)) {
        if (!isSecondTick(world, frameRate) || --bee->idleTime > 0) {
            return;
        }
        if (bee->mustGoToHive) {
            bee->inHive = false;
            bee->mustGoToHive = false;
        } else if (isGoingToFlower(bee)) {
            //printf("Bee %d nectar: %d\n", bee->id, bee->nectar);
            if (bee->harvestNectar > 0) {
                bee->nectar += bee->harvestNectar;
                bee->harvestNectar = 0;
                bee->mustGoToHive = true;
                bee->flowerToGo = -1;
            }
        }
        return;
    }

    if (hasNextLocations(bee)) {
        Location *nextLocation = getNextLocation(bee);
        bee->location = nextLocation; // Set new location to bee
        //printf("Bee %d moved to %d, %d\n", bee->id, nextLocation->x, nextLocation->y);
        triggerFoodLoss(hive, bee, frameRate);

        if (hasNextLocations(bee)) {
            return;
        }

        if (isGoingToFlower(bee)) {
            int harvestNectar = triggerHarvest(world->flowers, bee);
            if (harvestNectar > 0) {
                bee->harvestNectar += harvestNectar;
                bee->idleTime = 3; // Idling for 3 seconds
                goToHive(hive, bee, false);
            }
            return;
        }
        if (bee->mustGoToHive) {
            if (bee->role != QUEEN) {
                if (bee->role != DRONE) {
                    bee->idleTime = 5; // Idling for 5 seconds
                }
                bee->inHive = true;
            }

            handleNectarTransfer(bee, hive);
            handleBeeFeeding(bee, hive);
        }
        //  printf("Bee already has next locations, moving to %d, %d\n", bee->location->x, bee->location->y);
        return;
    }

    if (bee->mustGoToHive || isGoingToFlower(bee)) {
        if (bee->role == QUEEN) {
            bee->mustGoToHive = false;
        }
        return;
    }
    if (bee->role == DRONE && canQueenStartLayingEggs(hive->queen)) {
        return;
    }

    if (bee->role == QUEEN) {
        if (canQueenStartLayingEggs(bee)) {
            return;
        }
        determineNextLocations(world, hive, bee);
        return;
    }
    Flower *flower = checkNearbyFlowers(world->flowers, bee);

    if (flower != NULL) {
        Location *flowerLocation = flower->location;
        randomPath(bee, flowerLocation);
        bee->flowerToGo = flower->id;
    } else {
        determineNextLocations(world, hive, bee);
    }
}

/**
 *
 * @param world
 * @param flowersCount
 *
 * Method to create flowers
 */
void createFlowers(World *world, int flowersCount) {
    int attempts = 0;
    int actualFlowersCount = world->flowers == NULL ? 0 : world->flowers->size;

    for (int i = 0; i < flowersCount; i++) {

        Location *location = createLocation(rand() % (world->width - 20), rand() % (world->height - 20));

        if (attempts > 1000) {
            //printf("Too many attempts, breaking\n");
            break;
        }

        if (isFlowerAround(world, location, DISTANCE_BETWEEN_FLOWERS) || isHiveAround(world, location, DISTANCE_FLOWER_HIVE) || isLakeAround(world, location, DISTANCE_FLOWER_LAKE) || isBorderAround(world, location, DISTANCE_FLOWER_BORDER)) {
            //printf("Flower around with location %d, %d\n", location->x, location->y);
            free(location);
            if (i != 0) {
                i--;
            }
            attempts++;
            continue;
        }
        int randomCapacity = MIN_FLOWER_CAPACITY + (rand() % MIN_FLOWER_CAPACITY);
        int randomHoneyGiven = MIN_FLOWER_NECTAR_GIVEN + (rand() % MIN_FLOWER_NECTAR_GIVEN);

        Flower *flower = createFlower(actualFlowersCount + i + 1, location, randomCapacity, randomCapacity,randomHoneyGiven);
        addFlower(world, flower);
        //printf("Flower created with location %d, %d\n", location->x, location->y);
    }
}

/**
 *
 * @param world
 * @param lakesCount
 *
 * Method to create lakes
 */
void createLakes(World *world, int flowersCount) {
    int attempts = 0;
    int actualLakesCount = world->lakes == NULL ? 0 : world->lakes->size;

    for (int i = 0; i < flowersCount; i++) {

        Location *location = createLocation(rand() % (world->width - 20), rand() % (world->height - 20));

        if (attempts > 1000) {
            //printf("Too many attempts, breaking\n");
            break;
        }

        if (isLakeAround(world, location, DISTANCE_BETWEEN_LAKES) || isHiveAround(world, location, DISTANCE_LAKE_HIVE) || isBorderAround(world, location, DISTANCE_LAKE_BORDER)) {
            //printf("Flower around with location %d, %d\n", location->x, location->y);
            free(location);
            if (i != 0) {
                i--;
            }
            attempts++;
            continue;
        }
        Lake *lake = createLake(actualLakesCount + i + 1, location->x, location->y, 70, 70);
        addLake(world, lake);
    }
}

/**
 * Method to create the bees
 *
 * @param hive
 */
void createBees(Hive *hive, int beesCount) {
    int randomQueenLifeTime = QUEEN_MAX_LIFE_TIME - (rand() % (QUEEN_MAX_LIFE_TIME / 4));

    List *hiveLocations = hive->locations;
    Bee *queen = createBee(++hive->beesCount, getLocationFromList(hiveLocations, rand() % hiveLocations->size), QUEEN, QUEEN_FOOD, randomQueenLifeTime, randomQueenLifeTime, 0, malloc(sizeof(int) * 2));

    int dronesCreated = 0;

    for (int i = 0; i < beesCount; i++) {
        Location *location = getLocationFromList(hiveLocations, rand() % hiveLocations->size);

        int randomWorkerLifeTime = WORKER_MAX_LIFE_TIME - (rand() % (WORKER_MAX_LIFE_TIME / 4));
        enum Role role = chooseRole();
        Bee *bee = createBee(++hive->beesCount, location, role, DEFAULT_FOOD, randomWorkerLifeTime, randomWorkerLifeTime, 0,malloc(sizeof(int) * 2));
        //  printf("Bee role: %d\n", bee->role);
        addBee(hive, bee);

        if (role == DRONE){
            dronesCreated++;
        }
    }

    if (dronesCreated <= COUPLING_TIMES_BEFORE_LAY_EGGS ){
        for (int i = 0; i < COUPLING_TIMES_BEFORE_LAY_EGGS * 2; i++) {
            int randomWorkerLifeTime = WORKER_MAX_LIFE_TIME - (rand() % (WORKER_MAX_LIFE_TIME / 4));
            Location *location = createLocation(queen->location->x, queen->location->y);
            Bee *bee = createBee(++hive->beesCount, location, DRONE, DEFAULT_FOOD, randomWorkerLifeTime, randomWorkerLifeTime, 0,malloc(sizeof(int) * 2));
            addBee(hive, bee);
        }
    }

    addBee(hive, queen);
    hive->queen = queen;
}

/**
 *
 * @param world
 * @param frameRate
 *
 * Method to tick flowers
 */
void tickFlowers(World *world, int frameRate) {
    if (!isSecondTick(world, frameRate) || world->flowers == NULL || world->flowers->size >= INITIAL_FLOWERS) {
        return;
    }
    bool mustCreateFlower = rand() % 5 == 0;

    if (mustCreateFlower) {
        createFlowers(world, 1);
    }
}


/**
 *
 * @param world
 * @param frameRate
 *
 * Method to tick hives
 */
void tickHives(World *world, int frameRate) {
    ListElement *actualHiveElement = world->hives->firstElement;
    Hive *hive;

    while (actualHiveElement != NULL) {
        hive = actualHiveElement->value;
        hive->time++;
        tickBees(world, hive, frameRate);

        actualHiveElement = actualHiveElement->nextElement;
    }
}

/**
 *
 * @param world
 * @param hive
 * @param frameRate
 *
 * Method to tick bees
 */
void tickBees(World *world, Hive *hive, int frameRate) {
    ListElement *actualBeeElement = hive->bees->firstElement;
    Bee *bee;

    while (actualBeeElement != NULL) {
        bee = actualBeeElement->value;

        if (bee->dead) {
            actualBeeElement = actualBeeElement->nextElement;
            continue;
        }

        if (isSecondTick(world, frameRate)) {
            bee->health -= bee->disease ? 5 : 1;
            // printf("Bee %d life time: %d\n", bee->id, bee->lifeTime);

            if (bee->health <= 0) {
                bee->dead = true;
                //printf("Bee %d died\n", bee->id);

                if (bee->role == QUEEN) {
                    printf("Queen died, creating new queen\n");
                    int randomQueenLifeTime = QUEEN_MAX_LIFE_TIME - (rand() % (QUEEN_MAX_LIFE_TIME / 4));

                    Bee *queen = createBee(++hive->beesCount,getLocationFromList(hive->locations, rand() % hive->locations->size), QUEEN,QUEEN_FOOD, randomQueenLifeTime, randomQueenLifeTime, 0,malloc(sizeof(int) * 2));
                    addBee(hive, queen);
                }
                actualBeeElement = actualBeeElement->nextElement;
                continue;
            }
            triggerDisease(bee, frameRate);
            tryEvolve(bee);

            if (bee->role == QUEEN && canQueenStartLayingEggs(bee)) {
                handleBeeFeeding(bee, hive);
                handleEggsLaying(bee, hive);
            }
        }
        if (bee->role != EGG && bee->role != LARVA && bee->role != NYMPH && bee->role != WORKER) {
            moveBee(world, hive, bee, frameRate);
        }
        actualBeeElement = actualBeeElement->nextElement;
    }
}

/**
 *
 * @param world
 * @param renderer
 * @param textures
 *
 * Method to show metrics on screen
 */
void showMetrics(World *world, SDL_Renderer *renderer, SDL_Texture **textures) {
    // loop through all locations and draw them in yellow

    ListElement *actualHiveElement = world->hives->firstElement;
    Hive *hive1 = actualHiveElement->value;


    SDL_Rect rect = {world->width - 180, 40, 150, 22};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderCopy(renderer, textures[5], NULL, &rect);

    int value = (hive1->nectar * 147) / MAX_NECTAR_CAPACITY;

    SDL_Rect rect2 = {world->width - 177, 42, value, 18};
    SDL_SetRenderDrawColor(renderer, 223, 117, 5, 255);
    SDL_RenderFillRect(renderer, &rect2);
}

/**
 *
 * @param world
 * @param renderer
 * @param textures
 *
 * Method to show hives on screen
 */
void showHives(World *world, SDL_Renderer *renderer, SDL_Texture **textures) {
    // loop through all locations and draw them in yellow

    ListElement *actualHiveElement = world->hives->firstElement;

    while (actualHiveElement != NULL) {
        Hive *hive = actualHiveElement->value;
        ListElement *actualLocationElement = hive->locations->firstElement;
        Location *location = hive->locations->firstElement->value;

        SDL_Rect rect = {location->x, location->y, hive->height, hive->width};
        SDL_RenderCopy(renderer, textures[1], NULL, &rect);

        showBees(hive, renderer, textures); // show bees
        actualHiveElement = actualHiveElement->nextElement;
    }
}

/**
 *
 * @param hive
 * @param renderer
 * @param textures
 *
 * Method to show bees on screen
 */
void showBees(Hive *hive, SDL_Renderer *renderer, SDL_Texture **textures) {
    ListElement *actualElement = hive->bees->firstElement;
    Bee *bee;

    while (actualElement != NULL) {
        bee = actualElement->value;

        if (bee->dead || bee->inHive) {
            actualElement = actualElement->nextElement;
            continue;
        }
        // create a rect for each bee with black color

        if (bee->role == QUEEN) {
            SDL_Rect rect = {bee->location->x - 16, bee->location->y - 16, 30, 27};
            SDL_RenderCopy(renderer, textures[2], NULL, &rect);
        } else if (bee->role == FORAGER) {
            SDL_Rect rect = {bee->location->x, bee->location->y, 10, 9};
            SDL_RenderCopy(renderer, textures[2], NULL, &rect);
        } else if (bee->role == DRONE && canQueenGoCoupling(hive->queen)) {
            SDL_Rect rect = {bee->location->x, bee->location->y, 15, 14};
            SDL_RenderCopy(renderer, textures[2], NULL, &rect);
        }

        /*SDL_Rect rect = {bee->location->x, bee->location->y, queen ? 5 : 3, queen ? 5 : 3};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);*/

        actualElement = actualElement->nextElement;
    }
}

/**
 *
 * @param world
 * @param renderer
 * @param textures
 *
 * Method to show flowers on screen
 */
void showFlowers(World *world, SDL_Renderer *renderer, SDL_Texture **textures) {
    ListElement *actualElement = world->flowers->firstElement;
    Flower *flower;

    while (actualElement != NULL) {
        flower = actualElement->value;

        if (flower->dead) {
            actualElement = actualElement->nextElement;
            continue;
        }

        // create a rect for each flower with red color
        SDL_Rect rect = {flower->location->x - 30, flower->location->y, 60, 38};
        SDL_RenderCopy(renderer, textures[3], NULL, &rect);

        actualElement = actualElement->nextElement;
    }
}

/**
 *
 * @param world
 * @param renderer
 * @param textures
 *
 * Method to show lakes on screen
 */
void showLakes(World *world, SDL_Renderer *renderer, SDL_Texture **textures) {
    ListElement *actualElement = world->lakes->firstElement;
    Lake *lake;

    while (actualElement != NULL) {
        lake = actualElement->value;
        Location *location = lake->locations->firstElement->value;
        // create a rect for each lake with red color
        SDL_Rect rect = {location->x, location->y, lake->height, lake->width};
        SDL_RenderCopy(renderer, textures[4], NULL, &rect);

        actualElement = actualElement->nextElement;
    }
}

/**
 *
 * @param renderer
 * @param texture
 * @param path
 * @return Texture from path
 */
SDL_Texture *loadImage(SDL_Renderer *renderer, SDL_Texture *texture, char *path) {
    SDL_Surface *surface = SDL_LoadBMP(path);

    if (surface == NULL) {
        printf("Error SDL_LoadBMP : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    Uint32 colorkey = SDL_MapRGB(surface->format, 255, 255, 255);
    SDL_SetColorKey(surface, SDL_TRUE, colorkey);

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == NULL) {
        printf("Error SDL_CreateTextureFromSurface : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return texture;
}

World *createWorld() {
    World *world = malloc(sizeof(World));
    world->time = 0;
    world->width = 0;
    world->height = 0;

    printf("-- World created --\n");
    printf("> Time: %ld\n", world->time);
    printf("-------------------\n");

    return world;
}

void addHive(World *world, Hive *hive) {
    if (world->hives == NULL) {
        world->hives = createList();
    }
    ListElement *listElement = malloc(sizeof(ListElement));
    listElement->value = hive;
    listElement->nextElement = NULL;
    insertElement(world->hives, listElement);
}

void addFlower(World *world, Flower *flower) {
    if (world->flowers == NULL) {
        world->flowers = createList();
    }
    ListElement *listElement = malloc(sizeof(ListElement));
    listElement->value = flower;
    listElement->nextElement = NULL;
    insertElement(world->flowers, listElement);
}

void addLake(World *world, Lake *lake) {
    if (world->lakes == NULL) {
        world->lakes = createList();
    }
    ListElement *listElement = malloc(sizeof(ListElement));
    listElement->value = lake;
    listElement->nextElement = NULL;
    insertElement(world->lakes, listElement);
}