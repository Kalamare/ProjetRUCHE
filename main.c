#include <SDL2/SDL.h>
#include "world.h"

World *globalWorld;
const unsigned int FRAME_RATE = 300;

void tickWorld() {
    globalWorld->time++;
    tickHives(globalWorld, FRAME_RATE);
    tickFlowers(globalWorld, FRAME_RATE);
}

void openGameWindow(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture **textures) {

    bool close;

    SDL_Rect rect = {0, 0, globalWorld->width, globalWorld->height};
    SDL_RenderCopy(renderer, textures[0], NULL, &rect);
    SDL_RenderPresent(renderer);

    while (!close) {
        SDL_Event event;

        // Events management
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                close = true;
                printf("Simulation ended\n");
                break;
            }
        }

        tickWorld();

        // clears the screen
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, textures[0], NULL, &rect);
        showLakes(globalWorld, renderer, textures); // show lakes
        showFlowers(globalWorld, renderer, textures); // show flowers
        showHives(globalWorld, renderer, textures); // show hive (locations)

        // triggers the double buffers
        // for multiple rendering
        SDL_RenderPresent(renderer);

        // calculates to FRAME_RATE fps
        SDL_Delay(1000 / FRAME_RATE);
    }

    // destroy texture
    for (int i = 0; i < 5; ++i) {
        SDL_DestroyTexture(textures[i]);
    }

    // destroy renderer
    SDL_DestroyRenderer(renderer);

    // destroy window
    SDL_DestroyWindow(window);

    // close SDL
    SDL_Quit();
}


void startSimulation(World *world) {
    printf("Simulation started\n");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        goto Quit;
    }
    /*SDL_Rect rect;
    SDL_GetWindowRect(0, &rect);

    world->height = rect.x;
    world->width = rect.y;

    printf("Screen resolution: %dx%d\n", world->width, world->height);*/

    SDL_Window *window = SDL_CreateWindow("HIVE SIMULATOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("Error creating window: %s\n", SDL_GetError());
        goto Quit;
    }

    printf("Screen resolution: %dx%d\n", world->width, world->height);

    // creates a renderer to render our images
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

        if (renderer == NULL) {
            printf("Error initializing SDL renderer: %s\n", SDL_GetError());
            goto Quit;
        }
    }
    SDL_Texture **textures = malloc(sizeof(SDL_Texture *) * 5);
    SDL_Texture *backGroundTexture = loadImage(renderer, backGroundTexture, "assets/background.bmp");
    SDL_Texture *hiveTexture = loadImage(renderer, hiveTexture, "assets/hive.bmp");
    SDL_Texture *beeTexture = loadImage(renderer, beeTexture, "assets/mid-bee.bmp");
    SDL_Texture *flowerTexture = loadImage(renderer, flowerTexture, "assets/flower.bmp");
    SDL_Texture *lakeTexture = loadImage(renderer, lakeTexture, "assets/lake.bmp");

    textures[0] = backGroundTexture;
    textures[1] = hiveTexture;
    textures[2] = beeTexture;
    textures[3] = flowerTexture;
    textures[4] = lakeTexture;

    openGameWindow(window, renderer, textures);

    Quit:
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != NULL) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

void freeBees() {
    ListElement *actualHiveElement = globalWorld->hives->firstElement;

    while (actualHiveElement != NULL) {
        Hive *hive = actualHiveElement->value;
        ListElement *actualElement = hive->bees->firstElement;
        Bee *bee;

        while (actualElement != NULL) {
            bee = actualElement->value;
            free(bee->nextLocations);
            free(bee->location);
            actualElement = actualElement->nextElement;
        }
        actualHiveElement = actualHiveElement->nextElement;
    }
}

void freeFlowers() {
    ListElement *actualElement = globalWorld->flowers->firstElement;
    Flower *flower;

    while (actualElement != NULL) {
        flower = actualElement->value;
        free(flower->location);
        actualElement = actualElement->nextElement;
    }
}

enum Role chooseRole() {
    int random = rand() % 100;

    if(random <= 40){
        return WORKER;
    }
    if(random <= 80){
        return DRONE;
    }
    return FORAGER;
}

void createBees(Hive *hive) {
    int beesCount = INITIAL_BEES + (rand() % (INITIAL_BEES / 3));
    int randomQueenLifeTime = QUEEN_MAX_LIFE_TIME - (rand() % (QUEEN_MAX_LIFE_TIME / 4));

    List *hiveLocations = hive->locations;
    Bee *queen = createBee(++hive->beesCount, getLocationFromList(hiveLocations, rand() % hiveLocations->size), QUEEN, QUEEN_FOOD, randomQueenLifeTime, randomQueenLifeTime, 0, malloc(sizeof(int) * 2));

    for (int i = 0; i < beesCount; i++) {
        Location *location = getLocationFromList(hiveLocations, rand() % hiveLocations->size);


        int randomWorkerLifeTime = WORKER_MAX_LIFE_TIME - (rand() % (WORKER_MAX_LIFE_TIME / 4));
        Bee *bee = createBee(++hive->beesCount, location, chooseRole(), DEFAULT_FOOD, randomWorkerLifeTime, randomWorkerLifeTime, 0,malloc(sizeof(int) * 2));
      //  printf("Bee role: %d\n", bee->role);
        addBee(hive, bee);
    }

    addBee(hive, queen);
}



int main() {
    srand(time(NULL));

    World *world = createWorld();
    Hive *hive = createHive(1, 1920 /2 , 1080 / 2, 32, 32);
    Lake *lake = createLake(1, 128, 128, 80, 80);

    world->height = SDL_GetWindowSurface(window)->w;
    world->width = SDL_GetWindowSurface(window)->h;

    addHive(world, hive);
    addLake(world, lake);

    createBees(hive);
    createFlowers(world, INITIAL_FLOWERS + (rand() % (INITIAL_FLOWERS /4)));

    globalWorld = world;
    startSimulation(world);

    freeBees();
    freeFlowers();

    freeList(world->flowers);
    freeList(hive->bees);
    freeList(hive->locations);

    free(world);
    free(hive);
    free(lake);

    return 0;
}


