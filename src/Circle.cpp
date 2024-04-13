#include <algorithm>
#include <cmath>
#include <iostream>
#include "../include/Circle.hpp"

Circle::Circle()
    : radius(0), center({0, 0}), color({0, 0, 0, 0}), speed({0, 0})
    {}

Circle::Circle(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed, std::string image_path)
    : radius(radius), center(center), color(color), speed(speed)
    {
        if(image_path.empty())
        {
            return;
        }

        surface = std::shared_ptr<SDL_Surface>(IMG_Load(image_path.c_str()), SDL_FreeSurface);
        if (!surface)
        {
            std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        }
    }

_Float32 Circle::getRadius() const {
    return radius;
}

const std::pair<_Float32, _Float32>& Circle::getCenter() const {
    return center;
}

SDL_Color Circle::getColor() const {
    return color;
}

std::pair<_Float32, _Float32> Circle::getSpeed() const {
    return speed;
}

std::shared_ptr<SDL_Surface> Circle::getSurface() const {
    return surface;
}

void Circle::setRadius(_Float32 newRadius) {
    radius = newRadius;
}

void Circle::setCenter(std::pair<_Float32, _Float32> newCenter) {
    center = newCenter;
}

void Circle::setColor(SDL_Color newColor) {
    color = newColor;
}

void Circle::setSpeed(std::pair<_Float32, _Float32> newSpeed) {
    speed = newSpeed;
}


void Circle::update(uint64_t delta_time) {
    center.first += speed.first * delta_time;
    center.second += speed.second * delta_time;
}

bool Circle::resolveCollisionWithRectangle(const SDL_Rect& rect)
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