#ifndef SDLCOMPONENT_H
#define SDLCOMPONENT_H

#include <SDL2/SDL.h>
#include <memory>

class SDLComponent {
public:
    SDLComponent();
    void setSurfaceDimensions(uint32_t width, uint32_t height);
    virtual ~SDLComponent();
    virtual void handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2) = 0;
    virtual SDL_Surface* render(uint64_t delta_time) = 0;

protected:
    std::shared_ptr<SDL_Surface> surface;
};

#endif
