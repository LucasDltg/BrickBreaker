#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include "../include/BrickBreakerMenu.h"

BrickBreakerMenu::BrickBreakerMenu(std::string directory_path)
:  selectedLevel(0), num_rows(3), num_columns(3), brickBreaker(nullptr), background(nullptr), font(nullptr, nullptr)
{
    font = std::unique_ptr<TTF_Font, void(*)(TTF_Font*)>(TTF_OpenFont("./assets/fonts/arial/arial.ttf", 30), TTF_CloseFont);
    if (!font.get())
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
        if (entry.is_regular_file()) {
            if(entry.path().extension() != ".lvl")
                continue;
            
            levels.push_back({entry.path().string()});
            
            // check first line of file to find path to image
            std::ifstream file(entry.path().string());
            if (file.is_open())
            {
                std::string line;
                if (std::getline(file, line))
                {
                    levels.back().surface = std::shared_ptr<SDL_Surface>(IMG_Load(line.c_str()), SDL_FreeSurface);
                    if (!levels.back().surface)
                    {
                        std::cerr << "Failed to load image for level " << entry.path().string() << ": " << IMG_GetError() << std::endl;
                    }
                }
            }
            file.close();
        }
    }

    if(levels.empty())
    {
        std::cerr << "No levels found in " << directory_path << std::endl;
        return;
    }
    srand(time(nullptr));
}

void BrickBreakerMenu::handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize)
{
    for (auto& level : levels)
    {
        level.rect.x = level.rect.x * newSize.first / previousSize.first;
        level.rect.y = level.rect.y * newSize.second / previousSize.second;
        level.rect.w = level.rect.w * newSize.first / previousSize.first;
        level.rect.h = level.rect.h * newSize.second / previousSize.second;
    }
}

void BrickBreakerMenu::handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2)
{
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        handleResize({*(int*)data1.get(), *(int*)data2.get()}, {event.window.data1, event.window.data2});
    }

    if (brickBreaker != nullptr)
    {
        brickBreaker->handleEvents(event, data1, data2);
        return;
    }

    if (event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_RETURN:
                brickBreaker = std::make_unique<BrickBreaker>(levels[selectedLevel].path);
                brickBreaker->initSurface(surface->w, surface->h);
                break;
            case SDLK_LEFT:
                selectedLevel = (selectedLevel + levels.size() - 1) % levels.size();
                break;
            case SDLK_RIGHT:
                selectedLevel = (selectedLevel + 1) % levels.size();
                break;
            case SDLK_UP:
                selectedLevel = (selectedLevel + levels.size() - num_columns) % levels.size();
                break;
            case SDLK_DOWN:
                selectedLevel = (selectedLevel + num_columns) % levels.size();
                break;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int x = event.button.x;
        int y = event.button.y;

        for (const auto& level : levels) {
            if (x >= level.rect.x && x <= level.rect.x + level.rect.w && y >= level.rect.y && y <= level.rect.y + level.rect.h) {
                brickBreaker = std::make_unique<BrickBreaker>(level.path);
                brickBreaker->initSurface(surface->w, surface->h);
                break;
            }
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        int x = event.motion.x;
        int y = event.motion.y;

        for (size_t i = 0; i < levels.size(); ++i) {
            if (x >= levels[i].rect.x && x <= levels[i].rect.x + levels[i].rect.w && y >= levels[i].rect.y && y <= levels[i].rect.y + levels[i].rect.h) {
                selectedLevel = i;
                break;
            }
        }
    }
}

void BrickBreakerMenu::update(uint64_t delta_time)
{
    if (brickBreaker != nullptr)
    {
        brickBreaker->update(delta_time);
        if (!brickBreaker->isRunning())
        {
            brickBreaker = nullptr;
            reloadBackground();
        }
    }
    else // update background with an automated platform
    {
        background->update(delta_time);
        
        const SDL_Rect& platformRect = background->getPlatform().getRect();
        const Ball& ball = background->getBalls().front();
        _Float32 initialPlatformSpeed = background->getInitialPlatformSpeed();
        
        if (ball.getCenter().first < platformRect.x)
            background->getPlatform().setSpeedX(-initialPlatformSpeed);
        else if (ball.getCenter().first > (platformRect.x + platformRect.w))
            background->getPlatform().setSpeedX(initialPlatformSpeed);


        if (!background->isRunning())
        {
            reloadBackground();
        }
    }
}

std::shared_ptr<SDL_Surface> BrickBreakerMenu::render()
{
    if (brickBreaker != nullptr)
    {
        return brickBreaker->render();
    }

    SDL_FillRect(surface.get(), nullptr, SDL_MapRGB(surface->format, 0, 0, 0));
    if (background != nullptr)
    {
        std::shared_ptr<SDL_Surface> backgroundSurface = background->render();
        SDL_BlitSurface(backgroundSurface.get(), nullptr, surface.get(), nullptr);
    }

    SDL_Rect selectedRect = levels[selectedLevel].rect;
    uint32_t padding = getPadding();
    selectedRect.x -= padding / 16; selectedRect.y -= padding / 16;
    selectedRect.w += padding / 8;  selectedRect.h += padding / 8;
    SDL_FillRect(surface.get(), &selectedRect, SDL_MapRGB(surface->format, 0, 0, 255));

    for (const auto& level : levels)
    {
        if (level.surface)
        {
            SDL_Rect imageRect = {level.rect.x, level.rect.y, level.rect.w, level.rect.h};
            SDL_BlitScaled(level.surface.get(), nullptr, surface.get(), &imageRect);
        }
        else
            SDL_FillRect(surface.get(), &level.rect, SDL_MapRGB(surface->format, 255, 255, 255));

        SDL_Color textColor = {0, 0, 0};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font.get(), level.name.c_str(), textColor);
        SDL_Rect textRect = {level.rect.x + level.rect.w / 2 - textSurface->w / 2, level.rect.y + level.rect.h / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
        SDL_BlitSurface(textSurface, nullptr, surface.get(), &textRect);
        SDL_FreeSurface(textSurface);
    }

    return surface;
}

void BrickBreakerMenu::initSurface(uint32_t width, uint32_t height) {
    setSurfaceDimensions(width, height);
    uint32_t padding = getPadding();

    const int totalWidth = width - 2 * padding;
    const int totalHeight = height - 2 * padding;
    const int rectWidth = (totalWidth - (num_columns - 1) * padding / 2) / num_columns;
    const int rectHeight = (totalHeight - (num_rows - 1) * padding / 2) / num_rows;

    for (size_t i = 0; i < levels.size(); ++i) {
        int row = i / num_columns;
        int col = i % num_rows;

        int x = padding + col * (rectWidth + padding / 2);
        int y = padding + row * (rectHeight + padding / 2);

        levels[i].rect = {x, y, rectWidth, rectHeight};
    }
    reloadBackground();
}

uint32_t BrickBreakerMenu::getPadding() const
{
    return surface->w / 10;
}

void BrickBreakerMenu::reloadBackground()
{
    uint32_t level = rand() % levels.size();
    background = std::make_unique<BrickBreaker>(levels[level].path);
    background->initSurface(surface->w, surface->h);
}
