#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include "../include/BrickBreakerMenu.h"

BrickBreakerMenu::BrickBreakerMenu(std::string directory_path)
:  selectedLevel(0), num_rows(3), num_columns(3), brickBreaker(nullptr), background(nullptr), current_page(0), font(nullptr, nullptr)
{
    font = std::unique_ptr<TTF_Font, void(*)(TTF_Font*)>(TTF_OpenFont("./assets/fonts/arial/arial.ttf", getFontSize()), TTF_CloseFont);
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
    num_pages = levels.size() / num_rows / num_columns + (levels.size() % (num_rows * num_columns) != 0);
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
    TTF_SetFontSize(font.get(), getFontSize());
}

void BrickBreakerMenu::handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2)
{
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        handleResize({*(int*)data1.get(), *(int*)data2.get()}, {event.window.data1, event.window.data2});
        background->handleEvents(event, data1, data2);
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
                std::cout << "Selected level: " << selectedLevel << " " << current_page << std::endl;
                brickBreaker = std::make_unique<BrickBreaker>(levels[selectedLevel + current_page * num_rows * num_columns].path);
                brickBreaker->initSurface(surface->w, surface->h);
                break;
            case SDLK_LEFT:
                if(static_cast<int>(selectedLevel) % num_columns == 0)
                {
                    current_page = (current_page + num_pages - 1) % num_pages;
                    selectedLevel = num_columns * selectedLevel / num_columns + num_columns - 1;
                }
                else
                    selectedLevel -= 1;
                break;
            case SDLK_RIGHT:
                if(selectedLevel % num_columns == num_columns - 1)
                {
                    current_page = (current_page + 1) % num_pages;
                    selectedLevel = num_columns * selectedLevel / num_columns - num_columns + 1;
                }
                else if (selectedLevel + current_page * num_rows * num_columns < levels.size() - 1)
                    selectedLevel += 1;
                
                break;
            case SDLK_UP:
                selectedLevel = (selectedLevel + num_columns * (num_rows - 1)) % (num_rows * num_columns);
                while (selectedLevel + current_page * num_rows * num_columns >= levels.size())
                    selectedLevel -= num_columns;
                break;
            case SDLK_DOWN:                
                selectedLevel = (selectedLevel + num_columns) % (num_rows * num_columns);
                while (selectedLevel + current_page * num_rows * num_columns >= levels.size())
                    selectedLevel -= num_columns;
            break;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int x = event.button.x;
        int y = event.button.y;

        // check if a level was clicked
        for (const auto& level : levels) {
            if (x >= level.rect.x && x <= level.rect.x + level.rect.w && y >= level.rect.y && y <= level.rect.y + level.rect.h) {
                brickBreaker = std::make_unique<BrickBreaker>(level.path);
                brickBreaker->initSurface(surface->w, surface->h);
                break;
            }
        }

        // check if a page was clicked
        if(y >= static_cast<int32_t>(surface->h - getPadding() / 2))
        {
            int totalWidth = num_pages * getPadding() / 4 + (num_pages - 1) * getPadding() / 2;
            int startX = surface->w / 2 - totalWidth / 2;
            for (size_t i(0); i < num_pages; ++i)
            {
                SDL_Rect pageRect = {
                    static_cast<int32_t>(startX + i * getPadding() / 2 + i * getPadding() / 4),
                    static_cast<int32_t>(surface->h - getPadding() / 2),                                    
                    static_cast<int32_t>(getPadding() / 4),
                    static_cast<int32_t>(getPadding() / 4)};
                if (x >= pageRect.x && x <= pageRect.x + pageRect.w)
                {
                    current_page = i;
                    selectedLevel = 0;
                    break;
                }
            }
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        int x = event.motion.x;
        int y = event.motion.y;

        for (size_t i(current_page * num_rows * num_columns); i < levels.size() && i < num_rows * num_columns * (current_page + 1); ++i)
        {
            if (x >= levels[i].rect.x && x <= levels[i].rect.x + levels[i].rect.w && y >= levels[i].rect.y && y <= levels[i].rect.y + levels[i].rect.h) {
                selectedLevel = i;
                break;
            }
        }
    }
    else if (event.type == SDL_MOUSEWHEEL)
    {
        if (event.wheel.y > 0 || event.wheel.x > 0)
        {
            current_page = (current_page + num_pages - 1) % num_pages;
            selectedLevel = (num_rows * num_columns * current_page) + (selectedLevel % (num_rows * num_columns));
        }
        else if (event.wheel.y < 0 || event.wheel.x < 0)
        {
            current_page = (current_page + 1) % num_pages;
            selectedLevel = (num_rows * num_columns * current_page) + (selectedLevel % (num_rows * num_columns));
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
        
        const SDL_FRect& platformRect = background->getPlatform().getRect();
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

    uint32_t padding = getPadding();

    for (size_t i(num_columns * num_rows * current_page); i < levels.size() && i < num_columns * num_rows * (current_page + 1); ++i)
    {
        if (levels[i].surface)
        {
            if (i == selectedLevel + num_columns * num_rows * current_page)
            {
                SDL_FRect selectedRect = levels[selectedLevel].rect;
                SDL_Rect s_rect = {static_cast<int32_t>(selectedRect.x), static_cast<int32_t>(selectedRect.y), static_cast<int32_t>(selectedRect.w), static_cast<int32_t>(selectedRect.h)};
                s_rect.x -= padding / 16; s_rect.y -= padding / 16; s_rect.w += padding / 8; s_rect.h += padding / 8;
                SDL_BlitScaled(levels[i].surface.get(), nullptr, surface.get(), &s_rect);
            }   
            else
            {
                SDL_Rect imageRect = {static_cast<int32_t>(levels[i].rect.x), static_cast<int32_t>(levels[i].rect.y), static_cast<int32_t>(levels[i].rect.w), static_cast<int32_t>(levels[i].rect.h)};
                SDL_BlitScaled(levels[i].surface.get(), nullptr, surface.get(), &imageRect);
            }
        }
        else
        {
            if (i == selectedLevel + num_columns * num_rows * current_page)
            {
                SDL_FRect selectedRect = levels[selectedLevel].rect;
                SDL_Rect s_rect = {static_cast<int32_t>(selectedRect.x), static_cast<int32_t>(selectedRect.y), static_cast<int32_t>(selectedRect.w), static_cast<int32_t>(selectedRect.h)};
                s_rect.x -= padding / 16; s_rect.y -= padding / 16; s_rect.w += padding / 8; s_rect.h += padding / 8;
                SDL_FillRect(surface.get(), &s_rect, SDL_MapRGB(surface->format, 255, 255, 255));
            }   
            else
            {
                SDL_Rect s_rect = {static_cast<int32_t>(levels[i].rect.x), static_cast<int32_t>(levels[i].rect.y), static_cast<int32_t>(levels[i].rect.w), static_cast<int32_t>(levels[i].rect.h)};
                SDL_FillRect(surface.get(), &s_rect, SDL_MapRGB(surface->format, 255, 255, 255));
            }
        }

        SDL_Color textColor = {0, 0, 0};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font.get(), levels[i].name.c_str(), textColor);
        SDL_Rect textRect = {static_cast<int32_t>(levels[i].rect.x + levels[i].rect.w / 2 - textSurface->w / 2), static_cast<int32_t>(levels[i].rect.y + levels[i].rect.h / 2 - textSurface->h / 2), textSurface->w, textSurface->h};
        SDL_BlitSurface(textSurface, nullptr, surface.get(), &textRect);
        SDL_FreeSurface(textSurface);
    }

    // draw squares for pages numbers
    int totalWidth = num_pages * padding / 4 + (num_pages - 1) * padding / 2;
    int startX = surface->w / 2 - totalWidth / 2;
    for (size_t i(0); i < num_pages; ++i)
    {
        SDL_Rect pageRect = {
            static_cast<int32_t>(startX + i * padding / 2 + i * padding / 4),
            static_cast<int32_t>(surface->h - padding / 2),                                    
            static_cast<int32_t>(padding / 4),
            static_cast<int32_t>(padding / 4)};
        SDL_FillRect(surface.get(), &pageRect, SDL_MapRGB(surface->format, 255, 255, 255));
        if (i == current_page)
            SDL_FillRect(surface.get(), &pageRect, SDL_MapRGB(surface->format, 0, 0, 255));
    }

    return surface;
}

void BrickBreakerMenu::initSurface(uint32_t width, uint32_t height) {
    setSurfaceDimensions(width, height);
    uint32_t padding = getPadding();

    const _Float32 totalWidth = width - 2 * padding;
    const _Float32 totalHeight = height - 2 * padding;
    const _Float32 rectWidth = (totalWidth - (num_columns - 1) * padding / 2) / num_columns;
    const _Float32 rectHeight = (totalHeight - (num_rows - 1) * padding / 2) / num_rows;

    for (size_t i(0); i < levels.size(); ++i)
    {
        int32_t row = (i % (num_rows * num_columns)) / num_columns;
        int32_t col = (i % (num_rows * num_columns)) % num_columns;

        _Float32 x = padding + col * (rectWidth + padding / 2);
        _Float32 y = padding + row * (rectHeight + padding / 2);

        levels[i].rect = {x, y, rectWidth, rectHeight};
    }
    reloadBackground();
    TTF_SetFontSize(font.get(), getFontSize());
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

uint32_t BrickBreakerMenu::getFontSize() const
{
    return surface->w / 45;
}