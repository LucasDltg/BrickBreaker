#ifndef BALL_H
#define BALL_H

#include <cstdint>
#include <SDL2/SDL.h>
#include "../include/Circle.h"

class Ball : public Circle{
public:
    Ball(const _Float32 radius, const std::pair<_Float32, _Float32>& center, const SDL_Color& color, const std::pair<_Float32, _Float32>& speed);
};

#endif // BALL_H
