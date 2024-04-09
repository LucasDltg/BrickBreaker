#include "../include/SDLApp.h"
#include <iostream>

SDLApp::SDLApp(int screen_width, int screen_height, uint32_t flags)
    : window(nullptr, &SDL_DestroyWindow), renderer(nullptr, &SDL_DestroyRenderer), is_running(false), last_time(SDL_GetTicks64())
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return;
    }

    window.reset(SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  screen_width, screen_height, SDL_WINDOW_SHOWN | flags));
    if (!window)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED));
    if (!renderer)
    {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    is_running = true;
    window_dimensions = {screen_width, screen_height};
}

SDLApp::~SDLApp()
{
    SDL_Quit();
}


void SDLApp::addComponent(std::shared_ptr<SDLComponent> obj)
{
    // a changer pour pouvoir afficher plusieurs objets
    components.push_back(obj);
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window.get(), &windowWidth, &windowHeight);
    components.back()->setSurfaceDimensions(windowWidth, windowHeight);
}

void SDLApp::run()
{
    // create a resize event to forward their size to the components
    SDL_Event resizeEvent;
    resizeEvent.type = SDL_WINDOWEVENT;
    resizeEvent.window.windowID = SDL_GetWindowID(window.get());
    resizeEvent.window.event = SDL_WINDOWEVENT_RESIZED;
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window.get(), &windowWidth, &windowHeight);
    resizeEvent.window.data1 = windowWidth;
    resizeEvent.window.data2 = windowHeight;
    SDL_PushEvent(&resizeEvent);

    while (is_running) 
    {
        handleEvents();
        render(SDL_GetTicks64() - last_time);
        last_time = SDL_GetTicks64();
    }
}

void SDLApp::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
        {
            is_running = false;
        }
        else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) // a changer pour pouvoir afficher plusieurs objets
        {            
            // on change la taille ici, pour communiquer la nouvelle taille aux objets (ils sont libres de la garder ou non)
            for (auto& obj : components)
            {
                obj->setSurfaceDimensions(event.window.data1, event.window.data2);
                // on envoie la taille précédente et la nouvelle taille
                obj->handleEvents(event, std::make_shared<int>(window_dimensions.first), std::make_shared<int>(window_dimensions.second)); 
            }
            int window_width, window_height;
            SDL_GetWindowSize(window.get(), &window_width, &window_height);
            window_dimensions = {window_width, window_height};
        }
        else
        {
            for (auto& obj : components)
            {
                obj->handleEvents(event, nullptr, nullptr);
            }
        }
    }
}

// adaptable facilement pour pouvoir afficher plusieurs objets, besoin de changer addComponent et resize dans handleEvents
void SDLApp::render(uint64_t delta_time)
{
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    for (auto& obj : components)
    {
        SDL_Surface* surface = obj->render(delta_time);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.get(), surface);
        if(!texture)
        {
            std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
            continue;
        }
        SDL_Rect destRect = {0, 0, surface->w, surface->h};
        SDL_RenderCopy(renderer.get(), texture, nullptr, &destRect);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer.get());
}
