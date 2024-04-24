#include <algorithm>
#include <cmath>
#include <iostream>
#include "Ball.h"

Ball::Ball(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed)
    : Circle(radius, center, color, speed)
    {}