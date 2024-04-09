#ifndef BALL_H
#define BALL_H

#include <utility>
#include <cstdint>
#include <SDL2/SDL.h>

class Ball {
public:
    Ball(uint32_t radius, std::pair<uint32_t, uint32_t> center, SDL_Color color, std::pair<int, int> speed);

    uint32_t getRadius() const;
    const std::pair<uint32_t, uint32_t>& getCenter() const;
    SDL_Color getColor() const;
    std::pair<int, int> getSpeed() const;

    void setRadius(uint32_t newRadius);
    void setCenter(std::pair<uint32_t, uint32_t> newCenter);
    void setColor(SDL_Color newColor);
    void setSpeed(std::pair<int, int> newSpeed);

private:
    uint32_t radius;
    std::pair<uint32_t, uint32_t> center;
    SDL_Color color;
    std::pair<int, int> speed;
};

#endif // BALL_H
