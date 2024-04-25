#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <memory>
#include <vector>
#include <cstdint>
#include <string>
#include <cstdlib>
#include "../include/Brick.h"
#include "../include/PowerUp.h"

Brick::Brick(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up, const std::shared_ptr<SDL_Renderer> renderer)
: _position(position), _color(color), _resistance(resistance), _max_resistance(resistance), _powerUp(initPowerUp(power_up)) 
{}

int Brick::getResistance() const
{
    return _resistance;
}

void Brick::decreaseResistance()
{
    if (_resistance > 0)
        _resistance--;
}

_Float32 Brick::getResistancePercentage() const
{
    return 1 - static_cast<_Float32>(_resistance) / static_cast<_Float32>(_max_resistance);
}

const uint32_t& Brick::getColor() const
{
    return _color;
}

std::unique_ptr<PowerUp> Brick::getPowerUp()
{
    return std::move(_powerUp);
}


BrickRectangular::BrickRectangular(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up, const std::shared_ptr<SDL_Renderer> renderer)
: Brick(position, grid_dimensions, surface_size, color, resistance, power_up, renderer)
{
    calculateVerticesWithPosition(grid_dimensions, surface_size);
}

void BrickRectangular::calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size)
{
    SDL_Color originalColor; originalColor.r = _color >> 24; originalColor.g = _color >> 16; originalColor.b = _color >> 8; originalColor.a = _color;

    std::vector<SDL_Color> colors = GenerateTintAndShadeColors(originalColor, 4);

    _vertices.clear();

    int brickWidth = surface_size.first / grid_dimensions.first;
    int brickHeight = surface_size.second / grid_dimensions.second;

    float x = _position.first * brickWidth;
    float y = _position.second * brickHeight;

    _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, colors[0], SDL_FPoint{0.0f, 0.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y}, colors[1], SDL_FPoint{1.0f, 0.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y + brickHeight}, colors[2], SDL_FPoint{1.0f, 1.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y + brickHeight}, colors[3], SDL_FPoint{0.0f, 1.0f}});
}

std::vector<SDL_Vertex> Brick::getVertices() const
{
    return _vertices;
}

std::vector<SDL_Vertex> Brick::getVerticesWithoutColor() const
{
    std::vector<SDL_Vertex> verticesWithoutColor;
    for (const auto& vertex : _vertices)
    {
        verticesWithoutColor.push_back(SDL_Vertex{vertex.position, SDL_Color{255, 255, 255, 255}, vertex.tex_coord});
    }
    return verticesWithoutColor;
}

std::vector<int32_t> BrickRectangular::getIndices() const
{
    std::vector<int32_t> indices = {0, 1, 3, 1, 2, 3};
    return indices;
}

std::pair<_Float32, _Float32> BrickRectangular::getCenter() const
{
    float x = (_vertices[0].position.x + _vertices[2].position.x) / 2.0f;
    float y = (_vertices[0].position.y + _vertices[2].position.y) / 2.0f;
    return std::pair<_Float32, _Float32>{x, y};
}

BrickTriangular::BrickTriangular(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up, const std::shared_ptr<SDL_Renderer> renderer)
: Brick(position, grid_dimensions, surface_size, color, resistance, power_up, renderer)
{
    calculateVerticesWithPosition(grid_dimensions, surface_size);
}

