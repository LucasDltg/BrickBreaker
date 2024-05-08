#include <SDL2/SDL.h>
#include "../include/SDLApp.h"
#include "../include/BreakoutMenu.h"

// collision rectangle
// collsion bordure

// bug mineur avec menu, deplacement

int32_t main()
{
    const int32_t screen_width = SDLApp::_MIN_SCREEN_WIDTH;
    const int32_t screen_height = SDLApp::_MIN_SCREEN_HEIGHT;

    SDLApp app(screen_width, screen_height, SDL_WINDOW_RESIZABLE);
    app.addComponent(std::make_shared<BreakoutMenu>());
    
    app.run();

    return 0;
}
