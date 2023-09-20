#ifndef HELPERS_H
#define HELPERS_H

#include "display.h"

int ChangeFrameRate(struct Window *window, int frameRate);                 // change the framerate
void SetScreen(struct Window *window, int r, int g, int b);                // sets every pixel to a single colour
int ChangePixel(struct Window *window, int x, int y, int r, int g, int b); // changes a single pixel - to be pixed, do I really need this or can I trust myself?
int ChangeDimensions(struct Window *window, int width, int height);

#endif