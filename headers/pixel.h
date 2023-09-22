#ifndef PIXEL_H
#define PIXEL_H

#include "display.h"
#include "assert.h"

typedef int Color[4];

// I hate intellisense formatting
#define TRANSPARENT_COLOR \
    (Color) { 0, 0, 0, 0 }
#define WHITE_COLOR \
    (Color) { 255, 255, 255, 255 }
#define BLACK_COLOR \
    (Color) { 0, 0, 0, 255 }
#define RGBA_TO_ARRAY(r, g, b, a) \
    (Color) { (uint8_t)(r), (uint8_t)(g), (uint8_t)(b), (uint8_t)a }
#define UINT32_TO_COLOR(colorValue)           \
    (Color)                                   \
    {                                         \
        (int)((colorValue >> 24) & 0xFF),     \
            (int)((colorValue >> 16) & 0xFF), \
            (int)((colorValue >> 8) & 0xFF),  \
            (int)(colorValue & 0xFF)          \
    }
#define EXTRACT_COLOR(arr) ((arr[0] << 16) | (arr[1] << 8) | (arr[2]))
#define GET_INDEX(window, x, y)                                                                    \
    ({                                                                                             \
        assert((x) >= 0 && (x) < (window)->size.width && (y) >= 0 && (y) < (window)->size.height); \
        ((y) * (window)->size.width + (x));                                                        \
    })

// Pixel buffer handlers
void SetScreen(struct Window *window, Color color);
void ChangePixel(struct Window *window, int x, int y, Color color);

// Shape API
void DrawRect(struct Window *window, int posX, int posY, int width, int height, Color fillColor);
void DrawLine(struct Window *window, int x1, int y1, int x2, int y2, int strokeWidth, Color color);

// text
void DrawString(struct Window *window, char *string, int posX, int posY, int scalarPx, int characterWarp, Color textColor, Color backColor);
int DrawLetter(struct Window *window, char character, int posX, int posY, int scalarPx, Color textColor, Color backColor);

#endif