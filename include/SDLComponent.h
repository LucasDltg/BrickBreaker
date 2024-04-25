#ifndef SDLCOMPONENT_H
#define SDLCOMPONENT_H

#include <SDL2/SDL.h>
#include <memory>
#include "../include/TextureManager.h"

class SDLComponent {
public:
    SDLComponent();
    void setSurfaceDimensions(uint32_t width, uint32_t height); // use to set the dimensions of the surface
    virtual void handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2) = 0;
    virtual void update(uint64_t delta_time) = 0;
    virtual std::shared_ptr<SDL_Surface> render() = 0;
    virtual void initSurface() = 0; // use to initialize the surface after the initial size is known
    bool isRunning() const;

protected:
    std::shared_ptr<SDL_Surface> _surface;
    bool _is_running;
    std::shared_ptr<SDL_Renderer> _renderer;
    TextureManager _textureManager;
};

#endif
