#ifndef BRICK_H
#define BRICK_H

#include <SDL2/SDL.h>
#include <memory>
#include "../include/PowerUp.h"

class Brick
{
public:
    Brick(int x, int y, uint32_t color, int resistance, std::string PowerUp);

    int getResistance() const;
    void decreaseResistance();

    void setPosition(int x, int y);
    const std::pair<uint32_t, uint32_t>& getPosition() const;

    const uint32_t& getColor() const;
    std::unique_ptr<PowerUp> getPowerUp();


private:
    std::pair<uint32_t, uint32_t> position;
    uint32_t color;
    int resistance;
    std::unique_ptr<PowerUp> powerUp;
};

#endif // BRICK_H
