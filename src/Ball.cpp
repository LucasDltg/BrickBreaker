#include <algorithm>
#include <cmath>
#include "Ball.h"

Ball::Ball(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed)
    : radius(radius), center(center), color(color), speed(speed) {}

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

bool Ball::resolveCollisionWithRectangle(const SDL_Rect& rect) {
    // Calculate the closest point on the rectangle to the circle's center
    float closestX = std::max(static_cast<float>(rect.x), std::min(center.first, static_cast<float>(rect.x + rect.w)));
    float closestY = std::max(static_cast<float>(rect.y), std::min(center.second, static_cast<float>(rect.y + rect.h)));
    
    // Calculate the distance between the circle's center and the closest point
    float distanceX = center.first - closestX;
    float distanceY = center.second - closestY;
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    
    // Check if the distance is less than the circle's radius squared
    if (distanceSquared < (radius * radius)) {
        // Collision detected, resolve it
        float distance = sqrt(distanceSquared);
        float overlap = radius - distance;

        // Calculate the direction from the circle's center to the closest point on the rectangle
        float directionX = distanceX / distance;
        float directionY = distanceY / distance;

        // Adjust the circle's position by the overlap in the opposite direction
        center.first += overlap * directionX;
        center.second += overlap * directionY;

        // Invert the appropriate component of the circle's speed vector to simulate bouncing off the rectangle
        if (center.first < closestX || center.first > closestX + rect.w)
            speed.first = -speed.first;
        if (center.second < closestY || center.second > closestY + rect.h)
            speed.second = -speed.second;

        return true;
    }

    return false;
}
