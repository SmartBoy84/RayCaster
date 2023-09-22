#ifndef GAME_H
#define GAME_H

#include "display.h"
#include "pixel.h"
#include "helpers.h"
#include "math.h"

#define WIDTH 100
#define HEIGHT 100
#define UNIT_SIZE 10
#define FRAMERATE 120

void PrimarySetup(struct Window *window);
void PrimaryUpdate(struct Window *window);
void ClosedRoutine(struct Window *window);

void SecondarySetup(struct Window *window);
void SecondaryUpdate(struct Window *window);

#endif