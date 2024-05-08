#include <SDL2/SDL.h>
#include <iostream>
#include "../include/SDLApp.h"
#include "../include/BrickBreaker.h"
#include "../include/BrickBreakerMenu.h"

// public et private
// portee

// collision rectangle
// collsion bordure
// fix ball leaving the screen at low fps

// bug mineur avec menu, deplacement

// final sdl_components
int32_t main()
{
    const int32_t screen_width = SDLApp::_MIN_SCREEN_WIDTH;
    const int32_t screen_height = SDLApp::_MIN_SCREEN_HEIGHT;

    SDLApp app(screen_width, screen_height, SDL_WINDOW_RESIZABLE);
    app.addComponent(std::make_shared<BrickBreakerMenu>(app.getRenderer()));
    
    app.run();

    return 0;
}
