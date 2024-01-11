#include <SDL2/SDL.h>
#include "world.h"

World *globalWorld;
const unsigned int FRAME_RATE = 60;

void tickWorld() {
    globalWorld->time++;
    tickHives(globalWorld, FRAME_RATE);
    tickFlowers(globalWorld, FRAME_RATE);
}

/**
 * Method to open the game window
 *
 * @param window
 * @param renderer
 * @param textures
 *
 */
void openGameWindow(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture **textures) {

    bool close;
    SDL_Rect rect = {0, 0, globalWorld->width, globalWorld->height};

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
        showMetrics(globalWorld, renderer, textures); // show metrics


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

/**
 * Main method to start the simulation
 *
 * @param world
 */
void startSimulation(World *world) {
    printf("Simulation started\n");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        goto Quit;
    }
    SDL_WINDOWPOS_CENTERED_DISPLAY(10);


    SDL_Window *window = SDL_CreateWindow("HIVE SIMULATOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, world->width, world->height, 0);
    if (window == NULL) {
        printf("Error creating window: %s\n", SDL_GetError());
        goto Quit;
    }

    // creates a renderer to render our images
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

        if (renderer == NULL) {
            printf("Error initializing SDL renderer: %s\n", SDL_GetError());
            goto Quit;
        }
    }
    char* paths[6] = {"assets/background.bmp", "assets/hive.bmp", "assets/mid-bee.bmp", "assets/flower.bmp", "assets/lake.bmp", "assets/progress-bar.bmp"};
    SDL_Texture **textures = malloc(sizeof(SDL_Texture *) * 6);

    for (int i = 0; i < 6; i++) {
        textures[i] = loadImage(renderer, textures[i], paths[i]);
    }

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

/**
 * Method to free the memory of the bees
 */
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

/**
 * Method to free the memory of the flowers
 */
void freeFlowers() {
    ListElement *actualElement = globalWorld->flowers->firstElement;
    Flower *flower;

    while (actualElement != NULL) {
        flower = actualElement->value;
        free(flower->location);
        actualElement = actualElement->nextElement;
    }
}

/**
 * Method to free the memory of the lakes
 */
void freeLakes() {
    ListElement *actualLakeElement = globalWorld->lakes->firstElement;

    while (actualLakeElement != NULL) {
        Lake *lake = actualLakeElement->value;
        ListElement *actualLocationElement = lake->locations->firstElement;
        Location *location;

        while (actualLocationElement != NULL) {
            location = actualLocationElement->value;
            free(location);
            actualLocationElement = actualLocationElement->nextElement;
        }
        actualLakeElement = actualLakeElement->nextElement;
    }
}

/**
 * Method to free the memory of the hives
 */
void freeHives() {
    ListElement *actualHiveElement = globalWorld->hives->firstElement;

    while (actualHiveElement != NULL) {
        Hive *hive = actualHiveElement->value;

        freeList(hive->locations);
        freeList(hive->bees);

        actualHiveElement = actualHiveElement->nextElement;
    }
}


int main() {
    srand(time(NULL));

    World *world = createWorld();

    /*printf("SELECTED PARAMETERS:\n");
    printf("Enter the World size (Ex: 1920 1080): ");
    scanf("%d %d", &world->width, &world->height);*/

    world->width = 1280; //TODO REMOVE
    world->height = 720;

    Hive *hive = createHive(1, world->width / 2 , world->height / 2, 32, 32);
   // Lake *lake = createLake(1, 128, 128, 80, 80);

    addHive(world, hive);
   // addLake(world, lake);

    createBees(hive,INITIAL_BEES + (rand() % (INITIAL_BEES / 3)));
    createLakes(world, INITIAL_LAKES + (rand() % (INITIAL_LAKES / 4)));
    createFlowers(world, INITIAL_FLOWERS + (rand() % (INITIAL_FLOWERS /4)));

    globalWorld = world;
    startSimulation(world);

    freeBees();
    freeFlowers();
    freeLakes();
    freeHives();

    freeList(world->flowers);
    freeList(world->hives);
    freeList(world->lakes);

    free(world);
    return 0;
}


