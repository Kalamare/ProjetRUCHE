#include <SDL2/SDL.h>
#include <stdbool.h>
#include "list.h"
#include "location.h"

#ifndef HIVE_H
#define HIVE_H

#define EGG_LIFE_TIME 20
#define LARVA_LIFE_TIME 40
#define NYMPH_LIFE_TIME 60

#define QUEEN_MAX_LIFE_TIME 12300
#define WORKER_MAX_LIFE_TIME 532

#define DAYS_BEFORE_GO_COUPLING 8
#define COUPLING_TIMES_BEFORE_LAY_EGGS 8
#define MAX_EGGS_PER_DAY 30
#define QUEEN_MAX_RANDOM_WALK_RANGE 100
#define DRONE_MAX_RANDOM_WALK_RANGE 100
#define QUEEN_COUPLING_RANGE 20

#define DEFAULT_FOOD 10
#define QUEEN_FOOD 200

#define INITIAL_BEES 120
#define INITIAL_FLOWERS 30
#define INITIAL_LAKES 5
#define DISTANCE_LAKE_HIVE 150
#define DISTANCE_LAKE_BORDER 50
#define DISTANCE_BETWEEN_LAKES 120
#define DISTANCE_FLOWER_BORDER 35
#define DISTANCE_BETWEEN_FLOWERS 45
#define DISTANCE_FLOWER_HIVE 150
#define DISTANCE_FLOWER_LAKE 30

#define FLOWERS_VIEW_DISTANCE 40
#define MIN_FLOWER_CAPACITY 30
#define MIN_FLOWER_NECTAR_GIVEN 5

#define FEED_TRESHOLD 0.25
#define FEED_AMOUNT_RATIO 0.5
#define MAX_NECTAR_CAPACITY 50000
#define BEES_VIEW_DISTANCE 20
#define SEARCH_LOCATION_RADIUS 200

typedef struct Flower Flower;
typedef struct Hive Hive;
typedef struct Bee Bee;
typedef struct Lake Lake;

enum Role {
    QUEEN,
    WORKER,
    FORAGER,
    DRONE,
    EGG,
    LARVA,
    NYMPH,
    HORNET,
    PRAYING_MANTIS
};

struct Bee {
    int id;
    int health;
    int lifeTime;
    int nectar;
    int harvestNectar;
    int water;
    int *nextLocations;
    int idleTime;
    int food;
    int couplingTimes;
    bool disease;
    bool dead;
    bool mustGoToHive;
    bool inHive;
    int flowerToGo;
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
    Bee* queen;
};

struct Flower {
    int id;
    int initialCapacity;
    int capacity;
    int nectarGiven;
    bool dead;
    Location *location;
};

struct Lake {
    int id;
    int height;
    int width;
    List *locations;
};

Hive *createHive(int id, int x, int y, int height, int width);

Bee *createBee(int id, Location *location, enum Role role, int food, int health, int lifeTime, int idleTime,int *nextLocations);

Lake *createLake(int id, int x, int y, int height, int width);

Flower *createFlower(int id, Location *location, int initialCapacity, int capacity, int honeyGiven);

Location *getNextLocation(Bee *bee);

Location *getLocationFromList(List *locations, int position);

void addBee(Hive *hive, Bee *bee);

void addLocation(Hive *hive, Location *location);

void addLakeLocation(Lake *lake, Location *location);

void tryEvolve(Bee *bee);

void triggerDisease(Bee *bee, int frameRate);

int triggerHarvest(List *flowers, Bee *bee);

void triggerFoodLoss(Hive *hive, Bee *bee, int frameRate);

void handleNectarTransfer(Bee *bee, Hive *hive);

void handleBeeFeeding(Bee *bee, Hive *hive);

void handleEggsLaying(Bee *bee, Hive *hive);

enum Role chooseRole();

bool isIdling(Bee *bee);

bool isGoingToFlower(Bee *bee);

bool canQueenGoCoupling(Bee *queen);

bool canQueenStartLayingEggs(Bee *queen);

#endif