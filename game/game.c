#include "game.h"

// main buffer
uint8_t *game_map;

// map settings
Color grid_color = WHITE_COLOR;
Color map_background = BLACK_COLOR;
Color unit_selected = WHITE_COLOR;
Color unit_hover = RGBA_TO_COLOR(255, 255, 255, 60);
Color ray_color = RGBA_TO_COLOR(255, 255, 0, 255);

// Initial states
BOOL grid = TRUE;
BOOL primary_map = TRUE;
BOOL fps_show = FALSE;
BOOL secondary_show = FALSE;

// ray parameters
float FOV = 0.2; // [0,1] - what percentage of 2pi
int lines = 2500;
int max_lines = 8000;
int mouse_wheel = 2;
int sensitivity = 2; // number of rotations to complete as user moves mouse across screen

// player settings
const float speed = 0.01;                           // px/frame
const float diagonal_speed = speed / 1.41421356237; // speed/sqrt(2) => to fix increased speed of diagonal movement - derivation: sqrt(sq(speed) + sq(speed)) = sqrt(2) * sq(speed)
float render_distance = 0.4;                        // percentage of ray of max length

// Internal shizzle
float phase_shift = 0;
const float pi = 3.1415926535;
const float two_pi = pi * 2;

int unit_size;
int cross_length;

float relative_player_pos[2];
int actual_player_pos[2];
POINT cursor_pos; // cursor position relative to the primary screen

int RayCallback(int x0, int y0, int x1, int y1)
{
    int old_index = GET_MAP_INDEX(x0, y0, unit_size);
    int new_index = GET_MAP_INDEX(x1, y1, unit_size);

    // edge case must be handled because, by nature, the bresenham line algorithm can jump across this gap and seep through two squares
    if (abs(new_index - old_index) == MAP_WIDTH - 1 || abs(new_index - old_index) == MAP_WIDTH + 1) // +1 for crossing between quad i/iii and -1 for crossing between quad ii/iv (acw)
    {
        return (
                   (game_map[old_index] || game_map[new_index]) // first check the squares we are in

                   || (new_index > old_index // line is angled towards bottom two quadrants
                       && (game_map[GET_MAP_INDEX(x1, y1 - 1, unit_size)] || game_map[GET_MAP_INDEX(x0, y0 + 1, unit_size)]))

                   || (new_index < old_index // line is angled towards top two quadrants
                       && (game_map[GET_MAP_INDEX(x1, y1 + 1, unit_size)] || game_map[GET_MAP_INDEX(x0, y0 - 1, unit_size)])))

                   ? PREVIOUS
                   : 0;
    }

    return game_map[new_index] ? PREVIOUS : 0;
}

void RenderMap(struct Window *window)
{
    SetScreen(window, map_background); // clear the screen

    actual_player_pos[0] = relative_player_pos[0] * window->size.width;
    actual_player_pos[1] = relative_player_pos[1] * window->size.height;

    // Draw boundaries
    if (grid)
    {
        POINT cursor = GetRelativeCursorPos(window);

        for (int x = 0; x <= MAP_WIDTH; x++)
            DrawLine(window, x * unit_size, 0, x * unit_size, window->size.height, 1, grid_color, NULL);

        for (int y = 0; y <= MAP_HEIGHT; y++)
            DrawLine(window, 0, y * unit_size, window->size.width, y * unit_size, 1, grid_color, NULL);

        POINT coord = (POINT){.x = (int)floor((float)cursor.x / unit_size), .y = (int)floor((float)cursor.y / unit_size)};
        if (coord.x >= 0 && coord.y >= 0)
        {
            if (GET_KEY_STATE(VK_LBUTTON) || GET_KEY_STATE(VK_RBUTTON))
                game_map[coord.y * MAP_WIDTH + coord.x] = GET_KEY_STATE(VK_LBUTTON);
            DrawRect(window, coord.x * unit_size + 1, coord.y * unit_size + 1, unit_size - 1, unit_size - 1, unit_hover);
        }
    }
    else
        for (float i = phase_shift; i < phase_shift + two_pi * FOV; i += two_pi / lines)
            DrawLine(window, actual_player_pos[0], actual_player_pos[1], actual_player_pos[0] + render_distance * cos(i) * cross_length, actual_player_pos[1] + render_distance * sin(i) * cross_length, 1, ray_color, RayCallback);

    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            DrawRect(window, x * unit_size, y * unit_size, unit_size, unit_size, game_map[(y * MAP_WIDTH) + x] ? unit_selected : TRANSPARENT_COLOR);

    phase_shift = primary_map ? atan2((cursor_pos.y - actual_player_pos[1]), (cursor_pos.x - actual_player_pos[0])) - 0.5 * FOV * two_pi // point at cursor
                              : (two_pi * cursor_pos.x * sensitivity / (GetHead(window)->size.width));

    // the cool thing is that this could be handled anywhere
    float current_speed = 0;
    if ((GET_KEY_STATE('A') || GET_KEY_STATE('D')) && (GET_KEY_STATE('W') || GET_KEY_STATE('S')))
        current_speed = diagonal_speed;
    else
        current_speed = speed;

    float new_position[2] = {
        CLAMP((relative_player_pos[0] + (GET_KEY_STATE('A') * -current_speed) + (GET_KEY_STATE('D') * current_speed)), 0, 1), // new x pos
        CLAMP((relative_player_pos[1] + (GET_KEY_STATE('W') * -current_speed) + (GET_KEY_STATE('S') * current_speed)), 0, 1),
    }; // new y pos

    // Very, very rudimentary and ugly collision system - it is what it is, I can cop a few fps
    // the trick is we test with each coordinate to see if the player is in a activated square - if so, then that coordinate in the x/y direction isn't set
    relative_player_pos[0] = game_map[FLOOR_INT(MAP_HEIGHT * relative_player_pos[1]) * MAP_WIDTH + FLOOR_INT(MAP_WIDTH * new_position[0])] ? relative_player_pos[0] : new_position[0];
    relative_player_pos[1] = game_map[FLOOR_INT(MAP_HEIGHT * new_position[1]) * MAP_WIDTH + FLOOR_INT(MAP_WIDTH * relative_player_pos[0])] ? relative_player_pos[1] : new_position[1];
}

void RenderGame(struct Window *window)
{
    SetScreen(window, RGBA_TO_COLOR(156, 99, 34, 255));
}