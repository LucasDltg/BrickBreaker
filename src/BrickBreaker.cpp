#include "../include/BrickBreaker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <SDL2/SDL.h>
#include "../include/Brick.h"
#include "../include/Ball.h"
#include "../include/Platform.h"

BrickBreaker::BrickBreaker()
: SDLComponent()
{}

BrickBreaker::BrickBreaker(const std::string& filename)
: SDLComponent()
{
    createBricksFromLevel(filename);
}

void BrickBreaker::initSurface(uint32_t width, uint32_t height)
{
    setSurfaceDimensions(width, height);
    SDL_Color color;
    color.r = 0; color.g = 0; color.b = 255; color.a = 0;
    std::pair<uint32_t, uint32_t> center = {surface->w / 2, surface->h / 4 * 3};
    std::pair<int, int> speed = {0, -10};
    balls.push_back(std::make_unique<Ball>(25, center, color, speed));

    color.r = 255; color.g = 0; color.b = 0; color.a = 0;
    platform = std::make_unique<Platform>(surface->w / 2 - surface->w / 14, surface->h * 9 / 10, surface->w / 7, surface->h / 30, color);
}

void BrickBreaker::handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize)
{
    for (auto& ball : balls)
    {
        std::pair<uint32_t, uint32_t> center = ball->getCenter();

        uint32_t newX = static_cast<uint32_t>(static_cast<float>(center.first) * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
        uint32_t newY = static_cast<uint32_t>(static_cast<float>(center.second) * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second)));

        ball->setCenter({newX, newY});
    }

    // Resize platform
    SDL_Rect rect = platform->getRect();
    rect.x = static_cast<uint32_t>(static_cast<float>(rect.x) * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
    std::cout << rect.x << " " << newSize.first << " " << previousSize.first << std::endl;
    rect.y = surface->h * 9 / 10;
    rect.w = newSize.first / 7;
    rect.h = newSize.second / 30;
    platform->setRect(rect);
    
}

void BrickBreaker::createBricksFromLevel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << filename << std::endl;
        return;
    }

    std::string magicSequence;
    file >> magicSequence;
    if (magicSequence != "rectangle") {
        std::cerr << "Invalid magic sequence in level file: " << filename << std::endl;
        return;
    }

    file >> gridDimensions.first >> gridDimensions.second;

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int posX, posY, resistance;
        std::string colorHex;

        if (!(iss >> posX >> posY >> resistance >> colorHex)) {
            std::cerr << "Invalid line format in level file: " << filename << std::endl;
            continue;
        }

        Uint32 colorValue;
        std::stringstream(colorHex) >> std::hex >> colorValue >> std::dec;
        SDL_Color color = { (uint8_t)((colorValue >> 24) & 0xFF), (uint8_t)((colorValue >> 16) & 0xFF), (uint8_t)((colorValue >> 8) & 0xFF), (uint8_t)(colorValue & 0xFF)};

        uint32_t mappedColor = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);

        bricks.push_back(std::make_unique<Brick>(posX, posY, mappedColor, resistance));
    }

    file.close();
}


void BrickBreaker::handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2)
{
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        handleResize({*(int*)data1.get(), *(int*)data2.get()}, {event.window.data1, event.window.data2});
    }
    else if(event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_LEFT:
                platform->setSpeedX(-2);
                break;
            case SDLK_RIGHT:
                platform->setSpeedX(2);
                break;
            default:
                break;
        }
    }
    else if(event.type == SDL_KEYUP)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_LEFT:
            case SDLK_RIGHT:
                platform->setSpeedX(0);
                break;
            default:
                break;
        }
    }
    
}

SDL_Surface* BrickBreaker::render(uint64_t delta_time)
{
    platform->update(delta_time, surface->w);
    

    SDL_FillRect(surface.get(), nullptr, SDL_MapRGB(surface->format, 0, 0, 0));

    if (bricks.empty() || balls.empty() || gridDimensions.first == 0 || gridDimensions.second == 0)
    {
        return surface.get();
    }

    int brickWidth = surface->w / gridDimensions.first;
    int brickHeight = surface->h * BrickBreaker::BRICK_HEIGHT_LIMIT / gridDimensions.second;
    for (const auto& brick : bricks)
    {
       const std::pair<uint32_t, uint32_t> position = brick->getPosition();
       SDL_Rect rect = {(int)position.first * brickWidth, (int)position.second * brickHeight, brickWidth, brickHeight};
       SDL_FillRect(surface.get(), &rect, brick->getColor());

    }

    for (const auto& ball : balls)
    {
        const std::pair<uint32_t, uint32_t> position = ball->getCenter();
        float radius = ball->getRadius();
        SDL_Rect rect = {static_cast<int>(position.first - radius / 2), static_cast<int>(position.second - radius / 2), static_cast<int>(radius), static_cast<int>(radius)};
        SDL_Color color = ball->getColor();
        SDL_FillRect(surface.get(), &rect, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
    }

    SDL_Color color = platform->getColor();
    SDL_FillRect(surface.get(), &(platform.get()->getRect()), SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));

    return surface.get();
}
