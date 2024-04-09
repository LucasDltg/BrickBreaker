#ifndef SDLCOMPONENT_H
#define SDLCOMPONENT_H

#include <SDL2/SDL.h>

class SDLComponent
{
public:
    virtual void handleEvents(SDL_Event& event) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
};

#endif
