#include <SDL2/SDL.h>
#include <iostream>
#include "../include/SDLApp.h"
#include "../include/BrickBreaker.h"
#include "../include/BrickBreakerMenu.h"

// consistence throw et cerr
// const
// public et private
// collision rectangle
// collsion bordure
// portee


int main()
{
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    SDLApp app(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    app.addComponent(std::make_shared<BrickBreakerMenu>());
    
    app.run();

    return 0;
}
