#ifndef HELPERS_H
#define HELPERS_H

#include "display.h"

int ChangeFrameRate(struct Window *window, int frameRate); // change the framerate
int ChangeDimensions(struct Window *window, int width, int height);

// Helper utilities
int ShowCanvas(struct Window *window);
int HideCanvas(struct Window *window);
int MoveCanvas(struct Window *window, int posX, int posY);
int MakeTopmost(struct Window *window, BOOL state);

int AddHotkey(struct Window *window, void *callback, UINT fsModifiers, UINT vk);
int RemoveHotKey(struct Window *window, int id);

POINT GetRelativeCursorPos(struct Window *window);

#endif