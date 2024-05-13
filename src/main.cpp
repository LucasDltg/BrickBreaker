#include <SDL2/SDL.h>
#include "../include/SDLApp.h"
#include "../include/BreakoutMenu.h"

// collision rectangle
// collsion bordure

// ajouter texture brique indestructible
// ajouter texture pages

// bug mineur avec menu, deplacement

int32_t main()
{
    const int32_t screen_width = SDLApp::_MIN_SCREEN_WIDTH;
    const int32_t screen_height = SDLApp::_MIN_SCREEN_HEIGHT;

    SDLApp app(screen_width, screen_height, SDL_WINDOW_RESIZABLE, "./assets/fonts/arial/arial.ttf");
    // app.addComponent(std::make_shared<BreakoutMenu>(), SDL_FRect{0.0f, 0.0f, 0.5f, 1.0f}, true);
    app.addComponent(std::make_shared<BreakoutMenu>(), SDL_FRect{0.0f, 0.0f, 1.0f, 1.0f}, true);
    
    app.run({}, 90);

    return 0;
}
