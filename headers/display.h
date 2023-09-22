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

#define BASIC_CANVAS WS_POPUP | WS_SYSMENU

// key press check - https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
#define IS_KEY_PRESSED(keycode) (GetKeyState(keycode) & 0x8000)
#define IS_KEY_UP(keycode) (!IS_KEY_PRESSED(keycode))

// Constants

// window states
#define BIRTHED 0x1000 // existence checkmark
#define PAUSED 0x0100  // window's event loop is paused
#define CLOSED 0x0010  // window is closed
#define KILLED 0x0001  // window is dead, pending deletion

// type definitions
typedef void (*Handler)(struct Window *window, LPARAM lParam, WPARAM hParam); // custom message handler
typedef void (*Updater)(struct Window *);

// struct definitions

// inspired from https://processing.org/reference#input
struct Runners
{
    // event handlers
    void (*canvasInitialised)(struct Window *); // initial set up
    void (*canvasDestroyed)(struct Window *);   // window closed

    Handler windowModalLoop; // when the user enters/exist modal loop
    Handler windowMoved;     // window has been moved

    Handler windowActivated;    // when the user focusses on window (tab-alt, mouse click etc)
    Handler windowChangePrelim; // run when the user wants to change (resize/move) the window - use can change MINMAXINFO struct to whatever

    int (*windowResized)(struct Window *window, LPARAM lParam, WPARAM hParam); // window has been resized - return 1 to override default behaviour (i.e., mapping buffer to new size)

    // input handlers
    Handler keyDown; // key pressed
    Handler keyUp;   // key released

    Handler mousePressed;  // mouse pressed
    Handler mouseReleased; // mouse released
    Handler mouseMoved;    // mouse moved over window
    Handler mouseWheel;    // mouse wheel scrolled
};

struct Hotkeys
{
    int number;
    void (*hotkeyCallbacks[])(struct Window *);
};

struct Window
{
    uint32_t *pixels;

    struct
    {
        int width;
        int height;
    } size; // {x, y}

    POINT position;

    int state;

    int frameRate;
    DWORD style;
    char *title;

    Updater updateLoop; // can't use type def here because it has this struct in its definition!
    struct Runners runners;
    struct Hotkeys *hotkeys;

    HWND windowHandler;
    HDC windowDC;
    WNDCLASSEX *windowClass;
    BITMAPINFO bitmapInfo;

    void *settings; // additional data to be lobbed about

    struct Window *prevWindow; // previous window in the linked list
    struct Window *nextWindow; // next window
};

// function declarations

// Primary functions
struct Window *CreateCanvas(int width, int height, int style, char *title, int frameRate, Updater updateLoop, struct Runners runners, struct Window *windowList);
int KindlyBegin(struct Window *window); // CPR

// Linked list traversal functions
struct Window *GetHead(struct Window *window);
struct Window *GetTail(struct Window *window);

void DestroyCanvas(struct Window *window); // completely destroyes window struct - pauses event loop, frees pixel buffer, closes window and destroys window struct

int PauseLoop(struct Window *window); // resume event loop timer
int StartLoop(struct Window *window); // pause event loop timer

int UpdateBuffer(struct Window *window, int newWidth, int newHeight); // remaps the old buffer to the new one, preserving as much information as possible

void LogMessage(char *error);

// just so I can have the main handler in a separate file
VOID CALLBACK UpdateTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif