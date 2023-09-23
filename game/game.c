#include "game.h"

// main buffer
uint8_t *game_map;

// map settings
const Color grid_color = WHITE_COLOR;
const Color map_background = BLACK_COLOR;
const Color unit_selected = WHITE_COLOR;
const Color unit_hover = RGBA_TO_COLOR(255, 255, 255, 60);
const Color ray_color = RGBA_TO_COLOR(255, 255, 0, 255);

// Initial states
BOOL grid = TRUE;
BOOL primary_map = TRUE;
BOOL fps_show = FALSE;
BOOL secondary_show = FALSE;

// ray parameters
float FOV = 0.2; // [0,1] - what percentage of 2pi
int lines = 5000;
int max_lines = 8000;
int mouse_wheel = 2;

// Internal shizzle
float phase_shift = 0;
float two_pi = 3.1415926535 * 2;
int unit_size;
int cross_length;

int RayCallback(int x0, int y0, int x1, int y1)
{
    return game_map[GET_MAP_INDEX(x1, y1, unit_size)] ? PREVIOUS : 0;
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
                game_map[coord.y * MAP_WIDTH + coord.x] = GetAsyncKeyState(VK_LBUTTON) != 0;
            DrawRect(window, coord.x * unit_size + 1, coord.y * unit_size + 1, unit_size - 1, unit_size - 1, unit_hover);
        }
    }
    else
        for (float i = phase_shift; i < phase_shift + two_pi * FOV; i += two_pi / lines)
            DrawLine(window, cursor.x, cursor.y, cross_length * cos(i), cross_length * sin(i), 1, ray_color, RayCallback);

    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            DrawRect(window, x * unit_size, y * unit_size, unit_size, unit_size, game_map[(y * MAP_WIDTH) + x] ? unit_selected : TRANSPARENT_COLOR);
}

void RenderGame(struct Window *window)
{
    SetScreen(window, RGBA_TO_COLOR(156, 99, 34, 255));
}