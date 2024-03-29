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

struct World {
    unsigned int id;
    int height;
    int width;
    long time;
    List *hives;
    List *flowers;
    List* lakes;
};


World *createWorld();

void createFlowers(World *world, int flowersCount);

void createLakes(World *world, int lakesCount);

void createBees(Hive *hive, int beesCount);

void tickHives(World *world, int frameRate);

void tickBees(World *world, Hive *hive, int frameRate);

void tickFlowers(World *world, int frameRate);

void moveBee(World *world, Hive *hive, Bee *bee, int frameRate);

void showHives(World *world, SDL_Renderer *renderer, SDL_Texture **textures);

void showBees(Hive *hive, SDL_Renderer *renderer, SDL_Texture **textures);

void showFlowers(World *world, SDL_Renderer *renderer, SDL_Texture **textures);

void showLakes(World *world, SDL_Renderer *renderer, SDL_Texture **textures);

void showMetrics(World *world, SDL_Renderer *renderer, SDL_Texture **textures);

SDL_Texture *loadImage(SDL_Renderer *renderer, SDL_Texture *texture, char *path);

void addHive(World *world, Hive *hive);

void addFlower(World *world, Flower *flower);

void addLake(World *world, Lake *lake);

void tickWorld();

bool isOutOfWorld(Location *location, World *world);

bool isFlowerAround(World *world, Location *location, int radius);

bool isLakeAround(World *world, Location *location, int radius);

bool isLakeAround(World *world, Location *location, int radius);

bool isHiveAround(World *world, Location *location, int radius);

bool isBorderAround(World *world, Location *location, int radius);

#endif