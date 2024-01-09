#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "hive.h"
#include "list.h"
#include "stack.h"

#ifndef WORLD_H
#define WORLD_H

typedef struct World World;
typedef struct Flower Flower;

struct World {
    unsigned int id;
    unsigned int size;
    long time;
    List *hives;
    List *flowers;
};

struct Flower {
    unsigned int initialCapacity;
    unsigned int capacity;
    unsigned int honeyGiven;
    Location *location;
};

World *createWorld(int id, int size);

Flower *createFlower(Location *location, unsigned int initialCapacity, unsigned int capacity, unsigned int honeyGiven);

bool isOutOfWorld(Location *location, World *world);

void tickHives(World *world, unsigned int frameRate);

void tickBees(World *world, Hive *hive, unsigned int frameRate);

void moveBee(World *world, Hive *hive, Bee *bee, unsigned int frameRate);

void showHives(World *world, SDL_Renderer *renderer, SDL_Texture **textures);

void showBees(Hive *hive, SDL_Renderer *renderer, SDL_Texture **textures);

void showFlowers(World *world, SDL_Renderer *renderer, SDL_Texture **textures);

SDL_Texture *loadImage(SDL_Renderer *renderer, SDL_Texture *texture, char *path);

void addHive(World *world, Hive *hive);

void addFlower(World *world, Flower *flower);

void tickWorld();

#endif