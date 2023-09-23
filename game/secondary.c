#include "game.h"

void S_HotKey(struct Window *window)
{
    if (!(window->state & CLOSED))
    {
        PauseLoop(window);
        HideCanvas(window);
    }
    else
    {
        StartLoop(window);
        ShowCanvas(window);
    }
}

void SecondaryDestroyed(struct Window *window)
{
    printf("Secondary destroyed\n");
    free(map);
}

void SecondarySetup(struct Window *window)
{
    printf("Hello from secondary screen!\n");

    AddHotkey(window, S_HotKey, 0, 'S');
    window->runners.canvasDestroyed = &SecondaryDestroyed;
}

void SecondaryUpdate(struct Window *window)
{
    MakeTopmost(window, TRUE); // keep at the top

    if (primary_map)
        RenderGame(window);
    else
        RenderMap(window);

    DrawBoundaries(window, 1, WHITE_COLOR);
    DrawString(window, "Minimap", 0, 0, 1, 0, WHITE_COLOR, TRANSPARENT_COLOR);
}
