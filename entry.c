#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "game.h"

BOOL grid = TRUE;
BOOL primary_map = FALSE;
uint8_t *map;

int main()
{
    map = calloc(MAP_WIDTH * MAP_HEIGHT, sizeof(uint8_t));

    // begins the event loop and creates the primary "head" window
    KindlyBegin(CreateCanvas(SCREEN_WIDTH, SCREEN_HEIGHT,    // window dimensions
                             BASIC_CANVAS, "primary window", // style and title
                             FRAMERATE, &PrimaryUpdate,      // frame rate and update routine
                             (struct Runners){.canvasInitialised = &PrimarySetup, .canvasDestroyed = &ClosedRoutine}, NULL));
}