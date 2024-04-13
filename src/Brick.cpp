#include <SDL2/SDL.h>
#include <iostream>
#include "../include/Brick.h"
#include "../include/PowerUp.h"

Brick::Brick(int x, int y, uint32_t color, int resistance, std::string power_up)
: position({x, y}), color(color), resistance(resistance), powerUp(initPowerUp(power_up)) 
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

void Brick::setPosition(int x, int y)
{
    position.first = x;
    position.second = y;
}

const std::pair<uint32_t, uint32_t>& Brick::getPosition() const
{
    return position;
}

const uint32_t& Brick::getColor() const
{
    return color;
}

std::unique_ptr<PowerUp> Brick::getPowerUp()
{
    return std::move(powerUp);
}