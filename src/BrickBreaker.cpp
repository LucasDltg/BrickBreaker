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
    int brickWidth = surface->w / gridDimensions.first;
    int brickHeight = surface->h / gridDimensions.second;

    for (const auto& brick : bricks)
    {
        brick->setRect(brick->getRect().x * brickWidth, brick->getRect().y * brickHeight, brickWidth, brickHeight);
    }
}

void BrickBreaker::createBricksFromLevel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << filename << std::endl;
        return;
    }

    // Read and validate the magic sequence
    std::string magicSequence;
    file >> magicSequence;
    if (magicSequence != "rectangle") {
        std::cerr << "Invalid magic sequence in level file: " << filename << std::endl;
        return;
    }

    // Read dimensions of the grid from the second line of the file
    file >> gridDimensions.first >> gridDimensions.second;

    // Calculate brick width and height based on grid dimensions and surface dimensions
    int brickWidth = surface->w / gridDimensions.first;
    int brickHeight = surface->h / gridDimensions.second;

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

        // Convert color from hexadecimal string to SDL_Color
        SDL_Color color;
        std::stringstream(colorHex) >> std::hex >> color.r >> color.g >> color.b >> color.a;

        // Create brick at adjusted position and add to vector
        bricks.push_back(std::make_unique<Brick>(posX * brickWidth, posY * brickHeight, brickWidth, brickHeight, color, resistance));
    }

    file.close();
}



void BrickBreaker::handleEvents(SDL_Event& event) {
    // handle resize
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        handleResize();
    }
}

SDL_Surface* BrickBreaker::render()
{
    // clear the surface
    SDL_FillRect(surface.get(), nullptr, SDL_MapRGB(surface->format, 0, 0, 0));

    // draw bricks on the surface
    for (const auto& brick : bricks)
    {
       SDL_FillRect(surface.get(), &brick->getRect(), SDL_MapRGB(surface->format, brick->getColor().r, brick->getColor().g, brick->getColor().b));
    }

    return surface.get();
}
