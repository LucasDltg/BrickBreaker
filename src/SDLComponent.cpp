#include "../include/SDLComponent.h"
#include <SDL2/SDL.h>
#include <memory>
#include <stdexcept>

SDLComponent::SDLComponent() 
    : _surface(SDL_CreateRGBSurface(0, 0, 0, 32, 0, 0, 0, 0), SDL_FreeSurface), _is_running(true), _renderer(), _texture_manager()
{
    if (!_surface.get())
    {
        throw std::runtime_error("SDL_CreateRGBSurface failed");
    }
}

void SDLComponent::setSurfaceDimensions(uint32_t width, uint32_t height)
{
    _surface.reset(SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0), SDL_FreeSurface);
    if (!_surface.get())
    {
        throw std::runtime_error("SDL_CreateRGBSurface failed");
    }
    _texture_manager.clearTextures(); // clear the textures before deleting the renderer
    _renderer.reset(SDL_CreateSoftwareRenderer(_surface.get()), SDL_DestroyRenderer);
    if (!_renderer.get())
    {
        throw std::runtime_error("SDL_CreateSoftwareRenderer failed");
    }
    _texture_manager.updateTextures(_renderer);
}

const bool SDLComponent::isRunning() const
{
    return _is_running;
}