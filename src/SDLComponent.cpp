#include "../include/SDLComponent.h"
#include <SDL2/SDL.h>
#include <memory>
#include <stdexcept>

SDLComponent::SDLComponent() 
    : surface(nullptr) {
    surface.reset(SDL_CreateRGBSurface(0, 100, 100, 32, 0, 0, 0, 0));
    if (!surface)
    {
        throw std::runtime_error("SDL_CreateRGBSurface failed");
    }
} 

SDLComponent::~SDLComponent()
{
    
}
