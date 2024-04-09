#include <SDL2/SDL.h>
#include <iostream>
#include "../include/SDLApp.h"
#include "../include/BrickBreaker.h"

// consistence throw et cerr

int main()
{
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    SDLApp app(SCREEN_WIDTH, SCREEN_HEIGHT);
    app.addComponent(std::make_shared<BrickBreaker>("./assets/level1.txt"));

    app.run();

    return 0;
}
