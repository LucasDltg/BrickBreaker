#ifndef BRICK_H
#define BRICK_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <map>
#include <cstdint>
#include "../include/PowerUp.h"

class Brick
{
public:
    Brick(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name);

    const int32_t getResistance() const;
    void decreaseResistance();
    const _Float32 getResistancePercentage() const;

    const uint32_t& getColor() const;
    const std::unique_ptr<PowerUp> getPowerUp();
    const std::vector<SDL_Vertex>& getVertices() const;
    const std::vector<SDL_Vertex> getVerticesWithoutColor() const;

    virtual const std::vector<int32_t> getIndices() const = 0;
    virtual void calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size) = 0;
    virtual const std::pair<_Float32, _Float32> getCenter() const = 0;

protected:
    std::pair<uint32_t, uint32_t> _position;
    std::vector<SDL_Vertex> _vertices;
    uint32_t _color;
    int32_t _resistance;
    int32_t _max_resistance;
    std::unique_ptr<PowerUp> _powerUp;

    const std::vector<SDL_Color> GenerateTintAndShadeColors(const SDL_Color& original_color, const uint32_t vertices_count);
};

class BrickRectangular : public Brick
{
    public:
    BrickRectangular(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name);
    void calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size) override;
    const std::vector<int32_t> getIndices() const override;
    const std::pair<_Float32, _Float32> getCenter() const override;
};

class BrickTriangular : public Brick
{
    public:
    BrickTriangular(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name);
    void calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size) override;
    const std::vector<int32_t> getIndices() const override;
    const std::pair<_Float32, _Float32> getCenter() const override;
};

class BrickHexagonal : public Brick
{
    public:
    BrickHexagonal(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name);
    void calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size) override;
    const std::vector<int32_t> getIndices() const override;
    const std::pair<_Float32, _Float32> getCenter() const override;
};

#endif // BRICK_H
