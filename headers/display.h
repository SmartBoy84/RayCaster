#ifndef DISPLAY_H
#define DISPLAY_H

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

// resolve any yucky circular references
struct Runners;
struct Window;

#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif

#define NULL_RUNNERS \
    (struct Runners) {}

// type definitions
typedef void (*Handler)(struct Window *window); // custom message handler
typedef void (*Updater)(struct Window *, MSG);

// struct definitions

// inspired from https://processing.org/reference#input
// I could have global variables but it's better to just pass through the window pointer so that I can easily have multiple windows + multithreading in the future
struct Runners
{
    Handler windowDestroyed; // window closed

    Handler windowModalLoop; // when the user enters/exist modal loop
    Handler windowMoved;     // window has been moved

    Handler windowActivated; // when the user focusses on window (tab-alt, mouse click etc)

    int (*windowResized)(struct Window *window, int newWidth, int newHeight); // window has been resized - return 1 to override default behaviour (i.e., mapping buffer to new size)
    void (*windowChangePrelim)(struct Window *window, MINMAXINFO *);          // run when the user wants to change (resize/move) the window - use can change MINMAXINFO struct to whatever

    // Handler keyDown; // key pressed
    // Handler keyUp;   // key released

    // Handler mousePressed;  // mouse pressed
    // Handler mouseReleased; // mouse released
    // Handler mouseMoved;    // mouse moved over window
    // Handler mouseWheel;    // mouse wheel scrolled
};

// I can't be assed dealing with any more win api functions so I'll just keep these here
// because abstraction is inevitable - so it's either this or I write a wrapper function over the winapi for each function
struct States
{
    BOOL modalLoop;
    BOOL resizing;
    BOOL moving;

    BOOL active;

    BOOL maximised;
    BOOL minimised;
};

struct Window
{
    struct
    {
        int x;
        int y;
    } position;

    struct
    {
        int width;
        int height;
    } size; // {x, y}

    uint32_t *pixels;
    int frameRate;

    Updater updateLoop; // can't use type def here because it has this struct in its definition!
    struct Runners runners;
    struct States states;

    HWND windowHandler;
    HDC windowDC;
    WNDCLASSEX windowClass;
    BITMAPINFO bitmapInfo;

    int checkmark;  // verify it exists
    void *settings; // additional data to be lobbed about
};

// function declarations
struct Window *InitialisePixels(int width, int height, char *title, int frameRate, Updater updateLoop, struct Runners runners); // creates a new screen

int UpdateBuffer(struct Window *window, int newWidth, int newHeight); // remaps the old buffer to the new one, preserving as much information as possible

int Start(struct Window *window); // starts the window's event loop - blocks until the window is closed
int Stop(struct Window *window);  // destroys the window

int PauseLoop(struct Window *window); // resume event loop timer
int StartLoop(struct Window *window); // pause event loop timer

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); // just so I can have the main handler in a separate file

#endif