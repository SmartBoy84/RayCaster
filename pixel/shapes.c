#include "pixel.h"

void DrawLine(struct Window *window, int x0, int y0, int x1, int y1, int strokeWidth, Color color)
{
    if (!strokeWidth)
        return;

    int dy = y1 - y0;
    int dx = x1 - x0;

    // first of all, handle straight lines
    if (!dy || !dx)
    {
        if (!dy)
            DrawRect(window, x0, y0, dx, strokeWidth, color);
        else if (!dx)
            DrawRect(window, x0, y0, strokeWidth, dy, color);
        return;
    }

    // copied from the wikipedia page - sue me
    dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while (1)
    {
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
}