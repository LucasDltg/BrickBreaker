#include "../include/SDLApp.h"
#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>

SDLApp::SDLApp(int screen_width, int screen_height, uint32_t flags)
    : _window(nullptr, SDL_DestroyWindow), _renderer(nullptr, SDL_DestroyRenderer), _is_running(false), _last_time(0)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        throw std::runtime_error("SDL initialization failed: " + std::string(SDL_GetError()));
    
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags))
        throw std::runtime_error("SDL_image initialization failed: " + std::string(IMG_GetError()));

    if(TTF_Init() == -1)
        throw std::runtime_error("SDL_ttf initialization failed: " + std::string(TTF_GetError()));

    _window.reset(SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  screen_width, screen_height, SDL_WINDOW_SHOWN | flags), SDL_DestroyWindow);
    if (!_window.get())
        throw std::runtime_error("Window creation failed: " + std::string(SDL_GetError()));

    _renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer);
    if (!_renderer.get())
        throw std::runtime_error("Renderer creation failed: " + std::string(SDL_GetError()));

    SDL_SetRenderDrawBlendMode(_renderer.get(), SDL_BLENDMODE_BLEND);

    _is_running = true;
    _window_dimensions = {screen_width, screen_height};
}


void SDLApp::addComponent(std::shared_ptr<SDLComponent> obj)
{
    // a changer pour pouvoir afficher plusieurs objets
    _components.push_back(obj);
    
    int windowWidth, windowHeight;
    SDL_GetWindowSize(_window.get(), &windowWidth, &windowHeight);
    _components.back()->setSurfaceDimensions(windowWidth, windowHeight);
    _components.back()->initSurface();
}

void SDLApp::run()
{
    const int desired_fps = 120;
    const __uint64_t frame_time = 1000 / desired_fps;

    __uint64_t last_time = SDL_GetTicks64();
    while (_is_running)
    {
        handleEvents();
    
        __uint64_t current_time = SDL_GetTicks64();
        __uint64_t delta_time = current_time - last_time;
        last_time = current_time;

        update(delta_time);

        render();

        __uint64_t elapsed_time = SDL_GetTicks64() - current_time;
        if (elapsed_time < frame_time)
            SDL_Delay(frame_time - elapsed_time);

        // double fps = 1000.0 / (SDL_GetTicks64() - last_time);
        // std::cout << "FPS: " << fps << std::endl;
    }
}

void SDLApp::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            _is_running = false;
        }
        else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) // a changer pour pouvoir afficher plusieurs objets
        {            
            if (event.window.data1 < _MIN_SCREEN_WIDTH || event.window.data2 < _MIN_SCREEN_HEIGHT)
            {
                SDL_SetWindowSize(_window.get(), _window_dimensions.first, _window_dimensions.second);
                continue;
            }
            
            // on change la taille ici, pour communiquer la nouvelle taille aux objets (ils sont libres de la garder ou non)
            for (auto& obj : _components)
            {
                obj->setSurfaceDimensions(event.window.data1, event.window.data2);
                // on envoie la taille précédente et la nouvelle taille
                obj->handleEvents(event, std::make_shared<int>(_window_dimensions.first), std::make_shared<int>(_window_dimensions.second)); 
            }
            int window_width, window_height;
            SDL_GetWindowSize(_window.get(), &window_width, &window_height);
            _window_dimensions = {window_width, window_height};
        }
        else
        {
            for (auto& obj : _components)
            {
                obj->handleEvents(event, nullptr, nullptr);
            }
        }
    }
}

void SDLApp::update(uint64_t delta_time)
{
    for (auto& obj : _components)
    {
        obj->update(delta_time);
    }
}

// adaptable facilement pour pouvoir afficher plusieurs objets, besoin de changer addComponent et resize dans handleEvents
void SDLApp::render()
{
    SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(_renderer.get());

    for (auto& obj : _components)
    {
        std::shared_ptr<SDL_Surface> surface = obj->render();
        std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture(SDL_CreateTextureFromSurface(_renderer.get(), surface.get()), &SDL_DestroyTexture);
        if(!texture.get())
        {
            std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
            continue;
        }
        SDL_Rect dest_rect = {0, 0, surface->w, surface->h};
        SDL_RenderCopy(_renderer.get(), texture.get(), nullptr, &dest_rect);
    }
    SDL_RenderPresent(_renderer.get());
}

std::shared_ptr<SDL_Window> SDLApp::getWindow() const
{
    return _window;
}

std::shared_ptr<SDL_Renderer> SDLApp::getRenderer() const
{
    return _renderer;
}