#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "game.h"

int main()
{
    game_map = calloc(MAP_WIDTH * MAP_HEIGHT, sizeof(uint8_t));

    // begins the event loop and creates the primary "head" window
    KindlyBegin(CreateCanvas(1, 1,                           // window dimensions
                             BASIC_CANVAS, "primary window", // style and title
                             FRAMERATE, &PrimaryUpdate,      // frame rate and update routine
                             (struct Runners){.canvasInitialised = &PrimarySetup}, NULL));
}