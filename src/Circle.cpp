#include <algorithm>
#include <cmath>
#include <iostream>
#include "../include/Circle.h"

Circle::Circle()
    : _radius(0), _center({0, 0}), _color({0, 0, 0, 0}), _speed({0, 0})
    {}

Circle::Circle(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed)
    : _radius(radius), _center(center), _color(color), _speed(speed)
    {}

_Float32 Circle::getRadius() const {
    return _radius;
}

const std::pair<_Float32, _Float32>& Circle::getCenter() const {
    return _center;
}

SDL_Color Circle::getColor() const {
    return _color;
}

std::pair<_Float32, _Float32> Circle::getSpeed() const {
    return _speed;
}

void Circle::setRadius(_Float32 newRadius) {
    _radius = newRadius;
}

void Circle::setCenter(std::pair<_Float32, _Float32> newCenter) {
    _center = newCenter;
}

void Circle::setColor(SDL_Color newColor) {
    _color = newColor;
}

void Circle::setSpeed(std::pair<_Float32, _Float32> newSpeed) {
    _speed = newSpeed;
}


void Circle::update(uint64_t delta_time) {
    _center.first += _speed.first * delta_time;
    _center.second += _speed.second * delta_time;
}

bool Circle::resolveCollisionWithRectangle(const SDL_FRect& rect) {
    float closestX = _center.first;
    float closestY = _center.second;

    if (_center.first < rect.x)
        closestX = rect.x;
    else if (_center.first > rect.x + rect.w)
        closestX = rect.x + rect.w;

    if (_center.second < rect.y)
        closestY = rect.y;
    else if (_center.second > rect.y + rect.h)
        closestY = rect.y + rect.h;

    float distance = sqrt((_center.first - closestX) * (_center.first - closestX) + 
                          (_center.second - closestY) * (_center.second - closestY));

    if (distance < _radius)
    {
        float overlap = _radius - distance;
        float dx = _center.first - closestX;
        float dy = _center.second - closestY;

        if (dx > 0)
            dx = overlap;
        else 
            dx = -overlap;

        if (dy > 0) 
            dy = overlap;
        else
            dy = -overlap;

        _center.first += dx;
        _center.second += dy;

        float collisionAngle = atan2(_center.second - (rect.y + rect.h / 2), _center.first - (rect.x + rect.w / 2));

        float currentSpeed = sqrt(_speed.first * _speed.first + _speed.second * _speed.second);
        _speed.first = currentSpeed * cos(collisionAngle);
        _speed.second = currentSpeed * sin(collisionAngle);

        return true;
    }
    return false;
}

bool Circle::resolveCollisionWithLine(std::pair<_Float32, _Float32> p1, std::pair<_Float32, _Float32> p2)
{
    float A = _center.first - p1.first;
    float B = _center.second - p1.second;
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

    float dx = _center.first - xx;
    float dy = _center.second - yy;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < _radius && distance) {
        float overlap = _radius - distance;

        if (distance > 0) {
            float directionX = dx / distance;
            float directionY = dy / distance;

            _center.first += overlap * directionX;
            _center.second += overlap * directionY;

            float dotProduct = (_speed.first * directionX + _speed.second * directionY) * 2.0f;
            _speed.first -= dotProduct * directionX;
            _speed.second -= dotProduct * directionY;

            return true;
        }
    }
    return false;
}