#include "../include/SDLComponent.h"
#include <SDL2/SDL.h>
#include <memory>
#include <stdexcept>

SDLComponent::SDLComponent() 
    : surface(SDL_CreateRGBSurface(0, 0, 0, 32, 0, 0, 0, 0), SDL_FreeSurface), is_running(true)
{
    if (!surface.get())
    {
        throw std::runtime_error("SDL_CreateRGBSurface failed");
    }
}

void SDLComponent::setSurfaceDimensions(uint32_t width, uint32_t height)
{
    surface.reset(SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0), SDL_FreeSurface);
    if (!surface.get())
    {
        throw std::runtime_error("SDL_CreateRGBSurface failed");
    }
}

bool SDLComponent::isRunning() const
{
    return is_running;
}