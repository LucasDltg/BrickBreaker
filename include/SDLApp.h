#ifndef SDLAPP_H
#define SDLAPP_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "../include/SDLComponent.h"

class SDLApp {
public:
    static constexpr _Float32 _MIN_SCREEN_WIDTH = 1092;
    static constexpr _Float32 _MIN_SCREEN_HEIGHT = 600;
    
    SDLApp(const int32_t screen_width, const int32_t screen_height, const uint32_t window_flags = 0);

    void addComponent(const std::shared_ptr<SDLComponent>& component);
    void run();
    const std::shared_ptr<SDL_Window> getWindow() const;
    const std::shared_ptr<SDL_Renderer> getRenderer() const;
    

private:
    std::shared_ptr<SDL_Window> _window;
    std::shared_ptr<SDL_Renderer> _renderer;
    bool _is_running;
    std::vector<std::shared_ptr<SDLComponent>> _components;
    uint32_t _last_time;
    std::pair<uint32_t, uint32_t> _window_dimensions;

    void handleEvents();
    void update(const uint64_t delta_time);
    void render();
};

#endif
