#include "display.h"

// Central event loop
int KindlyBegin(struct Window *window)
{
    if (window->prevWindow) // there shouldn't be anything before this in the linked list
    {
        LogMessage("Nu uh! Give me the head please!");
        return 1;
    }

    MSG msg;
    struct Window *windowLooper;
    BOOL ret = 0;

    // Great overlord
    for (;;)
    {
        ret = GetMessage(&msg, NULL, 0, 0); // NULL for hwnd - gets hwnd for all of the open windows
        if (!ret && window->state & KILLED)
        {
            printf("Window destroyed\n");

            if (window->nextWindow)
            {
                windowLooper = window->nextWindow;
                free(window); // just a bit of skippin'
                window = windowLooper;
            }
            else
                break; // the king is dead
        }

        DispatchMessage(&msg);
    }

    free(window); // Voila! We're done - no memory leakin'!
    printf("Cleaned up! We're done\n");

    return msg.wParam;
}

// Main updater loop - heart of the application
VOID CALLBACK UpdateTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    struct Window *window = (struct Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA); // retrieve pointer to our window struct from the window's memory

    // Run frame updater
    window->updateLoop(window); // make sure to pass back window pointer to allow user to meddle!
    InvalidateRect(window->windowHandler, NULL, FALSE);
    UpdateWindow(window->windowHandler); // update screen's pixel buffer
}

// Event handlers - https://www.autoitscript.com/autoit3/docs/appendix/WinMsgCodes.htm
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    struct Window *window = (struct Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA); // retrieve pointer to our window struct from the window's memory

    if (!window || !window->state || !(window->state & EXISTS)) // ensure that the necessary memory has been allocated
    {
        if (msg == WM_PAINT) // by this point, we really should have access to the window buffer...
            printf("Failed to find window data - not yet initialised?");

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    switch (msg)
    {
    case WM_PAINT:
    {
        // there may be some overhead (~2%) with doing this over BitBlt but the amount of complexity it removes makes it definitely worth it!
        StretchDIBits(window->windowDC, 0, 0, window->size.width, window->size.height, 0, 0, window->size.width, window->size.height, window->pixels, &window->bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
        ValidateRect(window->windowHandler, NULL);
        break;
    }

    case WM_SIZE:
        // main reason I prevent resizing buffer/resetting dimensions is because there may be cases where I don't want buffer to be cleared when screen is minimised
        if ((window->runners.windowResized && window->runners.windowResized(window, lParam, wParam) != 0) || wParam != SIZE_MINIMIZED)
            UpdateBuffer(window, LOWORD(lParam), HIWORD(lParam));

        // whatever the shebazzle is at this point, we need to update dimensions
        window->size.width = LOWORD(lParam);
        window->size.height = HIWORD(lParam);
        break;

    case WM_HOTKEY:
        if (window->hotkeys && wParam < window->hotkeys->number) // window->hotkeys->number is length, wParam is index
            window->hotkeys->hotkeyCallbacks[wParam](window);

    case WM_GETMINMAXINFO:
        if (window->runners.windowChangePrelim)
            window->runners.windowChangePrelim(window, lParam, wParam);
        break;

    case WM_MOVE:
        if (window->runners.windowMoved)
            window->runners.windowMoved(window, lParam, wParam);

        // I store it in the struct because the win32 api required allocated memory practically each time
        window->position.x = LOWORD(lParam);
        window->position.y = HIWORD(lParam);
        break;

    case WM_ENTERSIZEMOVE:
    case WM_EXITSIZEMOVE:
        if (window->runners.windowModalLoop)
            window->runners.windowModalLoop(window, lParam, wParam);
        break;

    case WM_ACTIVATE:
        if (window->runners.windowActivated)
            window->runners.windowActivated(window, lParam, wParam);
        break;

    case WM_KEYDOWN:
        if (window->runners.keyDown)
            window->runners.keyDown(window, lParam, wParam);
        break;

    case WM_KEYUP:
        if (window->runners.keyUp)
            window->runners.keyUp(window, lParam, wParam);
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        if (window->runners.mousePressed)
            window->runners.mousePressed(window, lParam, wParam);
        break;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        if (window->runners.mouseReleased)
            window->runners.mouseReleased(window, lParam, wParam);
        break;

    case WM_MOUSEMOVE:
        if (window->runners.mouseMoved)
            window->runners.mouseMoved(window, lParam, wParam);
        break;

    case WM_MOUSEWHEEL:
        if (window->runners.mouseWheel)
            window->runners.mouseWheel(window, lParam, wParam);
        break;

    case WM_CLOSE:
        DestroyCanvas(window);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}