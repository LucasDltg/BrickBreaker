#include "../include/BrickBreaker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <SDL2/SDL.h>
#include "../include/Brick.h"

BrickBreaker::BrickBreaker()
: SDLComponent()
{}

BrickBreaker::BrickBreaker(const std::string& filename)
: SDLComponent()
{
    createBricksFromLevel(filename);
}

void BrickBreaker::handleResize()
{
    
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

    int brickWidth = surface->w / gridDimensions.first;
    int brickHeight = surface->h * BrickBreaker::BRICK_HEIGHT_LIMIT / gridDimensions.second;

    std::cout << "Brick width: " << brickWidth << ", Brick height: " << brickHeight << std::endl;

    std::string line;
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



void BrickBreaker::handleEvents(SDL_Event& event) {
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        handleResize();
    }
}

SDL_Surface* BrickBreaker::render()
{
    SDL_FillRect(surface.get(), nullptr, SDL_MapRGB(surface->format, 0, 0, 0));


    int brickWidth = surface->w / gridDimensions.first;
    int brickHeight = surface->h * BrickBreaker::BRICK_HEIGHT_LIMIT / gridDimensions.second;
    for (const auto& brick : bricks)
    {
       const std::pair<uint32_t, uint32_t> position = brick->getPosition();
       SDL_Rect rect = {(int)position.first * brickWidth, (int)position.second * brickHeight, brickWidth, brickHeight};
       SDL_FillRect(surface.get(), &rect, brick->getColor());
    //    std::cout << "Brick at (" << position.first << ", " << position.second << ") with " << rect.w << "x" << rect.h << std::endl << position.first * brickHeight << " " << position.second * brickHeight << std::endl;
    }

    return surface.get();
}
