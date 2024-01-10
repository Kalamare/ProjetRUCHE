#include "world.h"
#include "randomwalker.h"

bool isOutOfWorld(Location *location, World *world) {
    return location == NULL || (location->x < 0 || location->x >= world->width || location->y < 0 || location->y >= world->height);
}

bool hasNextLocations(Bee *bee) {
    int *nextLocations = bee->nextLocations;
    return nextLocations != NULL && (nextLocations[0] != 0 || nextLocations[1] != 0);
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

void goToHive(Hive *hive, Bee *bee, bool isQueen) {
    List *hiveLocations = hive->locations;
    Location *randomHiveLocation = getLocationFromList(hiveLocations, rand() % hiveLocations->size);
    randomPath(bee, randomHiveLocation);

    if (isQueen) {
        bee->mustGoToHive = true;
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
    if (bee->role == QUEEN) {
        if (!canQueenGoCoupling(bee)) {
            return;
        }
        if (isSecondTick(world, frameRate) && !canQueenStartLayingEggs(bee)) {
            Bee *nearBee = checkNearbyBees(hive->bees, bee, DRONE);

            if (nearBee != NULL && !isGoingToFlower(nearBee) && ++bee->couplingTimes >= COUPLING_TIMES_BEFORE_LAY_EGGS) { // When queen is near another bee and the other bee is not going to flower
                nearBee->dead = true; // KILL THE MALE WHEN COUPLING
                goToHive(hive, bee, true);
                //printf("Bee %d coupling times: %d\n", bee->id, bee->couplingTimes);
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
                bee->flowerToGo = NULL;
            }
        }
        return;
    }

    if (hasNextLocations(bee)) {
        Location *nextLocation = getNextLocation(bee);
        bee->location = nextLocation; // Set new location to bee
        //printf("Bee %d moved to %d, %d\n", bee->id, nextLocation->x, nextLocation->y);
        triggerFoodLoss(hive, bee);

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
                bee->inHive = true;
                bee->idleTime = 5; // Idling for 5 seconds
            }

            handleNectarTransfer(bee, hive);
            handleBeeFeeding(bee, hive);
        }
        //  printf("Bee already has next locations, moving to %d, %d\n", bee->location->x, bee->location->y);
        return;
    }

    if (bee->mustGoToHive || isGoingToFlower(bee)) {
        if (bee->role == QUEEN){
            bee->mustGoToHive = false;
        }
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
        bee->flowerToGo = flower;
    } else {
        determineNextLocations(world, hive, bee);
    }
}

void createFlowers(World *world, int flowersCount) {
    int attempts = 0;
    int actualFlowersCount = world->flowers == NULL ? 0 : world->flowers->size;

    for (int i = 0; i < flowersCount; i++) {

        Location *location = createLocation(rand() % world->width, rand() % world->height);

        if (attempts > 1000) {
            //printf("Too many attempts, breaking\n");
            break;
        }

        if (isFlowerAround(world, location, DISTANCE_BETWEEN_FLOWERS) || isHiveAround(world, location, DISTANCE_FLOWER_HIVE) || isLakeAround(world, location, DISTANCE_FLOWER_LAKE)) {
            //printf("Flower around with location %d, %d\n", location->x, location->y);
            free(location);
            i--;
            attempts++;
            continue;
        }
        int randomCapacity = MIN_FLOWER_CAPACITY + (rand() % MIN_FLOWER_CAPACITY);
        int randomHoneyGiven = MIN_FLOWER_NECTAR_GIVEN + (rand() % MIN_FLOWER_NECTAR_GIVEN);

        addFlower(world,createFlower(actualFlowersCount + i + 1, location, randomCapacity, randomCapacity, randomHoneyGiven));
        //printf("Flower created with location %d, %d\n", location->x, location->y);
    }
}

void tickFlowers(World *world, int frameRate) {
    if (!isSecondTick(world, frameRate) || world->flowers == NULL || world->flowers->size >= INITIAL_FLOWERS) {
        return;
    }
    bool mustCreateFlower = rand() % 5 == 0;

    if (mustCreateFlower) {
        createFlowers(world, 1);
    }
}

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

                    Bee *queen = createBee(++hive->beesCount,
                                           getLocationFromList(hive->locations, rand() % hive->locations->size), QUEEN,
                                           QUEEN_FOOD, randomQueenLifeTime, randomQueenLifeTime, 0,
                                           malloc(sizeof(int) * 2));
                    addBee(hive, queen);
                }
                actualBeeElement = actualBeeElement->nextElement;
                continue;
            }
            triggerDisease(bee);
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
            SDL_Rect rect = {bee->location->x, bee->location->y, 30, 27};
            SDL_RenderCopy(renderer, textures[2], NULL, &rect);
        } else if (bee->role == FORAGER) {
            SDL_Rect rect = {bee->location->x, bee->location->y, 10, 9};
            SDL_RenderCopy(renderer, textures[2], NULL, &rect);
        } else if (bee->role == DRONE) {
            SDL_Rect rect = {bee->location->x, bee->location->y, 10, 9};
            SDL_RenderCopy(renderer, textures[2], NULL, &rect);
        }

        /*SDL_Rect rect = {bee->location->x, bee->location->y, queen ? 5 : 3, queen ? 5 : 3};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);*/

        actualElement = actualElement->nextElement;
    }
}

void showFlowers(World *world, SDL_Renderer *renderer, SDL_Texture **textures) {
    ListElement *actualElement = world->flowers->firstElement;
    Flower *flower;

    while (actualElement != NULL) {
        flower = actualElement->value;
        // create a rect for each flower with red color
        SDL_Rect rect = {flower->location->x - 30, flower->location->y, 60, 38};
        SDL_RenderCopy(renderer, textures[3], NULL, &rect);

        actualElement = actualElement->nextElement;
    }
}

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