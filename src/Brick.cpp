#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>
#include <cstdint>
#include <string>
#include <ctime>
#include <cstdlib>
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
    SDL_Color originalColor; originalColor.r = color >> 24; originalColor.g = color >> 16; originalColor.b = color >> 8; originalColor.a = color;

    std::vector<SDL_Color> colors = GenerateTintAndShadeColors(originalColor, 4);

    vertices.clear();

    int brickWidth = surface_size.first / grid_dimensions.first;
    int brickHeight = surface_size.second / grid_dimensions.second;

    float x = position.first * brickWidth;
    float y = position.second * brickHeight;

    vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, colors[0], SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y}, colors[1], SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y + brickHeight}, colors[2], SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x, y + brickHeight}, colors[3], SDL_FPoint{0}});
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
    std::vector<SDL_Color> colors = GenerateTintAndShadeColors(SD, 3);

    vertices.clear();

    int brickWidth = surface_size.first / grid_dimensions.first;
    int brickHeight = static_cast<int>(grid_dimensions.second) % 2 == 0 ? surface_size.second / (grid_dimensions.second / 2): surface_size.second / ((grid_dimensions.second + 1) / 2);

    if (position.second % 2 == 0)
    {
        float x = position.first * brickWidth;
        float y = (position.second / 2) * brickHeight;

        vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, colors[0], SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y}, colors[1], SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 2, y + brickHeight}, colors[2], SDL_FPoint{0}});
    }
    else
    {
        float x = position.first * brickWidth;
        float y = ((position.second - 1) / 2) * brickHeight;
        vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, colors[1], SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 2, y + brickHeight}, colors[2], SDL_FPoint{0}});
        vertices.push_back(SDL_Vertex{SDL_FPoint{x - brickWidth / 2, y + brickHeight}, colors[0], SDL_FPoint{0}});
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
    SDL_Color SD = {static_cast<uint8_t>(color >> 24), static_cast<uint8_t>(color >> 16), static_cast<uint8_t>(color >> 8), static_cast<uint8_t>(color)};
    std::vector<SDL_Color> colors = GenerateTintAndShadeColors(SD, 6);

    vertices.clear();

    float brickWidth = surface_size.first / (grid_dimensions.first * 1.5f + 0.25f);
    int brickHeight = static_cast<int>(grid_dimensions.second) % 2 == 0 ? surface_size.second / ((grid_dimensions.second) / 2 + 0.5f): surface_size.second / ((grid_dimensions.second + 1) / 2);
    float x, y;

    if(position.second % 2 == 0)
    {
        x = position.first * (brickWidth + brickWidth / 2);
        y = (position.second / 2) * brickHeight;
    }
    else
    {
        x = position.first * (brickWidth + brickWidth / 2) + 3 * brickWidth / 4;
        y = ((position.second - 1) / 2) * brickHeight + brickHeight / 2;
    }

    vertices.push_back(SDL_Vertex{SDL_FPoint{x, y + brickHeight / 2}, colors[5], SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 4, y}, colors[0], SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brickWidth / 4, y}, colors[1], SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y + brickHeight / 2}, colors[2], SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brickWidth / 4, y + brickHeight}, colors[3], SDL_FPoint{0}});
    vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 4, y + brickHeight}, colors[4], SDL_FPoint{0}});
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

std::vector<SDL_Color> Brick::GenerateTintAndShadeColors(const SDL_Color& originalColor, int n)
{
    std::vector<SDL_Color> colors;
    colors.reserve(n);

    float step = 1.0f / n;

    for (int i = n/2; i >= 1; i--)
    {
        SDL_Color tintedColor;
        tintedColor.r = originalColor.r + (255 - originalColor.r) * (i * step);
        tintedColor.g = originalColor.g + (255 - originalColor.g) * (i * step);
        tintedColor.b = originalColor.b + (255 - originalColor.b) * (i * step);
        tintedColor.a = originalColor.a;
        colors.push_back(tintedColor);
    }

    if (n % 2)
        colors.push_back(originalColor);

    for (int i = 1; i <= n / 2; ++i)
    {
        SDL_Color shadedColor;
        shadedColor.r = originalColor.r * ((i * step));
        shadedColor.g = originalColor.g * ((i * step));
        shadedColor.b = originalColor.b * ((i * step));
        shadedColor.a = originalColor.a;
        colors.push_back(shadedColor);
    }

    return colors;
}
