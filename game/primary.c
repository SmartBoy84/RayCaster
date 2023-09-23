#include "game.h"

clock_t startTime;
char fps[256]; // Change fps to a fixed-size array

void FPS()
{
    static clock_t prevTime = 0;
    clock_t currentTime = clock();

    double deltaTime = (double)(currentTime - prevTime) / CLOCKS_PER_SEC;
    sprintf(fps, "FPS: %.2f", 1.0 / deltaTime);
    prevTime = currentTime;
}

void CornerSecondary(struct Window *window)
{
    MoveCanvas(window->nextWindow, window->position.x + window->size.width - window->nextWindow->size.width, window->position.y); // position in the top right corner
}

void Q_HotKey(struct Window *window)
{
    printf("Closing primary window!\n");
    DestroyCanvas(window);
}

void G_HotKey(struct Window *window)
{
    grid = !grid;
}

void C_HotKey(struct Window *window)
{
    memset(map, 0, MAP_WIDTH * MAP_HEIGHT);
}

void M_HotKey(struct Window *window)
{
    primary_map = !primary_map;

    // first position the primary window
    if (primary_map)
    {
        ChangeDimensions(window, MAP_WIDTH * UNIT_SIZE, MAP_HEIGHT * UNIT_SIZE);
        ChangeDimensions(window->nextWindow, SCREEN_WIDTH * SECONDARY_SCALAR, SCREEN_HEIGHT * SECONDARY_SCALAR);
    }
    else
    {
        ChangeDimensions(window, SCREEN_WIDTH, SCREEN_HEIGHT);
        ChangeDimensions(window->nextWindow, MAP_WIDTH * UNIT_SIZE * SECONDARY_SCALAR, MAP_HEIGHT * UNIT_SIZE * SECONDARY_SCALAR);
    }

    MakeCenter(window); // center the primary window on screen
    CornerSecondary(window);
}

void PrimarySetup(struct Window *window)
{
    printf("Started primary!\n");

    startTime = clock(); // Initialize startTime when the program starts
    AddHotkey(window, &Q_HotKey, 0, 'Q');
    AddHotkey(window, &G_HotKey, 0, 'G');
    AddHotkey(window, &M_HotKey, 0, 'M');
    AddHotkey(window, &C_HotKey, 0, 'C');

    MakeTopmost(window, TRUE);

    // create secondary canvas
    struct Window *secondary = CreateCanvas(MAP_WIDTH * UNIT_SIZE * SECONDARY_SCALAR, MAP_HEIGHT * UNIT_SIZE * SECONDARY_SCALAR, BASIC_CANVAS, "secondary window",
                                            FRAMERATE, &SecondaryUpdate,
                                            (struct Runners){.canvasInitialised = &SecondarySetup}, window);
    CornerSecondary(window);
    printf("Created secondary\n");
}

void ClosedRoutine(struct Window *window)
{
    printf("Closing secondary\n");
    DestroyCanvas(window->nextWindow);
}

void PrimaryUpdate(struct Window *window)
{
    if (primary_map)
        RenderMap(window);
    else
        RenderGame(window);

    FPS();
    DrawString(window, fps, 10, 10, 2, 0, WHITE_COLOR, RGBA_TO_COLOR(208, 219, 53, 200));
}