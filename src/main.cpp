#include "SDL.h"

int main(int argc, char *argv[])
{
    SDL_bool done;
    SDL_Window *window;
    SDL_Event event;                        // Declare event handle
    char* dropped_filedir;                  // Pointer for directory of dropped file

    SDL_Init(SDL_INIT_VIDEO);               // SDL2 initialization

    window = SDL_CreateWindow (  // Create a window
        "ClearSVG",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        480,
        SDL_WINDOW_OPENGL
    );

    // Check that the window was successfully made
    if (window == NULL)
    {
        // In the event that the window could not be made...
        SDL_Log("Could not create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    done = SDL_FALSE;
    while (!done) {                         // Program loop
        while (!done && SDL_PollEvent(&event)) {
            switch (event.type) {
                case (SDL_QUIT): {          // In case of exit
                    done = SDL_TRUE;
                    break;
                }

                case (SDL_DROPFILE): {      // In case if dropped file
                    dropped_filedir = event.drop.file;
                    // Shows directory of dropped file
                    SDL_ShowSimpleMessageBox(
                        SDL_MESSAGEBOX_INFORMATION,
                        "File dropped on window",
                        dropped_filedir,
                        window
                    );
                    SDL_free(dropped_filedir);    // Free dropped_filedir memory
                    break;
               }
            }
        }
        SDL_Delay(0);
    }

    SDL_DestroyWindow(window);        // Close and destroy the window

    SDL_Quit();                       // Clean up
    return 0;
}
