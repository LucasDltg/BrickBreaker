#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>
#include "../include/Brick.h"
#include "../include/PowerUp.h"

Brick::Brick(std::pair<_Float32, _Float32> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up)
: position(position), color(color), resistance(resistance), powerUp(initPowerUp(power_up)) 
{}

int Brick::getResistance() const
{
    return resistance;
}

void Brick::decreaseResistance()
{
    if (resistance > 0)
        resistance--;
}

void Brick::setVertices(std::vector<SDL_Vertex> vertices)
{
    this->vertices = vertices;
}

const uint32_t& Brick::getColor() const
{
    return color;
}

std::unique_ptr<PowerUp> Brick::getPowerUp()
{
    return std::move(powerUp);
}


BrickRectangular::BrickRectangular(std::pair<_Float32, _Float32> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up)
: Brick(position, grid_dimensions, surface_size, color, resistance, power_up)
{
    calculateVerticesWithPosition(grid_dimensions, surface_size);
}

void BrickRectangular::calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size)
{
    SDL_Color SD; SD.r = color >> 24; SD.g = color >> 16; SD.b = color >> 8; SD.a = color;

    vertices.clear();

    int brickWidth = surface_size.first / grid_dimensions.first;
    int brickHeight = surface_size.second / grid_dimensions.second;

    float x = position.first * brickWidth;
    float y = position.second * brickHeight;

    vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, SD, SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y}, SD, SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y + brickHeight}, SD, SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x, y + brickHeight}, SD, SDL_FPoint{0}});
}

std::vector<SDL_Vertex> BrickRectangular::getVertices() const
{
    return vertices;
}

std::vector<int32_t> BrickRectangular::getIndices() const
{
    std::vector<int32_t> indices = {0, 1, 3, 1, 2, 3};
    return indices;
}

std::pair<_Float32, _Float32> BrickRectangular::getCenter() const
{
    float x = vertices[0].position.x + (vertices[1].position.x - vertices[0].position.x) / 2;
    float y = vertices[0].position.y + (vertices[3].position.y - vertices[0].position.y) / 2;
    return {x, y};
}