#include "world.h"

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

Location *createLocation(int x, int y) {
    Location *location = malloc(sizeof(Location));
    location->x = x;
    location->y = y;

    /*printf("-- Location created --\n");
    printf("> X: %d\n", location->x);
    printf("> Y: %d\n", location->y);
    printf("-------------------\n");*/

    return location;
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

void addFlower(World *world, Flower *flower) {
    if (world->flowers == NULL) {
        world->flowers = createList();
    }
    ListElement *listElement = malloc(sizeof(ListElement));
    listElement->value = flower;
    listElement->nextElement = NULL;
    insertElement(world->flowers, listElement);
}

bool isOutOfWorld(Location *location, World *world) {
    return location == NULL || (location->x < 0 || location->x >= world->size || location->y < 0 || location->y >= world->size);
}

double distanceSquared(Location *location1, Location *location2) {
    return pow(location1->x - location2->x, 2) + pow(location1->y - location2->y, 2);
}

double distance(Location *location1, Location *location2) {
    return sqrt(distanceSquared(location1, location2));
}