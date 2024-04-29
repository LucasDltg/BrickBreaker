#ifndef SDLCOMPONENT_H
#define SDLCOMPONENT_H

#include <SDL2/SDL.h>
#include <memory>
#include "../include/TextureManager.h"

class SDLComponent {
public:
    SDLComponent();
    void setSurfaceDimensions(const uint32_t width, const uint32_t height); // use to set the dimensions of the surface
    virtual void handleEvents(const SDL_Event& event, const std::shared_ptr<void>& data1, const std::shared_ptr<void>& data2) = 0;
    virtual void update(const uint64_t delta_time) = 0;
    virtual const std::shared_ptr<SDL_Surface> render() = 0;
    virtual void initSurface() = 0; // use to initialize the surface after the initial size is known
    const bool isRunning() const;

protected:
    std::shared_ptr<SDL_Surface> _surface;
    bool _is_running;
    std::shared_ptr<SDL_Renderer> _renderer;
    TextureManager _texture_manager;
};

#endif
