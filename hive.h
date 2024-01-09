#include <SDL2/SDL.h>
#include <stdbool.h>
#include "list.h"
#include "location.h"

#ifndef HIVE_H
#define HIVE_H

#define WORLD_SIZE 800

#define EGG_LIFE_TIME 30
#define LARVA_LIFE_TIME 60
#define NYMPH_LIFE_TIME 100

#define QUEEN_MAX_LIFE_TIME 12300
#define WORKER_MAX_LIFE_TIME 432

#define DAYS_BEFORE_GO_COUPLING 10
#define COUPLING_TIMES_BEFORE_LAY_EGGS 4
#define MAX_EGGS_PER_DAY 15
#define QUEEN_MAX_RANDOM_WALK_RANGE 100
#define QUEEN_COUPLING_RANGE 20

#define DEFAULT_FOOD 5
#define QUEEN_FOOD 200

#define INITIAL_BEES 35
#define INITIAL_FLOWERS 30
#define DISTANCE_BETWEEN_FLOWERS 45
#define DISTANCE_FLOWER_HIVE 150

#define FLOWERS_VIEW_DISTANCE 30
#define MIN_FLOWER_CAPACITY 30
#define MIN_FLOWER_NECTAR_GIVEN 5

#define FEED_TRESHOLD 0.25
#define FEED_AMOUNT_RATIO 0.5
#define MAX_NECTAR_CAPACITY 50000
#define BEES_VIEW_DISTANCE 10

typedef struct Flower Flower;
typedef struct Hive Hive;
typedef struct Bee Bee;

enum Role {
    QUEEN,
    WORKER,
    DRONE,
    EGG,
    LARVA,
    NYMPH
};

struct Bee {
    int id;
    int health;
    int lifeTime;
    int nectar;
    int water;
    int *nextLocations;
    int idleTime;
    int food;
    int couplingTimes;
    bool disease;
    bool dead;
    bool mustGoToHive;
    bool inHive;
    Flower *flowerToGo;
    Location *location;
    enum Role role;
};

struct Hive {
    int id;
    int height;
    int width;
    int beesCount;
    int nectar;
    int water;
    long time;
    List *bees;
    List *locations;
};

struct Flower {
    int id;
    int initialCapacity;
    int capacity;
    int nectarGiven;
    Location *location;
};

Hive *createHive(int id, int x, int y, int height, int width);

Bee *createBee(int id, Location *location, enum Role role, int food, int health, int lifeTime, int idleTime, int *nextLocations);

Flower *createFlower(int id, Location *location,  int initialCapacity,  int capacity,  int honeyGiven);

Location *getNextLocation(Bee *bee);

Location *getLocationFromList(List *locations, int position);

void addBee(Hive *hive, Bee *bee);

void addLocation(Hive *hive, Location *location);

void tryEvolve(Bee *bee);

void triggerDisease(Bee *bee);

void triggerHarvest(List *flowers, Bee *bee);

void triggerFoodLoss(Hive* hive, Bee *bee);

void handleNectarTransfer(Bee *bee, Hive *hive);

void handleBeeFeeding(Bee *bee, Hive *hive);

void handleEggsLaying(Bee *bee, Hive *hive);

bool isIdling(Bee *bee);

bool isGoingToFlower(Bee *bee);

bool canQueenGoCoupling(Bee* queen);

bool canQueenStartLayingEggs(Bee* queen);

#endif