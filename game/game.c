#include "game.h"

const Color grid_color = WHITE_COLOR;
const Color map_background = BLACK_COLOR;
const Color unit_selected = WHITE_COLOR;
const Color unit_hover = RGBA_TO_COLOR(255, 255, 255, 60);

// Initial states
BOOL grid = TRUE;
BOOL primary_map = TRUE;

// Internal shizzle
int unit_size;
uint8_t *map;

int Ray(int x0, int y0, int x1, int y1)
{
    if (map[GET_MAP_INDEX(x1, y1, unit_size)])
    {
        printf("How?(%d, %d)", x1 / unit_size, y1 / unit_size);
    }
    return map[GET_MAP_INDEX(x1, y1, unit_size)] ? PREVIOUS : 0;
}

void RenderMap(struct Window *window)
{
    SetScreen(window, map_background); // clear the screen
    POINT cursor = GetRelativeCursorPos(window);

    // Draw boundaries
    if (grid)
    {
        for (int x = 0; x <= MAP_WIDTH; x++)
            DrawLine(window, x * unit_size, 0, x * unit_size, window->size.height, 1, grid_color, NULL);

        for (int y = 0; y <= MAP_HEIGHT; y++)
            DrawLine(window, 0, y * unit_size, window->size.width, y * unit_size, 1, grid_color, NULL);

        POINT coord = (POINT){.x = (int)floor((float)cursor.x / unit_size), .y = (int)floor((float)cursor.y / unit_size)};
        if (coord.x >= 0 && coord.y >= 0)
        {
            if (GetAsyncKeyState(VK_LBUTTON) || GetAsyncKeyState(VK_RBUTTON))
            {
                map[coord.y * MAP_WIDTH + coord.x] = GetAsyncKeyState(VK_LBUTTON) != 0;
                printf("Run");
            }
            DrawRect(window, coord.x * unit_size + 1, coord.y * unit_size + 1, unit_size - 1, unit_size - 1, unit_hover);
        }
    }

    POINT finalPoint = DrawLine(window, cursor.x, cursor.y, window->size.width, 0, 1, WHITE_COLOR, Ray);

    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            DrawRect(window, x * unit_size, y * unit_size, unit_size, unit_size, map[(y * MAP_WIDTH) + x] ? unit_selected : TRANSPARENT_COLOR);
}

void RenderGame(struct Window *window)
{
    SetScreen(window, RGBA_TO_COLOR(156, 99, 34, 255));
}