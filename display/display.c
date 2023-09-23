#include "display.h"
#include "synchapi.h"
#include "helpers.h"

UINT ret;

void LogMessage(char *error)
{
    printf("%s", error);
    MessageBox(NULL, error, "Error", MB_ICONEXCLAMATION | MB_OK);
}

void LogLastError()
{
    DWORD error = GetLastError();
    if (error == 0)
    {
        printf("No error message recorded");
        return;
    }
    char *errorMsg;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, errorMsg, 0, NULL);

    if (errorMsg != NULL)
    {
        LogMessage(errorMsg);
        LocalFree(errorMsg);
    }
    else
        printf("Failed to retrieve the error message");
}

struct Window *GetHead(struct Window *window)
{
    for (; window->prevWindow; window = window->prevWindow)
        ;
    return window;
}

struct Window *GetTail(struct Window *window)
{
    for (; window->nextWindow; window = window->nextWindow)
        ;
    return window;
}

int UpdateBuffer(struct Window *window, int newWidth, int newHeight)
{
    uint32_t *newPixelBuffer = calloc(newWidth * newHeight, sizeof(uint32_t));

    if (window->pixels)
    {
        int limitingWidth = 0;
        int limitingHeight = 0;

        if (newWidth > window->size.width)
            limitingWidth = window->size.width;
        else
            limitingWidth = newWidth;

        if (newHeight > window->size.height)
            limitingHeight = window->size.height;
        else
            limitingHeight = newHeight;

        for (int y = 0; y <= limitingHeight - 1; y++)
            memcpy(
                newPixelBuffer + newWidth * y,
                window->pixels + window->size.width * y,
                limitingWidth * sizeof(uint32_t)); // try to preserve as much data as possible
    }

    free(window->pixels);
    window->pixels = newPixelBuffer;

    window->bitmapInfo.bmiHeader.biWidth = newWidth;
    window->bitmapInfo.bmiHeader.biHeight = -newHeight; // make it top-down

    window->size.width = newWidth;
    window->size.height = newHeight;

    return 0;
}

struct Window *CreateCanvas(int width, int height, int style, char *title, int frameRate, Updater updateLoop, struct Runners runners, struct Window *windowList)
{
    if (windowList && !(GetHead(windowList)->state & BIRTHED))
    {
        LogMessage("Malformed linked list provided");
        return NULL;
    }

    struct Window *window = calloc(sizeof(struct Window), sizeof(uint8_t));
    HINSTANCE moduleHandle = GetModuleHandle(NULL);

    // fill  up the window struct
    window->state |= (PAUSED | BIRTHED);
    window->title = title;

    window->frameRate = frameRate; // milliseconds - time that should elapse in between frames
    window->style = style;

    window->updateLoop = updateLoop;
    window->runners = runners;

    window->size.width = width;
    window->size.height = height;

    // Initialize BITMAPINFO structure
    window->bitmapInfo = (BITMAPINFO){
        .bmiHeader = (BITMAPINFOHEADER){
            .biSize = sizeof(window->bitmapInfo), // number of bytes needed for this structure
            .biPlanes = 1,                        // must be set to 1
            .biBitCount = 32,                     // 32 bits per pixel
            .biCompression = BI_RGB               // no compression whatsoever (no yucky png/jpg)
        }};

    // Initialise window class
    if (!windowList) // if not given the head, then create a class for this "window clump"
    {
        WNDCLASSEX *windowClass = calloc(1, sizeof(WNDCLASSEX));
        windowClass->cbSize = sizeof(WNDCLASSEX);

        windowClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
        windowClass->hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        windowClass->hCursor = LoadCursor(NULL, IDC_ARROW);

        windowClass->hInstance = moduleHandle;
        windowClass->lpszClassName = window->title;

        windowClass->lpfnWndProc = WndProc;

        if (!RegisterClassExA(windowClass))
        {
            LogMessage("Window class registration failed");
            return windowList;
        }

        window->windowClass = windowClass;
    }
    else
        window->windowClass = GetHead(windowList)->windowClass; // copy over this clump's class

    // create the window
    if (windowList)
    {
        window->prevWindow = GetTail(windowList);
        window->prevWindow->nextWindow = window; // pop on to linked list so that event loop starts running this as well
    }

    window->windowHandler = CreateWindowA(window->windowClass->lpszClassName,
                                          window->title,                            // set title
                                          window->style | WS_VISIBLE,               // set styling
                                          window->position.x, window->position.y,   // center the screen
                                          window->size.width, window->size.height,  // set dimensions
                                          NULL, NULL, GetModuleHandle(NULL), NULL); // other shizzle

    if (window->windowHandler == NULL)
    {
        LogMessage("Window creation failed");
        return windowList;
    }

    window->windowDC = GetDC(window->windowHandler);
    UpdateBuffer(window, window->size.width, window->size.height);
    SetWindowLongPtr(window->windowHandler, GWLP_USERDATA, (LONG_PTR)window); // add our window data so we can access it in WinProc

    MakeCenter(window); // start in the center then the user can do whatever they want in initialiser routines

    window->runners.canvasInitialised(window);

    if (StartLoop(window))
    {
        LogMessage("[Fatal] failed to start event loop timer");
        return windowList;
    }

    return window;
}

void DestroyCanvas(struct Window *window)
{
    if (window->runners.canvasDestroyed)
        window->runners.canvasDestroyed(window);

    window->state |= KILLED; // lets the event loop know
    DestroyWindow(window->windowHandler);

    // free whatever parts of the window memory we can/should here
    ReleaseDC(window->windowHandler, window->windowDC);
    free(window->pixels);

    if (window->prevWindow) // if this window isn't the head of the linked list
    {
        ((struct Window *)window->prevWindow)->nextWindow = (struct Window *)window->nextWindow; // pop off self pointer
        free(window);
    }
    else // is the head
    {
        if (window->nextWindow)                                       // there's another link ahead
            ((struct Window *)window->nextWindow)->prevWindow = NULL; // pop off self pointer

        UnregisterClass(window->windowClass->lpszClassName, GetModuleHandle(NULL)); // unregister the class
    }

    PostQuitMessage(0); // Finally, signal event loop that canvas has been destroyed
}