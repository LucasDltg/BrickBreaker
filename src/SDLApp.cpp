#include "../include/SDLApp.h"
#include <iostream>

SDLApp::SDLApp(int screenWidth, int screenHeight, uint32_t flags)
    : window(nullptr, &SDL_DestroyWindow), renderer(nullptr, &SDL_DestroyRenderer), isRunning(false)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return;
    }

    window.reset(SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  screenWidth, screenHeight, SDL_WINDOW_SHOWN | flags));
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

    isRunning = true;
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

    while (isRunning) 
    {
        handleEvents();
        render();
    }
}

void SDLApp::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
        {
            isRunning = false;
        }
        else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) // a changer pour pouvoir afficher plusieurs objets
        {
            int windowWidth, windowHeight;
            SDL_GetWindowSize(window.get(), &windowWidth, &windowHeight);
            for (auto& obj : components)
            {
                obj->setSurfaceDimensions(windowWidth, windowHeight);
            }
        }


        for (auto& obj : components)
        {
            obj->handleEvents(event);
        }
    }
}

// adaptable facilement pour pouvoir afficher plusieurs objets, besoin de changer addComponent et resize dans handleEvents
void SDLApp::render()
{
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    for (auto& obj : components)
    {
        SDL_Surface* surface = obj->render();
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
