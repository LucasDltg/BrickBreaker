#include <SDL2/SDL.h>
#include <iostream>
#include "Brick.h"

Brick::Brick(int x, int y, int width, int height, SDL_Color color, int resistance)
: rect({x, y, width, height}), color(color), resistance(resistance) {}

int Brick::getResistance() const
{
    return resistance;
}

void Brick::decreaseResistance()
{
    if (resistance > 0)
        resistance--;
}

const SDL_Rect& Brick::getRect() const
{
    return this->rect;
}

const SDL_Color& Brick::getColor() const
{
    return color;
}