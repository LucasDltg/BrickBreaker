#ifndef BALL_H
#define BALL_H

#include "../include/Circle.h"

/**
 * @brief Represents a ball in Breakout.
 */
class Ball : public Circle {
public:
    /**
     * @brief Constructor for Ball.
     * 
     * @param radius The radius of the ball.
     * @param center The center coordinates of the ball.
     * @param speed The speed vector of the ball.
     */
    Ball(const _Float32 radius, const std::pair<_Float32, _Float32>& center, const std::pair<_Float32, _Float32>& speed);
};

#endif // BALL_H
