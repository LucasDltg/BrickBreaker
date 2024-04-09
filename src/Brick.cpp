#include <SDL2/SDL.h>
#include <iostream>
#include "Brick.h"

Brick::Brick(int x, int y, uint32_t color, int resistance)
: position({x, y}), color(color), resistance(resistance) 
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