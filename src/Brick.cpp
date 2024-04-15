#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>
#include "../include/Brick.h"
#include "../include/PowerUp.h"

Brick::Brick(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up)
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


BrickRectangular::BrickRectangular(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up)
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

std::vector<SDL_Vertex> Brick::getVertices() const
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

BrickTriangular::BrickTriangular(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up)
: Brick(position, grid_dimensions, surface_size, color, resistance, power_up)
{
    calculateVerticesWithPosition(grid_dimensions, surface_size);
}

void BrickTriangular::calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size)
{
    SDL_Color SD; SD.r = color >> 24; SD.g = color >> 16; SD.b = color >> 8; SD.a = color;

    vertices.clear();

    int brickWidth = surface_size.first / grid_dimensions.first;
    int brickHeight = surface_size.second / grid_dimensions.second;

    if (position.second % 2 == 0)
    {
        float x = position.first * brickWidth;
        float y = (position.second / 2) * brickHeight;

        vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 2, y + brickHeight}, SD, SDL_FPoint{0}});
    }
    else
    {
        float x = position.first * brickWidth;
        float y = ((position.second - 1) / 2) * brickHeight;
        vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 2, y + brickHeight}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x - brickWidth / 2, y + brickHeight}, SD, SDL_FPoint{0}});
    }   
}

std::vector<int32_t> BrickTriangular::getIndices() const
{
    std::vector<int32_t> indices = {0, 1, 2};
    return indices;
}

std::pair<_Float32, _Float32> BrickTriangular::getCenter() const
{
    float x = vertices[0].position.x + (vertices[1].position.x - vertices[0].position.x) / 2;
    float y = vertices[0].position.y + (vertices[2].position.y - vertices[0].position.y) / 2;
    return {x, y};
}


BrickHexagonal::BrickHexagonal(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up)
: Brick(position, grid_dimensions, surface_size, color, resistance, power_up)
{
    calculateVerticesWithPosition(grid_dimensions, surface_size);
}

void BrickHexagonal::calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size)
{
    SDL_Color SD; SD.r = color >> 24; SD.g = color >> 16; SD.b = color >> 8; SD.a = color;

    vertices.clear();

    float brickWidth = surface_size.first / (grid_dimensions.first * 1.5f + 0.25f);
    float brickHeight = surface_size.second / (grid_dimensions.second + 0.5f);

    if(position.second % 2 == 0)
    {
        float x = position.first * (brickWidth + brickWidth / 2);
        float y = (position.second / 2) * brickHeight;
        vertices.push_back(SDL_Vertex{SDL_FPoint{x, y + brickHeight / 2}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 4, y}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brickWidth / 4, y}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y + brickHeight / 2}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brickWidth / 4, y + brickHeight}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 4, y + brickHeight}, SD, SDL_FPoint{0}});
    }
    else
    {
        float x = position.first * (brickWidth + brickWidth / 2) + 3 * brickWidth / 4;
        float y = ((position.second - 1) / 2) * brickHeight + brickHeight / 2;
        vertices.push_back(SDL_Vertex{SDL_FPoint{x, y + brickHeight / 2}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 4, y}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brickWidth / 4, y}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y + brickHeight / 2}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brickWidth / 4, y + brickHeight}, SD, SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 4, y + brickHeight}, SD, SDL_FPoint{0}});
    }    
}

std::vector<int32_t> BrickHexagonal::getIndices() const
{
    std::vector<int32_t> indices = {0, 1, 5, 1, 2, 5, 2, 3, 5, 3, 4, 5};
    return indices;
}

std::pair<_Float32, _Float32> BrickHexagonal::getCenter() const
{
    float x = vertices[0].position.x + (vertices[3].position.x - vertices[0].position.x) / 2;
    float y = vertices[1].position.y + (vertices[4].position.y - vertices[1].position.y) / 2;
    return {x, y};
}

