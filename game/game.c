#include "game.h"

const Color grid_color = RGBA_TO_COLOR(255, 255, 255, 255);
const Color unit_selected = RGBA_TO_COLOR(255, 255, 255, 255);
const Color unit_hover = RGBA_TO_COLOR(255, 255, 255, 60);

void RenderMap(struct Window *window)
{
    SetScreen(window, BLACK_COLOR); // clear the screen

    int unit_size = window->size.width / MAP_WIDTH;

    if (grid)
    {
        for (int x = 0; x <= MAP_WIDTH; x++)
            DrawLine(window, x * unit_size, 0, x * unit_size, window->size.height, 1, grid_color);

        for (int y = 0; y <= MAP_HEIGHT; y++)
            DrawLine(window, 0, y * unit_size, window->size.width, y * unit_size, 1, grid_color);

        // it was annoying the hell out of me
        DrawLine(window, 0, window->size.height - 1, window->size.width, window->size.height - 1, 1, grid_color);
        DrawLine(window, window->size.width - 1, 0, window->size.width - 1, window->size.height, 1, grid_color);

        POINT cursor = GetRelativeCursorPos(window);

        POINT coord = (POINT){.x = (int)floor((float)cursor.x / unit_size), .y = (int)floor((float)cursor.y / unit_size)};
        if (coord.x >= 0 && coord.y >= 0)
        {
            if (GetAsyncKeyState(VK_LBUTTON) || GetAsyncKeyState(VK_RBUTTON))
                map[coord.y * MAP_WIDTH + coord.x] = GetAsyncKeyState(VK_LBUTTON) != 0;
            DrawRect(window, coord.x * unit_size + 1, coord.y * unit_size + 1, unit_size - 1, unit_size - 1, unit_hover);
        }
    }

    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            DrawRect(window, x * unit_size, y * unit_size, unit_size, unit_size, map[(y * MAP_WIDTH) + x] ? unit_selected : TRANSPARENT_COLOR);
}

void RenderGame(struct Window *window)
{
    SetScreen(window, RGBA_TO_COLOR(156, 99, 34, 255));
}