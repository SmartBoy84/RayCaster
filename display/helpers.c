#include "helpers.h"

void SetScreen(struct Window *window, int r, int g, int b)
{
    memset(window->pixels, r << 16 | g << 8 | b, sizeof(uint32_t) * window->size.width * window->size.height);
}

int ChangePixel(struct Window *window, int x, int y, int r, int g, int b)
{
    // Check if the pixel is within bounds!
    if (x >= window->size.width || y >= window->size.height)
    {
        printf("Pixel out of bounds!\n");
        return 0;
    }

    // Set the color
    window->pixels[((y - 1) * window->size.width) + x] = r << 16 | g << 8 | b;
    return 1;
}

int ChangeFrameRate(struct Window *window, int frameRate)
{
    if (frameRate <= 0)
        return 1;

    window->frameRate = frameRate;
    return StartLoop(window);
}

int ChangeDimensions(struct Window *window, int width, int height)
{
    BOOL ret = SetWindowPos(window->windowHandler, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
    if (!ret)
        return 1;

    window->size.width = width;
    window->size.height = height;
    return 0;
}