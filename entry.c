#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "game.h"

int main()
{

    // begins the event loop and creates the primary "head" window
    KindlyBegin(CreateCanvas(UNIT_SIZE * WIDTH, UNIT_SIZE * HEIGHT, // window dimensions
                             BASIC_CANVAS, "primary window",        // style and title
                             FRAMERATE, &PrimaryUpdate,             // frame rate and update routine
                             (struct Runners){.canvasInitialised = &PrimarySetup, .canvasDestroyed = &ClosedRoutine}, NULL));
}