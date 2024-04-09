#include "Ball.h"

Ball::Ball(uint32_t radius, std::pair<uint32_t, uint32_t> center, SDL_Color color, std::pair<int, int> speed)
    : radius(radius), center(center), color(color), speed(speed) {}

uint32_t Ball::getRadius() const {
    return radius;
}

std::pair<uint32_t, uint32_t> Ball::getCenter() const {
    return center;
}

SDL_Color Ball::getColor() const {
    return color;
}

std::pair<int, int> Ball::getSpeed() const {
    return speed;
}

void Ball::setRadius(uint32_t newRadius) {
    radius = newRadius;
}

void Ball::setCenter(std::pair<uint32_t, uint32_t> newCenter) {
    center = newCenter;
}

void Ball::setColor(SDL_Color newColor) {
    color = newColor;
}

void Ball::setSpeed(std::pair<int, int> newSpeed) {
    speed = newSpeed;
}
