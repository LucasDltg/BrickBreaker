#include <algorithm>
#include <cmath>
#include <iostream>
#include "Ball.h"

SDL_Surface* Ball::ballSurface = IMG_Load("ball.png");

Ball::Ball(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed)
    : radius(radius), center(center), color(color), speed(speed)
    {
        if (!ballSurface)
        {
            std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        }
    }

_Float32 Ball::getRadius() const {
    return radius;
}

const std::pair<_Float32, _Float32>& Ball::getCenter() const {
    return center;
}

SDL_Color Ball::getColor() const {
    return color;
}

std::pair<_Float32, _Float32> Ball::getSpeed() const {
    return speed;
}

void Ball::setRadius(_Float32 newRadius) {
    radius = newRadius;
}

void Ball::setCenter(std::pair<_Float32, _Float32> newCenter) {
    center = newCenter;
}

void Ball::setColor(SDL_Color newColor) {
    color = newColor;
}

void Ball::setSpeed(std::pair<_Float32, _Float32> newSpeed) {
    speed = newSpeed;
}


void Ball::update(uint64_t delta_time) {
    center.first += speed.first * delta_time;
    center.second += speed.second * delta_time;
}

bool Ball::resolveCollisionWithRectangle(const SDL_Rect& rect)
{
    float closestX = std::max(static_cast<float>(rect.x), std::min(center.first, static_cast<float>(rect.x + rect.w)));
    float closestY = std::max(static_cast<float>(rect.y), std::min(center.second, static_cast<float>(rect.y + rect.h)));

    float distanceX = center.first - closestX;
    float distanceY = center.second - closestY;
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;

    if (distanceSquared < (radius * radius)) {
        float distance = std::sqrt(distanceSquared);
        float overlap = radius - distance;

        if (distance > 0) {
            float directionX = distanceX / distance;
            float directionY = distanceY / distance;

            center.first += overlap * directionX;
            center.second += overlap * directionY;

            float dotProduct = (speed.first * directionX + speed.second * directionY) * 2.0f;
            speed.first -= dotProduct * directionX;
            speed.second -= dotProduct * directionY;

            return true;
        }
    }
    return false;
}