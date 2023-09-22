#include "helpers.h"

int ShowCanvas(struct Window *window)
{
    if (!(window->state & CLOSED))
        return 1;

    ShowWindow(window->windowHandler, SW_RESTORE);
    window->state &= ~CLOSED;

    return 0;
}

int HideCanvas(struct Window *window)
{
    if (window->state & CLOSED)
        return 1;

    ShowWindow(window->windowHandler, SW_HIDE);
    window->state |= CLOSED;

    return 0;
}

int ChangeFrameRate(struct Window *window, int frameRate)
{
    if (frameRate <= 0)
        return 1;

    window->frameRate = frameRate;
    return StartLoop(window);
}

int MakeTopmost(struct Window *window, BOOL state)
{
    return SetWindowPos(window->windowHandler, state ? HWND_TOPMOST : HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

int ChangeDimensions(struct Window *window, int width, int height)
{
    if (SetWindowPos(window->windowHandler, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER))
        return 1;

    window->size.width = width;
    window->size.height = height;
    return 0;
}

int MoveCanvas(struct Window *window, int posX, int posY)
{
    return SetWindowPos(window->windowHandler, 0, posX, posY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

int StartLoop(struct Window *window)
{
    if (!(window->state & PAUSED))
        return 0;

    // window ptr is used as id
    if (SetTimer(window->windowHandler, (UINT_PTR)window, 1000 / window->frameRate, (TIMERPROC)UpdateTimerProc) == 0)
        return 1;

    window->state &= ~PAUSED;
    return 0;
}

int PauseLoop(struct Window *window)
{
    if (window->state & PAUSED)
        return 0;

    window->state |= PAUSED;
    return KillTimer(window->windowHandler, (UINT_PTR)window);
}

int AddHotkey(struct Window *window, void *callback, UINT fsModifiers, UINT vk)
{
    int number = 0;
    if (window->hotkeys)
        number = window->hotkeys->number + 1;

    if (!RegisterHotKey(window->windowHandler, number, fsModifiers, vk))
    {
        LogMessage("Failed to register hotkey");
        return 0;
    }

    window->hotkeys = realloc(window->hotkeys, sizeof(struct Hotkeys) + (number + 1) * sizeof(void (*)(void)));
    window->hotkeys->hotkeyCallbacks[number] = callback;
    window->hotkeys->number = number + 1;

    return number;
}

int RemoveHotKey(struct Window *window, int id)
{
    if (!window->hotkeys || id < 0 || id > window->hotkeys->number)
    {
        LogMessage("Invalid id or no hotkeys");
        return 1;
    }

    if (!UnregisterHotKey(window->windowHandler, id))
    {
        LogMessage("Failed to unregister hotkey");
        return 1;
    }

    memcpy(window->hotkeys->hotkeyCallbacks[id], window->hotkeys->hotkeyCallbacks[id + 1], window->hotkeys->number - id - 1);
    window->hotkeys->number--;

    window->hotkeys = realloc(window->hotkeys, sizeof(struct Hotkeys) + window->hotkeys->number * sizeof(void (*)(void)));

    return 0;
}

POINT GetRelativeCursorPos(struct Window *window)
{
    POINT cursor = {0};
    if (!GetCursorPos(&cursor))
    {
        LogMessage("Failed to get cursor pos");
        return cursor;
    }

    if (!ScreenToClient(window->windowHandler, &cursor))
        LogMessage("Failed to make point relative");

    // It happens
    cursor.x = cursor.x < 0 ? 0 : cursor.x >= window->size.width ? window->size.width - 1
                                                                 : cursor.x;
    cursor.y = cursor.y < 0 ? 0 : cursor.y >= window->size.height ? window->size.height - 1
                                                                  : cursor.y;

    return cursor;
}