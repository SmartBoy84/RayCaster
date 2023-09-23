#ifndef GAME_H
#define GAME_H

#include "display.h"
#include "pixel.h"
#include "helpers.h"
#include "math.h"

// first bit indicates a collision has occured
#define LEFT_RIGHT 0x11
#define TOP_BOTTOM 0x10

struct raylission
{
    int side;
    POINT point;
};


// Primary dimensions
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 750

#define MAP_WIDTH 30 // Map size
#define MAP_HEIGHT 30
#define UNIT_SIZE 30

#define SECONDARY_SCALAR 0.2 // Percentage of the primary screen dimensions
#define FRAMERATE 120

#define GET_MAP_INDEX(x, y, unit_size) \
    ((int)floor((y) / (unit_size)) * (MAP_WIDTH) + (int)floor((x) / (unit_size)))

#define GET_KEY_STATE(key) (GetAsyncKeyState(key) != 0)

// function definitions
void PrimarySetup(struct Window *window);
void PrimaryUpdate(struct Window *window);

void SecondarySetup(struct Window *window);
void SecondaryUpdate(struct Window *window);

void RenderGame(struct Window *window);
void RenderMap(struct Window *window);

// variable sharing
extern BOOL fps_show;
extern BOOL secondary_show;

extern BOOL primary_map;
extern BOOL grid;
extern uint8_t *game_map;
extern struct raylission *ray_collisions;

extern int unit_size;
extern int cross_length;
extern float FOV;
extern float phase_shift;
extern const float two_pi;
extern const float pi;
extern int lines;
extern int mouse_wheel;
extern int max_lines;
extern POINT cursor_pos;
extern int ray_count;

#endif