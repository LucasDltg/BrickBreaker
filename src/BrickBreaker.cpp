#include "../include/BrickBreaker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <SDL2/SDL.h>
#include "../include/Brick.h"
#include "../include/Ball.h"

BrickBreaker::BrickBreaker()
: SDLComponent()
{}

BrickBreaker::BrickBreaker(const std::string& filename)
: SDLComponent()
{
    createBricksFromLevel(filename);
    SDL_Color color;
    color.r = 0; color.g = 0; color.b = 255; color.a = 0;
    std::pair<uint32_t, uint32_t> center = {surface->w / 2, surface->h / 4 * 3};
    std::pair<int, int> speed = {0, -10};
    balls.push_back(std::make_unique<Ball>(25, center, color, speed));
}

void BrickBreaker::handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize)
{
    for (auto& ball : balls)
    {
        ball->setCenter({newSize.first / 2, newSize.second / 4 * 3});
    }
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
}

SDL_Surface* BrickBreaker::render(uint64_t delta_time)
{
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
        SDL_Rect rect = {(int)position.first, (int)position.second, (int)ball->getRadius(), (int)ball->getRadius()};
        SDL_Color color = ball->getColor();
        SDL_FillRect(surface.get(), &rect, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
    }


    return surface.get();
}
