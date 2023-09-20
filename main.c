#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "display.h"
#include "helpers.h"

clock_t timer = 0;

// tbd - allow resizing, input handlers

void LogTime()
{
    printf("%d ms\n", clock() - timer);
    timer = clock();
}

void Update(struct Window *window, MSG msg)
{
    // LogTime();

    window->pixels[(Rand32() % (window->size.width * window->size.height)) - 1] = Rand32();
    // ChangePixel(window, rand() % (window->width - 1), rand() % (window->height - 1), rand() % 255, rand() % 255, rand() % 255);
    // SetScreen(window, 90, 100, 210);
}

void WindowDestroyed(struct Window *window)
{
    printf("Closed!");
}

void WindowActivated(struct Window *window)
{
    if (window->states.active)
    {
        printf("Activated\n");
        StartLoop(window);
    }
    else
    {
        printf("Deactivated\n");
        PauseLoop(window);
    }
}

void ModalLoop(struct Window *window)
{
    if (!window->states.modalLoop)
    {
        printf("Left modal loop\n");
        ChangeFrameRate(window, 100); // if it as stopped
    }
}

void WindowMoved(struct Window *window)
{
    printf("Window moved to (x, y) -> (%d, %d) %d\n", window->position.x, window->position.y, window->states.moving);
    PauseLoop(window);
}

int WindowResized(struct Window *window, int newWidth, int newHeight)
{
    // experiment with centering buffer in screen
    printf("Window resized: width: %d, height: %d\n", newWidth, newHeight);

    if (window->states.maximised)
    {
        printf("MAXIMISED!\n");
        PauseLoop(window);
    }
    else if (window->states.minimised)
    {
        printf("MINIMISED!\n");
        PauseLoop(window);
        return 1;
    }
    else
        StartLoop(window);
    return 0;
}

void windowChangePrelim(struct Window *window, MINMAXINFO *info)
{
    PauseLoop(window);
    info->ptMaxPosition.x = 1000;
    // info->ptMaxPosition.y = 1000;
    printf("Being changed");
}

// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

int main()
{
    // add support for mulitple windows using linked lists of the window objects - should be pretty easy with this design
    // figure out dimension restriction function causing program to crash
    // have some fun with centering buffer using the resize override
    // implement input
    // $$$ -> ray casting time

    Start(InitialisePixels(300, 300, "Hello world!", 1, &Update, (struct Runners){
                                                                     //  .windowDestroyed = &WindowDestroyed,
                                                                     //  .windowActivated = &WindowActivated,
                                                                     //  .windowMoved = &WindowMoved,
                                                                     .windowChangePrelim = &windowChangePrelim,
                                                                     //  .windowResized = &WindowResized,
                                                                     //  .windowModalLoop = &ModalLoop,
                                                                 }));
}