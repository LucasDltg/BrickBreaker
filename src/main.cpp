#include <SDL2/SDL.h>
#include <iostream>
#include "../include/SDLApp.h"
#include "../include/BrickBreaker.h"
#include "../include/BrickBreakerMenu.h"

// const
// public et private
// collision rectangle
// collsion bordure
// portee
// fix ball leaving the screen at low fps
// bug mineur avec menu, deplacement

int main()
{
    const int SCREEN_WIDTH = SDLApp::_MIN_SCREEN_WIDTH;
    const int SCREEN_HEIGHT = SDLApp::_MIN_SCREEN_HEIGHT;

    SDLApp app(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    app.addComponent(std::make_shared<BrickBreakerMenu>(app.getRenderer()));
    
    app.run();

    return 0;
}
