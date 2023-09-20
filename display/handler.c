#include "display.h"

// codes from here https://www.autoitscript.com/autoit3/docs/appendix/WinMsgCodes.htm

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    struct Window *window = (struct Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA); // retrieve pointer to our window struct from the window's memory

    if (!window || !window->checkmark || window->checkmark != 0xDEADBEEF) // ensure that the necessary memory has been allocated
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
    {
        window->states.resizing = TRUE;

        window->states.maximised = FALSE;
        window->states.minimised = FALSE;

        if (wParam == SIZE_MAXIMIZED)
            window->states.maximised = TRUE;

        if (wParam == SIZE_MINIMIZED)
            window->states.minimised = TRUE;

        // main reason I prevent resizing buffer/resetting dimensions is because there may be cases where I don't want buffer to be cleared when screen is minimised
        if (!window->runners.windowResized || window->runners.windowResized(window, LOWORD(lParam), HIWORD(lParam)) == 0)
            UpdateBuffer(window, LOWORD(lParam), HIWORD(lParam));
    }

    case WM_GETMINMAXINFO:
    {
        if (window->runners.windowChangePrelim)
            window->runners.windowChangePrelim(window, (MINMAXINFO *)lParam);
        break;
    }

    case WM_MOVE:
    {
        window->states.moving = TRUE;

        window->position.x = LOWORD(lParam);
        window->position.y = HIWORD(lParam);

        if (window->runners.windowMoved)
            window->runners.windowMoved(window);

        break;
    }

    case WM_ENTERSIZEMOVE:
    {
        window->states.modalLoop = TRUE;

        if (window->runners.windowModalLoop)
            window->runners.windowModalLoop(window);

        break;
    }

    case WM_EXITSIZEMOVE:
    {
        window->states.modalLoop = FALSE;

        window->states.resizing = FALSE;
        window->states.moving = FALSE;

        if (window->runners.windowModalLoop)
            window->runners.windowModalLoop(window);

        break;
    }

    case WM_ACTIVATE:
        window->states.active = LOWORD(wParam) > 0;

        if (window->runners.windowActivated)
            window->runners.windowActivated(window);

        break;

    case WM_CLOSE:
    {
        PostQuitMessage(0);
        window->checkmark = 0;
        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}