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
// fix ball leaving the screen at low fps
// change platform rect to have float values
// nom de variable
// bug mineur avec menu, deplacement

int main()
{
    const int SCREEN_WIDTH = 1092;
    const int SCREEN_HEIGHT = 600;

    SDLApp app(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    app.addComponent(std::make_shared<BrickBreakerMenu>());
    
    app.run();

    return 0;
}
