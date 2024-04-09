#include "../include/BrickBreaker.h"
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include "../include/Brick.h"

BrickBreaker::BrickBreaker()
: SDLComponent()
{
    // Default constructor, do any initialization here if needed
}

BrickBreaker::BrickBreaker(const std::string& filename)
: SDLComponent()
{
    createBricksFromLevel(filename);
}

void BrickBreaker::createBricksFromLevel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << filename << std::endl;
        return;
    }

    std::string line;
    int y = 0;
    while (std::getline(file, line)) {
        int x = 0;
        for (char c : line) {
            if (c == 'X') { // Assuming 'X' represents a brick
                // Create brick at position (x, y) and add to vector
                SDL_Color color = {0xFF, 0, 0, 0xFF}; // Red color
                int WIDTH = 50;
                int HEIGHT = 20;
                bricks.push_back(std::make_unique<Brick>(x, y, WIDTH, HEIGHT, color, 3));
            }
            x += Brick::WIDTH; // Adjust x position for next brick
        }
        y += Brick::HEIGHT; // Move to next row
    }

    file.close();
}

void BrickBreaker::handleEvents(SDL_Event& event) {
    // Handle events specific to the BrickBreaker component
}

SDL_Surface* BrickBreaker::render()
{
    // clear the surface
    SDL_FillRect(surface.get(), nullptr, SDL_MapRGB(surface->format, 0, 0, 0));

    for (const auto& brick : bricks)
    {
       SDL_FillRect(surface.get(), &brick->getRect(), SDL_MapRGB(surface->format, brick->getColor().r, brick->getColor().g, brick->getColor().b));
    }

    return surface.get();
}
