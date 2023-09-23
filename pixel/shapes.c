#include "pixel.h"

void DrawBoundaries(struct Window *window, int strokeWidth, Color fillColor)
{
    DrawRect(window, 0, 0, window->size.width, strokeWidth, WHITE_COLOR);
    DrawRect(window, 0, window->size.height - strokeWidth, window->size.width, strokeWidth, WHITE_COLOR);
    DrawRect(window, 0, 0, strokeWidth, window->size.height, WHITE_COLOR);
    DrawRect(window, window->size.width - strokeWidth, 0, strokeWidth, window->size.height, WHITE_COLOR);
}

POINT DrawLine(struct Window *window, int x0, int y0, int x1, int y1, int strokeWidth, Color color, int (*callbackFn)(int, int, int, int))
{
    if (!strokeWidth)
        return (POINT){.x = x0, .y = y0};

    int dy = y1 - y0;
    int dx = x1 - x0;

    // first of all, handle straight lines if callback function undefined
    if ((!dy || !dx) && !callbackFn)
    {
        if (!dy)
            DrawRect(window, x0, y0, dx, strokeWidth, color);
        else if (!dx)
            DrawRect(window, x0, y0, strokeWidth, dy, color);
        return (POINT){.x = x1, .y = y1};
    }

    // copied from the wikipedia page - sue me
    dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    int x2 = x0;
    int y2 = y0;

    int ret = 0;

    while (1)
    {
        if (x0 >= window->size.width || y0 >= window->size.height || x0 < 0 || y0 < 0)
            return (POINT){.x = x2, .y = y2};

        if ((ret = callbackFn(x2, y2, x0, y0))) // stop if condition met
            return ret == PREVIOUS ? (POINT){.x = x2, .y = y2} : (POINT){.x = x0, .y = y0};

        x2 = x0;
        y2 = y0;

        DrawRect(window, x0, y0, strokeWidth, strokeWidth, color);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * error;
        if (e2 >= dy)
        {
            if (x0 == x1)
                break;
            error = error + dy;
            x0 = x0 + sx;
        }
        if (e2 <= dx)
        {
            if (y0 == y1)
                break;
            error = error + dx;
            y0 = y0 + sy;
        }
    }

    return (POINT){.x = x1, .y = y1};
}