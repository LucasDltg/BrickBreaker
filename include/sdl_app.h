#ifndef SDLAPP_H
#define SDLAPP_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "../include/sdl_component.h"

class SDLApp {
public:
    SDLApp(int screenWidth, int screenHeight);
    ~SDLApp();

    void addComponent(std::shared_ptr<SDLComponent> component);
    void run();

private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    bool isRunning;
    std::vector<std::shared_ptr<SDLComponent>> components;

    void handleEvents();
    void render();
};

#endif
