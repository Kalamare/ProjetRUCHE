#include "world.h"
#include "randomwalker.h"


bool isOutOfWorld(Location *location, World *world) {
    return location == NULL ||
           (location->x < 0 || location->x >= world->size || location->y < 0 || location->y >= world->size);
}

bool hasNextLocations(Bee *bee) {
    int *nextLocations = bee->nextLocations;
    return nextLocations != NULL && (nextLocations[0] != 0 || nextLocations[1] != 0);
}

void moveBee(World *world, Hive *hive, Bee *bee, unsigned int frameRate) {
    if (isIdling(bee)) {
        if (world->time % frameRate != 0) {
            return;
        }
        bee->idleTime--;

        if (bee->idleTime > 0) {
            return;
        }
        if (bee->mustGoToHive) {
            bee->inHive = false;
            bee->mustGoToHive = false;
        } else if (bee->goingToFlower) {
            bee->goingToFlower = false;
            bee->mustGoToHive = true;
        }
        return;
    }

    if (hasNextLocations(bee)) {
        Location *nextLocation = getNextLocation(bee);
        bee->location = nextLocation; // Set new location to bee
        //printf("Bee %d moved to %d, %d\n", bee->id, nextLocation->x, nextLocation->y);

        if (hasNextLocations(bee)) {
            return;
        }

        if (bee->goingToFlower) {
            bee->idleTime = 3; // Idling for 3 seconds

            List *hiveLocations = hive->locations;
            Location *randomHiveLocation = getLocationFromList(hiveLocations, rand() % hiveLocations->size);
            randomPath(world, bee, randomHiveLocation);
            return;
        }
        if (bee->mustGoToHive) {
            bee->inHive = true;
            bee->idleTime = 5; // Idling for 5 seconds
        }
        //  printf("Bee already has next locations, moving to %d, %d\n", bee->location->x, bee->location->y);
        return;
    }

    if (bee->mustGoToHive || bee->goingToFlower) {
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

void tickHives(World *world, unsigned int frameRate) {
    ListElement *actualHiveElement = world->hives->firstElement;
    Hive *hive;

    while (actualHiveElement != NULL) {
        hive = actualHiveElement->value;
        hive->time++;
        tickBees(world, hive, frameRate);

        actualHiveElement = actualHiveElement->nextElement;
    }
}

void tickBees(World *world, Hive *hive, unsigned int frameRate) {
    ListElement *actualBeeElement = hive->bees->firstElement;
    Bee *bee;

    while (actualBeeElement != NULL) {
        bee = actualBeeElement->value;

        if (bee->dead) {
            actualBeeElement = actualBeeElement->nextElement;
            continue;
        }

        if (world->time % frameRate == 0) {
            bee->health -= bee->disease ? 5 : 1;
            // printf("Bee %d life time: %d\n", bee->id, bee->lifeTime);

            if (bee->health <= 0) {
                bee->dead = true;
                printf("Bee %d died\n", bee->id);
                actualBeeElement = actualBeeElement->nextElement;
                continue;
            }
            triggerDisease(bee);
            tryEvolve(bee);
        }
        moveBee(world, hive, bee, frameRate);
        actualBeeElement = actualBeeElement->nextElement;
    }
}

void showHives(World *world, SDL_Renderer *renderer, SDL_Texture **textures) {
    // loop through all locations and draw them in yellow

    ListElement *actualHiveElement = world->hives->firstElement;

    while (actualHiveElement != NULL) {
        Hive *hive = actualHiveElement->value;
        ListElement *actualLocationElement = hive->locations->firstElement;
        Location *location;

        while (actualLocationElement != NULL) {
            location = actualLocationElement->value;
            SDL_Rect rect = {location->x, location->y, hive->height, hive->width};
            SDL_RenderCopy(renderer, textures[1], NULL, &rect);

            /* SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
             SDL_RenderFillRect(renderer, &rect);*/

            actualLocationElement = actualLocationElement->nextElement;
        }

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
            SDL_Rect rect = {bee->location->x, bee->location->y, 10, 9};
            SDL_RenderCopy(renderer, textures[2], NULL, &rect);
        } else if (bee->role == WORKER) {
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
        SDL_Rect rect = {flower->location->x, flower->location->y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

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

World *createWorld(int id, int size) {
    World *world = malloc(sizeof(World));
    world->id = id;
    world->size = size;
    world->time = 0;

    printf("-- World created --\n");
    printf("> Id: %d\n", world->id);
    printf("> Size: %d\n", world->size);
    printf("> Time: %ld\n", world->time);
    printf("-------------------\n");

    return world;
}

Flower *createFlower(Location *location, unsigned int initialCapacity, unsigned int capacity, unsigned int honeyGiven) {
    Flower *flower = malloc(sizeof(Flower));
    flower->location = location;
    flower->initialCapacity = initialCapacity;
    flower->capacity = capacity;
    flower->honeyGiven = honeyGiven;

    printf("-- Flower created --\n");
    printf("> Location: %d, %d\n", location->x, location->y);
    printf("> Initial capacity: %d\n", flower->initialCapacity);
    printf("> Capacity: %d\n", flower->capacity);
    printf("> Honey given: %d\n", flower->honeyGiven);
    printf("-------------------\n");

    return flower;
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