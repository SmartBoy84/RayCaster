#include "game.h"

clock_t startTime;
char fps[256]; // Change fps to a fixed-size array

BOOL grid = TRUE;
Color grid_color = (int[4]){255, 255, 255, 50};

void FPS()
{
    static clock_t prevTime = 0;
    clock_t currentTime = clock();

    double deltaTime = (double)(currentTime - prevTime) / CLOCKS_PER_SEC;
    sprintf(fps, "FPS: %.2f", 1.0 / deltaTime);
    prevTime = currentTime;
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

void PrimarySetup(struct Window *window)
{
    printf("Started primary!\n");

    startTime = clock(); // Initialize startTime when the program starts
    AddHotkey(window, &Q_HotKey, 0, 0x51);
    AddHotkey(window, &G_HotKey, 0, 0x47);

    MakeTopmost(window, TRUE);

    // create secondary canvas
    struct Window *secondary = CreateCanvas(WIDTH, HEIGHT, BASIC_CANVAS, "secondary window",
                                            FRAMERATE, &SecondaryUpdate,
                                            (struct Runners){.canvasInitialised = &SecondarySetup}, window);

    MoveCanvas(secondary, window->position.x + window->size.width - secondary->size.width, window->position.y); // position in the top right corner
    printf("Created secondary\n");
}

void ClosedRoutine(struct Window *window)
{
    printf("Closing secondary");
    DestroyCanvas(window->nextWindow);
}

void PrimaryUpdate(struct Window *window)
{
    SetScreen(window, BLACK_COLOR); // clear the screen
    POINT cursor = GetRelativeCursorPos(window);

    if (grid)
    {
        for (int x = 0; x <= window->size.width; x += UNIT_SIZE)
            DrawLine(window, x, 0, x, window->size.height, 1, grid_color);

        for (int y = 0; y <= window->size.height; y += UNIT_SIZE)
            DrawLine(window, 0, y, window->size.width, y, 1, grid_color);

        // it was annoying the hell out of me
        DrawLine(window, 0, window->size.height - 1, window->size.width, window->size.height - 1, 1, grid_color);
        DrawLine(window, window->size.width - 1, 0, window->size.width - 1, window->size.height, 1, grid_color);

        if (GetAsyncKeyState(VK_LBUTTON))
            ChangePixel(window->nextWindow, (int)floor((float)cursor.x * WIDTH / window->size.width), (int)floor((float)cursor.y * HEIGHT / window->size.height), WHITE_COLOR);
    }

    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            DrawRect(window, x * UNIT_SIZE, y * UNIT_SIZE, UNIT_SIZE, UNIT_SIZE, UINT32_TO_COLOR(window->nextWindow->pixels[GET_INDEX(window->nextWindow, x, y)]));

    FPS();
    DrawString(window, fps, 10, 10, 2, 0, WHITE_COLOR, TRANSPARENT_COLOR);
}