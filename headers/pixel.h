#ifndef PIXEL_H
#define PIXEL_H

#include <assert.h>
#include <math.h>
#include "display.h"

typedef int Color[4];

#define TRANSPARENT_COLOR ((Color){0, 0, 0, 0})
#define WHITE_COLOR ((Color){255, 255, 255, 255})
#define BLACK_COLOR ((Color){0, 0, 0, 255})
#define RGBA_TO_COLOR(r, g, b, a) ((Color){r, g, b, a})
#define COLOR_TO_UINT32(color) (((uint32_t)(color[0]) << 16) | ((uint32_t)(color[1]) << 8) | (uint32_t)(color[2]))
#define IS_TRANSPARENT(color) (color[3] == 0)

#define GET_INDEX(window, x, y)                                                                    \
    ({                                                                                             \
        assert((x) >= 0 && (x) < (window)->size.width && (y) >= 0 && (y) < (window)->size.height); \
        ((y) * (window)->size.width + (x));                                                        \
    })

#define FLOOR_INT(value) ((int)floor((value)))

#define CLAMP(value, min_val, max_val) \
    ((value) < (min_val) ? (min_val) : ((value) > (max_val) ? (max_val) : (value)))

// this is for the line api callback - since the callback function doesn't know what the gradient of the line is
#define CURRENT 0x01
#define PREVIOUS 0x10

// Pixel buffer handlers
void SetScreen(struct Window *window, Color color);
void ChangePixel(struct Window *window, int x, int y, Color color);

// Shape API
void DrawRect(struct Window *window, int posX, int posY, int width, int height, Color fillColor);
POINT DrawLine(struct Window *window, int x0, int y0, int x1, int y1, int strokeWidth, Color color, int (*callbackFn)(int, int, int, int));

// text
void DrawString(struct Window *window, char *string, int posX, int posY, int scalarPx, int characterWarp, Color textColor, Color backColor);
int DrawLetter(struct Window *window, char character, int posX, int posY, int scalarPx, Color textColor, Color backColor);

void DrawBoundaries(struct Window *window, int strokeWidth, Color fillColor);

#endif