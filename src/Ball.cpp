#include <algorithm>
#include <cmath>
#include <iostream>
#include "Ball.h"

Ball::Ball(const _Float32 radius, const std::pair<_Float32, _Float32>& center, const std::pair<_Float32, _Float32>& speed)
    : Circle(radius, center, speed)
    {}