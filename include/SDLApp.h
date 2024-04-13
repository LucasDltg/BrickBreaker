#ifndef SDLAPP_H
#define SDLAPP_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "../include/SDLComponent.h"

class SDLApp {
public:
    SDLApp(int screenWidth, int screenHeight, uint32_t windowFlags = 0);
    ~SDLApp();

    void addComponent(std::shared_ptr<SDLComponent> component);
    void run();

private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    bool is_running;
    std::vector<std::shared_ptr<SDLComponent>> components;
    uint32_t last_time;
    std::pair<uint32_t, uint32_t> window_dimensions;

    void handleEvents();
    void update(uint64_t delta_time);
    void render();
};

#endif
