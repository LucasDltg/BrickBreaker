#ifndef SDLAPP_H
#define SDLAPP_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "../include/SDLComponent.h"

class SDLApp {
public:
    SDLApp(int screenWidth, int screenHeight, uint32_t windowFlags = 0);

    void addComponent(std::shared_ptr<SDLComponent> component);
    void run();
    std::shared_ptr<SDL_Window> getWindow() const;
    std::shared_ptr<SDL_Renderer> getRenderer() const;
    

private:
    std::shared_ptr<SDL_Window> _window;
    std::shared_ptr<SDL_Renderer> _renderer;
    bool _is_running;
    std::vector<std::shared_ptr<SDLComponent>> _components;
    uint32_t _last_time;
    std::pair<uint32_t, uint32_t> _window_dimensions;

    void handleEvents();
    void update(uint64_t delta_time);
    void render();
};

#endif