void BrickTriangular::calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size)
{
    SDL_Color SD; SD.r = _color >> 24; SD.g = _color >> 16; SD.b = _color >> 8; SD.a = _color;
    std::vector<SDL_Color> colors = GenerateTintAndShadeColors(SD, 3);

    _vertices.clear();

    int brickWidth = surface_size.first / grid_dimensions.first;
    int brickHeight = static_cast<int>(grid_dimensions.second) % 2 == 0 ? surface_size.second / (grid_dimensions.second / 2): surface_size.second / ((grid_dimensions.second + 1) / 2);

    if (_position.second % 2 == 0)
    {
        float x = _position.first * brickWidth;
        float y = (_position.second / 2) * brickHeight;

        _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, colors[0], SDL_FPoint{0.0f, 0.2f}});
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y}, colors[1], SDL_FPoint{1.0f, 0.2f}});
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 2, y + brickHeight}, colors[2], SDL_FPoint{0.5f, 1.0f}});
    }
    else
    {
        float x = _position.first * brickWidth;
        float y = ((_position.second - 1) / 2) * brickHeight;
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, colors[1], SDL_FPoint{0.5f, 0.0f}});
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 2, y + brickHeight}, colors[2], SDL_FPoint{1.0f, 0.8f}});
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x - brickWidth / 2, y + brickHeight}, colors[0], SDL_FPoint{0.0f, 0.8f}});
    }   
}

std::vector<int32_t> BrickTriangular::getIndices() const
{
    std::vector<int32_t> indices = {0, 1, 2};
    return indices;
}

std::pair<_Float32, _Float32> BrickTriangular::getCenter() const
{
    float x = (_vertices[0].position.x + _vertices[1].position.x + _vertices[2].position.x) / 3.0f;
    float y = (_vertices[0].position.y + _vertices[1].position.y + _vertices[2].position.y) / 3.0f;
    return std::pair<_Float32, _Float32>{x, y};
}


BrickHexagonal::BrickHexagonal(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string power_up, const std::shared_ptr<SDL_Renderer> renderer)
: Brick(position, grid_dimensions, surface_size, color, resistance, power_up, renderer)
{
    calculateVerticesWithPosition(grid_dimensions, surface_size);
}

void BrickHexagonal::calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size)
{
    SDL_Color SD = {static_cast<uint8_t>(_color >> 24), static_cast<uint8_t>(_color >> 16), static_cast<uint8_t>(_color >> 8), static_cast<uint8_t>(_color)};
    std::vector<SDL_Color> colors = GenerateTintAndShadeColors(SD, 6);

    _vertices.clear();

    float brickWidth = surface_size.first / (grid_dimensions.first * 1.5f + 0.25f);
    int brickHeight = static_cast<int>(grid_dimensions.second) % 2 == 0 ? surface_size.second / ((grid_dimensions.second) / 2 + 0.5f): surface_size.second / ((grid_dimensions.second + 1) / 2);
    float x, y;

    if(_position.second % 2 == 0)
    {
        x = _position.first * (brickWidth + brickWidth / 2);
        y = (_position.second / 2) * brickHeight;
    }
    else
    {
        x = _position.first * (brickWidth + brickWidth / 2) + 3 * brickWidth / 4;
        y = ((_position.second - 1) / 2) * brickHeight + brickHeight / 2;
    }

    _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y + brickHeight / 2}, colors[5], SDL_FPoint{0}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 4, y}, colors[0], SDL_FPoint{0}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brickWidth / 4, y}, colors[1], SDL_FPoint{0}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth, y + brickHeight / 2}, colors[2], SDL_FPoint{0}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brickWidth / 4, y + brickHeight}, colors[3], SDL_FPoint{0}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brickWidth / 4, y + brickHeight}, colors[4], SDL_FPoint{0}});
}

std::vector<int32_t> BrickHexagonal::getIndices() const
{
    std::vector<int32_t> indices = {0, 1, 5, 1, 2, 5, 2, 3, 5, 3, 4, 5};
    return indices;
}

std::pair<_Float32, _Float32> BrickHexagonal::getCenter() const
{
    float x = (_vertices[0].position.x + _vertices[1].position.x + _vertices[2].position.x + _vertices[3].position.x + _vertices[4].position.x + _vertices[5].position.x) / 6.0f;
    float y = (_vertices[0].position.y + _vertices[1].position.y + _vertices[2].position.y + _vertices[3].position.y + _vertices[4].position.y + _vertices[5].position.y) / 6.0f;
    return std::pair<_Float32, _Float32>{x, y};
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


