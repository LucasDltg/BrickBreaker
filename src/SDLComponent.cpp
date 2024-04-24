#include "../include/SDLComponent.h"
#include <SDL2/SDL.h>
#include <memory>
#include <stdexcept>

SDLComponent::SDLComponent() 
    : surface(SDL_CreateRGBSurface(0, 0, 0, 32, 0, 0, 0, 0), SDL_FreeSurface), is_running(true), renderer(), textureManager()
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
    textureManager.clearTextures(); // clear the textures before deleting the renderer
    renderer.reset(SDL_CreateSoftwareRenderer(surface.get()), SDL_DestroyRenderer);
    if (!renderer.get())
    {
        throw std::runtime_error("SDL_CreateSoftwareRenderer failed");
    }
    textureManager.updateTextures(renderer);
}

bool SDLComponent::isRunning() const
{
    return is_running;
}