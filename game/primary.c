#include "game.h"

clock_t startTime;
char fps[256];

void FPS()
{
    static clock_t prevTime = 0;
    clock_t currentTime = clock();

    double deltaTime = (double)(currentTime - prevTime) / CLOCKS_PER_SEC;
    sprintf(fps, "FPS: %.2f", 1.0 / deltaTime);
    prevTime = currentTime;
}

void ToggleScreen(struct Window *window)
{
    if (window->state & BIRTHED) // only flip if initialiser routine has run (i.e., this is NOT the first time this is run)
        primary_map = !primary_map;

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

    MoveCanvas(window->nextWindow, window->position.x + window->size.width - window->nextWindow->size.width, window->position.y); // position in the top right corner

    // set new parameters
    unit_size = (primary_map ? window->size.width : window->nextWindow->size.width) / MAP_WIDTH;
    cross_length = FLOOR_INT(sqrt(pow(unit_size * MAP_WIDTH, 2) + pow(unit_size * MAP_HEIGHT, 2)));
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
    memset(game_map, 0, MAP_WIDTH * MAP_HEIGHT);
}

void F_HotKey(struct Window *window)
{
    fps_show = !fps_show;
}

void R_HotKey(struct Window *window)
{
    if (!(window->nextWindow->state & CLOSED))
    {
        PauseLoop(window->nextWindow);
        HideCanvas(window->nextWindow);
    }
    else
    {
        StartLoop(window->nextWindow);
        ShowCanvas(window->nextWindow);
    }
}

void configure_collisions()
{
    if (ray_collisions)
        free(ray_collisions);
    ray_count = (int)ceil((float)lines * FOV);
    ray_collisions = calloc(ray_count, sizeof(struct raylission));
}

void MouseWheel(struct Window *window, LPARAM lParam, WPARAM wParam)
{
    if (primary_map)
    {
        lines = (int)(lines + GET_WHEEL_DELTA_WPARAM(wParam) * mouse_wheel);
        lines = lines < 0 ? 0 : lines > max_lines ? max_lines
                                                  : lines;
    }
    else
    {
        FOV = modf(FOV + (GET_WHEEL_DELTA_WPARAM(wParam) * mouse_wheel * 0.00005), NULL);
        FOV = FOV < 0 ? 0.0 : FOV; // novelty I can't be bothered adding right now
    }

    configure_collisions();
}

void ClosedRoutine(struct Window *window)
{
    printf("Closing secondary\n");
    DestroyCanvas(window->nextWindow);
}

void PrimarySetup(struct Window *window)
{
    printf("Started primary!\n");

    startTime = clock();                      // Initialize startTime when the program starts
    AddHotkey(window, &Q_HotKey, 0, 'Q');     // quit application
    AddHotkey(window, &G_HotKey, 0, 'G');     // hide/show grid
    AddHotkey(window, &ToggleScreen, 0, 'M'); // change mode (flip minimap)
    AddHotkey(window, &C_HotKey, 0, 'C');     // clear the map
    AddHotkey(window, &F_HotKey, 0, 'F');     // hide/show fps
    AddHotkey(window, &R_HotKey, 0, 'R');     // hide/show secondary screen

    MakeTopmost(window, TRUE);

    window->runners.canvasDestroyed = &ClosedRoutine;
    window->runners.mouseWheel = &MouseWheel;

    // create secondary canvas
    struct Window *secondary = CreateCanvas(1, 1, BASIC_CANVAS, "secondary window",
                                            FRAMERATE, &SecondaryUpdate,
                                            (struct Runners){.canvasInitialised = &SecondarySetup}, window);
    ToggleScreen(window);
    configure_collisions();
    printf("Created secondary\n");
}

void PrimaryUpdate(struct Window *window)
{
    cursor_pos = GetRelativeCursorPos(window); // we want the cursor position in main frame

    if (primary_map)
        RenderMap(window);
    else
        RenderGame(window);

    if (fps_show)
    {
        FPS();
        DrawString(window, fps, 10, 10, 2, 0, WHITE_COLOR, RGBA_TO_COLOR(208, 219, 53, 200));
    }

    DrawBoundaries(window, 1, WHITE_COLOR);
}