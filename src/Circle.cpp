#include <algorithm>
#include <cmath>
#include <iostream>
#include "../include/Circle.h"

Circle::Circle()
    : radius(0), center({0, 0}), color({0, 0, 0, 0}), speed({0, 0})
    {}

Circle::Circle(std::string image_path)
    : radius(0), center({0, 0}), color({0, 0, 0, 0}), speed({0, 0})
    {
        if(image_path.empty())
            return;

        surface = std::shared_ptr<SDL_Surface>(IMG_Load(image_path.c_str()), SDL_FreeSurface);
        if (!surface)
        {
            std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        }
    }

Circle::Circle(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed, std::string image_path)
    : radius(radius), center(center), color(color), speed(speed)
    {
        if(image_path.empty())
            return;

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

bool Circle::resolveCollisionWithRectangle(const SDL_Rect& rect) {
    float closestX = center.first;
    float closestY = center.second;

    if (center.first < rect.x)
        closestX = rect.x;
    else if (center.first > rect.x + rect.w)
        closestX = rect.x + rect.w;

    if (center.second < rect.y)
        closestY = rect.y;
    else if (center.second > rect.y + rect.h)
        closestY = rect.y + rect.h;

    float distance = sqrt((center.first - closestX) * (center.first - closestX) + 
                          (center.second - closestY) * (center.second - closestY));

    if (distance < radius)
    {
        float overlap = radius - distance;
        float dx = center.first - closestX;
        float dy = center.second - closestY;

        if (dx > 0)
            dx = overlap;
        else 
            dx = -overlap;

        if (dy > 0) 
            dy = overlap;
        else
            dy = -overlap;

        center.first += dx;
        center.second += dy;

        float collisionAngle = atan2(center.second - (rect.y + rect.h / 2), center.first - (rect.x + rect.w / 2));

        float currentSpeed = sqrt(speed.first * speed.first + speed.second * speed.second);
        speed.first = currentSpeed * cos(collisionAngle);
        speed.second = currentSpeed * sin(collisionAngle);

        return true;
    }
    return false;
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