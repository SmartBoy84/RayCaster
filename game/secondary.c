#include "game.h"

void SecondaryDestroyed(struct Window *window)
{
    printf("Secondary destroyed\n");
    free(game_map);
}

void SecondarySetup(struct Window *window)
{
    printf("Hello from secondary screen!\n");

    window->runners.canvasDestroyed = &SecondaryDestroyed;

    if (!secondary_show)
    {
        PauseLoop(window);
        HideCanvas(window);
    }
}

void SecondaryUpdate(struct Window *window)
{
    MakeTopmost(window, TRUE); // keep at the top

    if (primary_map)
        RenderGame(window);
    else
        RenderMap(window);

    DrawBoundaries(window, 1, WHITE_COLOR);
}
