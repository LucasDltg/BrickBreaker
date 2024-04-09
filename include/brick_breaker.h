#ifndef BRICKBREAKER_H
#define BRICKBREAKER_H

#include "../include/sdl_component.h"

class BrickBreaker : public SDLComponent {
public:
    virtual void handleEvents(SDL_Event& event) override;
    virtual void render(SDL_Renderer* renderer) override;
};

#endif
