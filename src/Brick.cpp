#include "../include/Brick.h"
#include "../include/PowerUp.h"

Brick::Brick(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name)
: _position(position), _color(color), _resistance(resistance), _max_resistance(resistance), _powerUp(initPowerUp(power_up_name)) 
{
    (void)grid_dimensions;
    (void)surface_size;
}

int32_t Brick::getResistance() const
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

uint32_t Brick::getColor() const
{
    return _color;
}

const std::unique_ptr<PowerUp> Brick::getPowerUp()
{
    return std::move(_powerUp);
}

BrickRectangular::BrickRectangular(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name)
: Brick(position, grid_dimensions, surface_size, color, resistance, power_up_name)
{
    calculateVerticesWithPosition(grid_dimensions, surface_size);
}

void BrickRectangular::calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size)
{
    SDL_Color original_color = {static_cast<uint8_t>(_color >> 24), static_cast<uint8_t>(_color >> 16), static_cast<uint8_t>(_color >> 8), static_cast<uint8_t>(_color)};

    std::vector<SDL_Color> colors = GenerateTintAndShadeColors(original_color, 4);

    _vertices.clear();

    _Float32 brick_width = surface_size.first / grid_dimensions.first;
    _Float32 brick_height = surface_size.second / grid_dimensions.second;

    _Float32 x = _position.first * brick_width;
    _Float32 y = _position.second * brick_height;

    _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, colors[0], SDL_FPoint{0.0f, 0.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brick_width, y}, colors[1], SDL_FPoint{1.0f, 0.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brick_width, y + brick_height}, colors[2], SDL_FPoint{1.0f, 1.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y + brick_height}, colors[3], SDL_FPoint{0.0f, 1.0f}});
}

const std::vector<SDL_Vertex>& Brick::getVertices() const
{
    return _vertices;
}

const std::vector<SDL_Vertex> Brick::getVerticesWithoutColor() const
{
    std::vector<SDL_Vertex> vertices_without_color;
    for (const auto& vertex : _vertices)
    {
        vertices_without_color.push_back(SDL_Vertex{vertex.position, SDL_Color{255, 255, 255, 255}, vertex.tex_coord});
    }
    return vertices_without_color;
}

const std::vector<int32_t> BrickRectangular::getIndices() const
{
    std::vector<int32_t> indices = {0, 1, 3, 1, 2, 3};
    return indices;
}

const std::pair<_Float32, _Float32> BrickRectangular::getCenter() const
{
    _Float32 x = (_vertices[0].position.x + _vertices[2].position.x) / 2.0f;
    _Float32 y = (_vertices[0].position.y + _vertices[2].position.y) / 2.0f;
    return std::pair<_Float32, _Float32>{x, y};
}

BrickTriangular::BrickTriangular(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name)
: Brick(position, grid_dimensions, surface_size, color, resistance, power_up_name)
{
    calculateVerticesWithPosition(grid_dimensions, surface_size);
}

void BrickTriangular::calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size)
{
    SDL_Color original_color = {static_cast<uint8_t>(_color >> 24), static_cast<uint8_t>(_color >> 16), static_cast<uint8_t>(_color >> 8), static_cast<uint8_t>(_color)};
    std::vector<SDL_Color> colors = GenerateTintAndShadeColors(original_color, 3);

    _vertices.clear();

    _Float32 brick_width = surface_size.first / grid_dimensions.first;
    _Float32 brick_height = static_cast<int32_t>(grid_dimensions.second) % 2 == 0 ? surface_size.second / (grid_dimensions.second / 2): surface_size.second / ((grid_dimensions.second + 1) / 2);

    if (_position.second % 2 == 0)
    {
        _Float32 x = _position.first * brick_width;
        _Float32 y = (_position.second / 2) * brick_height;

        _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, colors[0], SDL_FPoint{0.0f, 0.2f}});
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brick_width, y}, colors[1], SDL_FPoint{1.0f, 0.2f}});
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brick_width / 2, y + brick_height}, colors[2], SDL_FPoint{0.5f, 1.0f}});
    }
    else
    {
        _Float32 x = _position.first * brick_width;
        _Float32 y = ((_position.second - 1) / 2) * brick_height;
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y}, colors[1], SDL_FPoint{0.5f, 0.0f}});
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brick_width / 2, y + brick_height}, colors[2], SDL_FPoint{1.0f, 0.8f}});
        _vertices.push_back(SDL_Vertex{SDL_FPoint{x - brick_width / 2, y + brick_height}, colors[0], SDL_FPoint{0.0f, 0.8f}});
    }   
}

