#include <SDL2/SDL.h>
#include <iostream>
#include "brick.h"

Brick::Brick(SDL_Renderer* renderer, int x, int y, int width, int height, SDL_Color color, int resistance)
: renderer(renderer), rect({x, y, width, height}), color(color), resistance(resistance) {}

void Brick::render()
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

int Brick::getResistance() const
{
    return resistance;
}

void Brick::decreaseResistance()
{
    if (resistance > 0)
        resistance--;
}