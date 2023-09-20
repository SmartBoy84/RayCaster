#include "display.h"
#include "synchapi.h"

MSG msg;
UINT ret;

const char g_szClassName[] = "myWindowClass";

#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif

void LogMessage(char *error)
{
    printf("%s", error);
    MessageBox(NULL, error, "Error", MB_ICONEXCLAMATION | MB_OK);
}

void LogLastError()
{
    DWORD error = GetLastError();
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
            memcpy(newPixelBuffer + (newWidth * y), window->pixels + (window->size.width * y), limitingWidth * sizeof(uint32_t)); // try to preserve as much data as possible
    }

    free(window->pixels);
    window->pixels = newPixelBuffer;

    window->bitmapInfo.bmiHeader.biWidth = newWidth;
    window->bitmapInfo.bmiHeader.biHeight = -newHeight; // make it top-down

    window->size.width = newWidth;
    window->size.height = newHeight;

    return 0;
}

struct Window *InitialisePixels(int width, int height, char *title, int frameRate, Updater updateLoop, struct Runners runners)
{
    struct Window *window = calloc(sizeof(struct Window), sizeof(uint8_t)); // FIX ME
    window->checkmark = 0xDEADBEEF;

    window->frameRate = frameRate; // milliseconds - time that should elapse in between frames
    window->updateLoop = updateLoop;
    window->runners = runners;

    HINSTANCE moduleHandle = GetModuleHandle(NULL);

    // Initialize BITMAPINFO structure
    window->bitmapInfo = (BITMAPINFO){
        .bmiHeader = (BITMAPINFOHEADER){
            .biSize = sizeof(window->bitmapInfo), // number of bytes needed for this structure
            .biPlanes = 1,                        // must be set to 1
            .biBitCount = 32,                     // 32 bits per pixel
            .biCompression = BI_RGB               // no compression whatsoever (no yucky png/jpg)
        }};

    // Initialise window class
    window->windowClass = (WNDCLASSEX){
        .cbSize = sizeof(window->windowClass),
        .style = 0,
        .cbClsExtra = 0,
        .cbWndExtra = 0,

        .hIcon = LoadIcon(NULL, IDI_APPLICATION),
        .hIconSm = LoadIcon(NULL, IDI_APPLICATION),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),

        .lpszMenuName = NULL,
        .lpszClassName = g_szClassName,

        .hInstance = moduleHandle,
        .lpfnWndProc = WndProc};

    if (!RegisterClassExA(&window->windowClass))
    {
        LogMessage("Window registration failed");
        return NULL;
    }

    window->windowHandler = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, moduleHandle, NULL);
    if (window->windowHandler == NULL)
    {
        LogMessage("Window creation failed");
        return NULL;
    }

    window->windowDC = GetDC(window->windowHandler);
    UpdateBuffer(window, width, height);
    SetWindowLongPtr(window->windowHandler, GWLP_USERDATA, (LONG_PTR)window); // add our window data so we can access it in WinProc

    return window;
}

VOID CALLBACK UpdateTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    struct Window *window = (struct Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA); // retrieve pointer to our window struct from the window's memory

    // Run frame updater
    window->updateLoop(window, msg); // make sure to pass back window pointer to allow user to meddle!
    InvalidateRect(window->windowHandler, NULL, FALSE);
    UpdateWindow(window->windowHandler); // update screen's pixel buffer
}

int StartLoop(struct Window *window)
{
    ret = SetTimer(window->windowHandler, (UINT_PTR)window, 1000 / window->frameRate, (TIMERPROC)UpdateTimerProc); // window ptr is used as id
    if (ret == 0)
        return 1;

    return 0;
}

int PauseLoop(struct Window *window)
{
    return KillTimer(window->windowHandler, (UINT_PTR)window);
}

int Start(struct Window *window)
{
    ShowWindow(window->windowHandler, SW_SHOW);

    // Heart of the application
    if (StartLoop(window))
    {
        LogMessage("[Fatal] failed to start event loop timer");
        return 1;
    }

    // If the above is the heart then the following is the lymph nodes
    while (1)
    {
        if (!window->checkmark)
            break;

        if (PeekMessageA(&msg, window->windowHandler, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Sleep(1); // good practise, I suppose - infinite loops freak me out
    }

    // release window memory
    free(window->pixels);
    ReleaseDC(window->windowHandler, window->windowDC);

    KillTimer(window->windowHandler, (UINT_PTR)window); // remember, window pointer is just used as the ID here
    free(window);

    if (window->runners.windowDestroyed)
        window->runners.windowDestroyed(window);

    return msg.wParam;
}

int Stop(struct Window *window)
{
    return DestroyWindow(window->windowHandler);
}
