#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include "hive.h"

World *globalWorld;
Hive *globalHive;
const unsigned int FRAME_RATE = 20;

void tickWorld() {
    globalWorld->time++;
    tickBees(globalWorld, globalHive, FRAME_RATE);
}

SDL_Texture *loadImage(SDL_Renderer *renderer, SDL_Texture *texture, char *path) {
    SDL_Surface *surface = SDL_LoadBMP(path);

    if (surface == NULL) {
        printf("Error SDL_LoadBMP : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == NULL) {
        printf("Error SDL_CreateTextureFromSurface : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return texture;
}

void showBees(SDL_Renderer *renderer, SDL_Texture *texture) {
    ListElement *actualElement = globalHive->bees->firstElement;
    Bee *bee;

    while (actualElement != NULL) {
        bee = actualElement->value;

        if (bee->dead) {
            actualElement = actualElement->nextElement;
            continue;
        }
        // create a rect for each bee with black color
        SDL_Rect rect = {bee->location->x, bee->location->y, 10, 10};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        actualElement = actualElement->nextElement;
    }
}

void showFlowers(SDL_Renderer *renderer, SDL_Texture *texture) {
    ListElement *actualElement = globalWorld->flowers->firstElement;
    Flower *flower;

    while (actualElement != NULL) {
        flower = actualElement->value;

        // create a rect for each flower with yellow color
        SDL_Rect rect = {flower->location->x, flower->location->y, 10, 10};
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        actualElement = actualElement->nextElement;
    }
}

void openGameWindow(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture) {

    bool close;
    SDL_Rect rect = {0, 0, 800, 800};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderPresent(renderer);

    while (!close) {
        SDL_Event event;

        tickWorld();

        // Events management
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                close = true;
                printf("Simulation ended\n");
                break;
            }
        }

        // clears the screen
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture, NULL, &rect);
        showBees(renderer, texture); // show bees
        showFlowers(renderer, texture); // show flowers

        // triggers the double buffers
        // for multiple rendering
        SDL_RenderPresent(renderer);

        // calculates to 5 fps
        SDL_Delay(1000 / FRAME_RATE);
    }

    // destroy texture
    SDL_DestroyTexture(texture);

    // destroy renderer
    SDL_DestroyRenderer(renderer);

    // destroy window
    SDL_DestroyWindow(window);

    // close SDL
    SDL_Quit();
}


void startSimulation(World *world) {
    int statut = EXIT_FAILURE;
    printf("Simulation started\n");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        goto Quit;
    }
    SDL_Window *window = SDL_CreateWindow("HIVE SIMULATOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800,
                                          0);
    if (window == NULL) {
        printf("Error creating window: %s\n", SDL_GetError());
        goto Quit;
    }
    // creates a renderer to render our images
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("Error initializing SDL renderer: %s\n", SDL_GetError());
        goto Quit;
    }
    SDL_Texture *texture = loadImage(renderer, texture, "assets/background.bmp");

    statut = EXIT_SUCCESS;
    openGameWindow(window, renderer, texture);

    Quit:
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != NULL) {
        SDL_DestroyWindow(window);
    }
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }
    SDL_Quit();
    exit(statut);
}

void freeBees() {
    ListElement *actualElement = globalHive->bees->firstElement;
    Bee *bee;

    while (actualElement != NULL) {
        bee = actualElement->value;
        free(bee->nextLocations);
        free(bee->location);
        actualElement = actualElement->nextElement;
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

int main() {
    srand(time(NULL));

    World *world = createWorld(1, 800);
    Hive *hive = createHive(1, 10, 10);
    Location *location = createLocation(0, 0);

    Bee *queen = createBee(hive->beesCount++, 80, 0, location, QUEEN, false, false, 100, 0, malloc(sizeof(int) * 2), false, false);
    Bee *worker = createBee(hive->beesCount++, 60, 0, location, WORKER, false, false, 100, 0, malloc(sizeof(int) * 2), false, false);
    Bee *drone = createBee(hive->beesCount++, 40, 0, location, DRONE, false, false, 100, 0, malloc(sizeof(int) * 2), false, false);

    addBee(hive, queen);
    addBee(hive, worker);
    addBee(hive, drone);

    addLocation(hive, createLocation(10, 10));

    addFlower(world, createFlower(createLocation(24, 58), 100, 100, 5));
    addFlower(world, createFlower(createLocation(239, 444), 100, 100, 5));
    addFlower(world, createFlower(createLocation(223, 39), 100, 100, 5));


    globalHive = hive;
    globalWorld = world;
    startSimulation(world);

    freeBees();
    freeFlowers();

    freeList(world->flowers);
    freeList(hive->bees);
    freeList(hive->locations);

    free(location);
    free(world);
    free(hive);

    return 0;
}


