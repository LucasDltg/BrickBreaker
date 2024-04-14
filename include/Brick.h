#ifndef BRICK_H
#define BRICK_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include "../include/PowerUp.h"

class Brick
{
public:
    Brick(std::pair<_Float32, _Float32> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string PowerUp);

    int getResistance() const;
    void decreaseResistance();

    void setVertices(std::vector<SDL_Vertex> vertices);

    const uint32_t& getColor() const;
    std::unique_ptr<PowerUp> getPowerUp();

    virtual std::vector<SDL_Vertex> getVertices() const = 0;
    virtual std::vector<int32_t> getIndices() const = 0;
    virtual std::pair<_Float32, _Float32> getCenter() const = 0;
    virtual void calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size) = 0;


protected:
    std::pair<_Float32, _Float32> position;
    std::vector<SDL_Vertex> vertices;
    uint32_t color;
    int resistance;
    std::unique_ptr<PowerUp> powerUp;
};

class BrickRectangular : public Brick
{
    public:
    BrickRectangular(std::pair<_Float32, _Float32> position, std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size, uint32_t color, int resistance, std::string PowerUp);
    void calculateVerticesWithPosition(std::pair<_Float32, _Float32> grid_dimensions, std::pair<_Float32, _Float32> surface_size) override;
    std::vector<SDL_Vertex> getVertices() const override;
    std::vector<int32_t> getIndices() const override;
    std::pair<_Float32, _Float32> getCenter() const override;
};

#endif // BRICK_H
