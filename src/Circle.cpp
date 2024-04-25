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

void Circle::setRadius(_Float32 new_radius) {
    _radius = new_radius;
}

void Circle::setCenter(std::pair<_Float32, _Float32> new_center) {
    _center = new_center;
}

void Circle::setColor(SDL_Color new_color) {
    _color = new_color;
}

void Circle::setSpeed(std::pair<_Float32, _Float32> new_speed) {
    _speed = new_speed;
}


void Circle::update(uint64_t delta_time) {
    _center.first += _speed.first * delta_time;
    _center.second += _speed.second * delta_time;
}

bool Circle::resolveCollisionWithRectangle(const SDL_FRect& rect) {
    float closest_x = _center.first;
    float closest_y = _center.second;

    if (_center.first < rect.x)
        closest_x = rect.x;
    else if (_center.first > rect.x + rect.w)
        closest_x = rect.x + rect.w;

    if (_center.second < rect.y)
        closest_y = rect.y;
    else if (_center.second > rect.y + rect.h)
        closest_y = rect.y + rect.h;

    float distance = sqrt((_center.first - closest_x) * (_center.first - closest_x) + 
                          (_center.second - closest_y) * (_center.second - closest_y));

    if (distance < _radius)
    {
        float overlap = _radius - distance;
        float dx = _center.first - closest_x;
        float dy = _center.second - closest_y;

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

        float collision_angle = atan2(_center.second - (rect.y + rect.h / 2), _center.first - (rect.x + rect.w / 2));

        float current_speed = sqrt(_speed.first * _speed.first + _speed.second * _speed.second);
        _speed.first = current_speed * cos(collision_angle);
        _speed.second = current_speed * sin(collision_angle);

        return true;
    }
    return false;
}

bool Circle::resolveCollisionWithLine(std::pair<_Float32, _Float32> p1, std::pair<_Float32, _Float32> p2)
{
    float a = _center.first - p1.first;
    float b = _center.second - p1.second;
    float c = p2.first - p1.first;
    float d = p2.second - p1.second;

    float dot = a * c + b * d;
    float len_sq = c * c + d * d;
    float param = dot / len_sq;

    float xx, yy;

    if (param < 0) {
        xx = p1.first;
        yy = p1.second;
    } else if (param > 1) {
        xx = p2.first;
        yy = p2.second;
    } else {
        xx = p1.first + param * c;
        yy = p1.second + param * d;
    }

    float dx = _center.first - xx;
    float dy = _center.second - yy;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < _radius && distance) {
        float overlap = _radius - distance;

        if (distance > 0) {
            float direction_x = dx / distance;
            float direction_y = dy / distance;

            _center.first += overlap * direction_x;
            _center.second += overlap * direction_y;

            float dot_product = (_speed.first * direction_x + _speed.second * direction_y) * 2.0f;
            _speed.first -= dot_product * direction_x;
            _speed.second -= dot_product * direction_y;

            return true;
        }
    }
    return false;
}