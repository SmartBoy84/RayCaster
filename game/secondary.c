#include "game.h"

void M_HotKey(struct Window *window)
{
    if (!(window->state & CLOSED))
        HideCanvas(window);
    else
        ShowCanvas(window);
}

void SecondaryDestroyed(struct Window *window)
{
    printf("Secondary destroyed");
}

void SecondarySetup(struct Window *window)
{
    printf("Hello from secondary screen!\n");

    MakeTopmost(window, TRUE);
    SetScreen(window, BLACK_COLOR);

    AddHotkey(window, M_HotKey, 0, 0x4D);
    window->runners.canvasDestroyed = &SecondaryDestroyed;
}

void SecondaryUpdate(struct Window *window)
{
    MakeTopmost(window, TRUE);
}
