#ifndef BALL_H
#define BALL_H

#include <utility>
#include <cstdint>
#include <SDL2/SDL.h>

class Ball {
public:
    Ball(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed);

    _Float32 getRadius() const;
    const std::pair<_Float32, _Float32>& getCenter() const;
    SDL_Color getColor() const;
    std::pair<_Float32, _Float32> getSpeed() const;

    void setRadius(_Float32 newRadius);
    void setCenter(std::pair<_Float32, _Float32> newCenter);
    void setColor(SDL_Color newColor);
    void setSpeed(std::pair<_Float32, _Float32> newSpeed);

    void update(uint64_t delta_time);
    bool resolveCollisionWithRectangle(const SDL_Rect& rect);

private:
    _Float32 radius;
    std::pair<_Float32, _Float32> center;
    SDL_Color color;
    std::pair<_Float32, _Float32> speed;
};

#endif // BALL_H
