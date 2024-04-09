#include <SDL2/SDL.h>
#include <iostream>
#include "../include/sdl_app.h"
#include "../include/brick_breaker.h"

int main()
{
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    SDLApp app(SCREEN_WIDTH, SCREEN_HEIGHT);
    app.addComponent(std::make_shared<BrickBreaker>());

    app.run();

    return 0;
}
