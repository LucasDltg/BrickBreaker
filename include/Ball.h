#ifndef BALL_H
#define BALL_H

#include <utility>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../include/Circle.hpp"

class Ball : public Circle{
public:
    Ball(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed);
};

#endif // BALL_H
