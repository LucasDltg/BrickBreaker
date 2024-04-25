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
    Brick(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string PowerUp, const std::shared_ptr<SDL_Renderer> renderer=nullptr);

    int getResistance() const;
    void decreaseResistance();
    _Float32 getResistancePercentage() const;

    const uint32_t& getColor() const;
    std::unique_ptr<PowerUp> getPowerUp();
    std::vector<SDL_Vertex> getVertices() const;
    std::vector<SDL_Vertex> getVerticesWithoutColor() const;

    virtual std::vector<int32_t> getIndices() const = 0;
    virtual void calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size) = 0;
    virtual std::pair<_Float32, _Float32> getCenter() const = 0;

protected:
    std::pair<uint32_t, uint32_t> _position;
    std::vector<SDL_Vertex> _vertices;
    uint32_t _color;
    int _resistance;
    int32_t _max_resistance;
    std::unique_ptr<PowerUp> _powerUp;

    std::vector<SDL_Color> GenerateTintAndShadeColors(const SDL_Color& originalColor, int n);
};

class BrickRectangular : public Brick
{
    public:
    BrickRectangular(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string PowerUp, const std::shared_ptr<SDL_Renderer> renderer=nullptr);
    void calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size) override;
    std::vector<int32_t> getIndices() const override;
    std::pair<_Float32, _Float32> getCenter() const override;
};

class BrickTriangular : public Brick
{
    public:
    BrickTriangular(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string PowerUp, const std::shared_ptr<SDL_Renderer> renderer=nullptr);
    void calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size) override;
    std::vector<int32_t> getIndices() const override;
    std::pair<_Float32, _Float32> getCenter() const override;
};

class BrickHexagonal : public Brick
{
    public:
    BrickHexagonal(std::pair<uint32_t, uint32_t> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string PowerUp, const std::shared_ptr<SDL_Renderer> renderer=nullptr);
    void calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size) override;
    std::vector<int32_t> getIndices() const override;
    std::pair<_Float32, _Float32> getCenter() const override;
};

#endif // BRICK_H
