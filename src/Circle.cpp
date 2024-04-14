#include <algorithm>
#include <cmath>
#include <iostream>
#include "../include/Circle.h"

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
    std::pair<_Float32, _Float32> p1, p2;
    p1 = {static_cast<_Float32>(rect.x), static_cast<_Float32>(rect.y)};
    p2 = {static_cast<_Float32>(rect.x + rect.w), static_cast<_Float32>(rect.y)};
    if(resolveCollisionWithLine(p1, p2))
        return true;

    p1 = {static_cast<_Float32>(rect.x + rect.w), static_cast<_Float32>(rect.y)};
    p2 = {static_cast<_Float32>(rect.x + rect.w), static_cast<_Float32>(rect.y + rect.h)};
    if(resolveCollisionWithLine(p1, p2))
        return true;

    p1 = {static_cast<_Float32>(rect.x + rect.w), static_cast<_Float32>(rect.y + rect.h)};
    p2 = {static_cast<_Float32>(rect.x), static_cast<_Float32>(rect.y + rect.h)};
    if(resolveCollisionWithLine(p1, p2))
        return true;

    p1 = {static_cast<_Float32>(rect.x), static_cast<_Float32>(rect.y + rect.h)};
    p2 = {static_cast<_Float32>(rect.x), static_cast<_Float32>(rect.y)};
    return resolveCollisionWithLine(p1, p2);
}

bool Circle::resolveCollisionWithLine(std::pair<_Float32, _Float32> p1, std::pair<_Float32, _Float32> p2)
{
    float A = center.first - p1.first;
    float B = center.second - p1.second;
    float C = p2.first - p1.first;
    float D = p2.second - p1.second;

    float dot = A * C + B * D;
    float len_sq = C * C + D * D;
    float param = dot / len_sq;

    float xx, yy;

    if (param < 0) {
        xx = p1.first;
        yy = p1.second;
    } else if (param > 1) {
        xx = p2.first;
        yy = p2.second;
    } else {
        xx = p1.first + param * C;
        yy = p1.second + param * D;
    }

    float dx = center.first - xx;
    float dy = center.second - yy;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < radius && distance) {
        float overlap = radius - distance;

        if (distance > 0) {
            float directionX = dx / distance;
            float directionY = dy / distance;

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