const std::vector<int32_t> BrickTriangular::getIndices() const
{
    std::vector<int32_t> indices = {0, 1, 2};
    return indices;
}

const std::pair<_Float32, _Float32> BrickTriangular::getCenter() const
{
    _Float32 x = (_vertices[0].position.x + _vertices[1].position.x + _vertices[2].position.x) / 3.0f;
    _Float32 y = (_vertices[0].position.y + _vertices[1].position.y + _vertices[2].position.y) / 3.0f;
    return std::pair<_Float32, _Float32>{x, y};
}


BrickHexagonal::BrickHexagonal(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name)
: Brick(position, grid_dimensions, surface_size, color, resistance, power_up_name)
{
    calculateVerticesWithPosition(grid_dimensions, surface_size);
}

void BrickHexagonal::calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size)
{
    SDL_Color original_color = {static_cast<uint8_t>(_color >> 24), static_cast<uint8_t>(_color >> 16), static_cast<uint8_t>(_color >> 8), static_cast<uint8_t>(_color)};
    std::vector<SDL_Color> colors = GenerateTintAndShadeColors(original_color, 6);

    _vertices.clear();

    _Float32 brick_width = surface_size.first / (grid_dimensions.first * 1.5f + 0.25f);
    _Float32 brick_height = static_cast<int32_t>(grid_dimensions.second) % 2 == 0 ? surface_size.second / ((grid_dimensions.second) / 2 + 0.5f): surface_size.second / ((grid_dimensions.second + 1) / 2);
    _Float32 x, y;

    if(_position.second % 2 == 0)
    {
        x = _position.first * (brick_width + brick_width / 2);
        y = (_position.second / 2) * brick_height;
    }
    else
    {
        x = _position.first * (brick_width + brick_width / 2) + 3 * brick_width / 4;
        y = ((_position.second - 1) / 2) * brick_height + brick_height / 2;
    }

    _vertices.push_back(SDL_Vertex{SDL_FPoint{x, y + brick_height / 2}, colors[5], SDL_FPoint{0.0f, 0.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brick_width / 4, y}, colors[0], SDL_FPoint{0.0f, 0.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brick_width / 4, y}, colors[1], SDL_FPoint{0.0f, 0.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brick_width, y + brick_height / 2}, colors[2], SDL_FPoint{0.0f, 0.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + 3 * brick_width / 4, y + brick_height}, colors[3], SDL_FPoint{0.0f, 0.0f}});
    _vertices.push_back(SDL_Vertex{SDL_FPoint{x + brick_width / 4, y + brick_height}, colors[4], SDL_FPoint{0.0f, 0.0f}});
}

const std::vector<int32_t> BrickHexagonal::getIndices() const
{
    std::vector<int32_t> indices = {0, 1, 5, 1, 2, 5, 2, 3, 5, 3, 4, 5};
    return indices;
}

const std::pair<_Float32, _Float32> BrickHexagonal::getCenter() const
{
    _Float32 x = (_vertices[0].position.x + _vertices[1].position.x + _vertices[2].position.x + _vertices[3].position.x + _vertices[4].position.x + _vertices[5].position.x) / 6.0f;
    _Float32 y = (_vertices[0].position.y + _vertices[1].position.y + _vertices[2].position.y + _vertices[3].position.y + _vertices[4].position.y + _vertices[5].position.y) / 6.0f;
    return std::pair<_Float32, _Float32>{x, y};
}

const std::vector<SDL_Color> Brick::GenerateTintAndShadeColors(const SDL_Color& original_color, const uint32_t vertices_count)
{
    std::vector<SDL_Color> colors;
    colors.reserve(vertices_count);

    _Float32 step = 1.0f / vertices_count;

    for (uint32_t i = vertices_count / 2; i >= 1; i--)
    {
        SDL_Color tinted_color;
        tinted_color.r = original_color.r + (255 - original_color.r) * (i * step);
        tinted_color.g = original_color.g + (255 - original_color.g) * (i * step);
        tinted_color.b = original_color.b + (255 - original_color.b) * (i * step);
        tinted_color.a = original_color.a;
        colors.push_back(tinted_color);
    }

    if (vertices_count % 2)
        colors.push_back(original_color);

    for (uint32_t i = 1; i <= vertices_count / 2; ++i)
    {
        SDL_Color shaded_color;
        shaded_color.r = original_color.r * ((i * step));
        shaded_color.g = original_color.g * ((i * step));
        shaded_color.b = original_color.b * ((i * step));
        shaded_color.a = original_color.a;
        colors.push_back(shaded_color);
    }

    return colors;
}


