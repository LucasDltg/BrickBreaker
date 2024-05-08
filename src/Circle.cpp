#include "../include/Circle.h"

Circle::Circle()
    : _radius(0), _center({0, 0}), _speed({0, 0})
    {}

Circle::Circle(const _Float32 radius, const std::pair<_Float32, _Float32>& center, const std::pair<_Float32, _Float32>& speed)
    : _radius(radius), _center(center), _speed(speed)
    {}

_Float32 Circle::getRadius() const {
    return _radius;
}

const std::pair<_Float32, _Float32>& Circle::getCenter() const {
    return _center;
}

const std::pair<_Float32, _Float32>& Circle::getSpeed() const {
    return _speed;
}

void Circle::setRadius(_Float32 new_radius) {
    _radius = new_radius;
}

void Circle::setCenter(const std::pair<_Float32, _Float32>& new_center) {
    _center = new_center;
}

void Circle::setSpeed(const std::pair<_Float32, _Float32>& new_speed) {
    _speed = new_speed;
}


void Circle::update(uint64_t delta_time) {
    _center.first += _speed.first * delta_time;
    _center.second += _speed.second * delta_time;
}

bool Circle::resolveCollisionWithRectangle(const SDL_FRect& rect) {
    _Float32 closest_x = _center.first;
    _Float32 closest_y = _center.second;

    if (_center.first < rect.x)
        closest_x = rect.x;
    else if (_center.first > rect.x + rect.w)
        closest_x = rect.x + rect.w;

    if (_center.second < rect.y)
        closest_y = rect.y;
    else if (_center.second > rect.y + rect.h)
        closest_y = rect.y + rect.h;

    _Float32 distance = sqrt((_center.first - closest_x) * (_center.first - closest_x) + 
                          (_center.second - closest_y) * (_center.second - closest_y));

    if (distance < _radius)
    {
        _Float32 overlap = _radius - distance;
        _Float32 dx = _center.first - closest_x;
        _Float32 dy = _center.second - closest_y;

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

        _Float32 collision_angle = atan2(_center.second - (rect.y + rect.h / 2), _center.first - (rect.x + rect.w / 2));

        _Float32 current_speed = sqrt(_speed.first * _speed.first + _speed.second * _speed.second);
        _speed.first = current_speed * cos(collision_angle);
        _speed.second = current_speed * sin(collision_angle);

        return true;
    }
    return false;
}

bool Circle::resolveCollisionWithLine(const std::pair<_Float32, _Float32>& p1, const std::pair<_Float32, _Float32>& p2, uint64_t deltaTime)
{
    // Calculate the ball's position at the next time step
    _Float32 nextX = _center.first + _speed.first * deltaTime;
    _Float32 nextY = _center.second + _speed.second * deltaTime;

    _Float32 a = nextX - p1.first;
    _Float32 b = nextY - p1.second;
    _Float32 c = p2.first - p1.first;
    _Float32 d = p2.second - p1.second;

    _Float32 dot = a * c + b * d;
    _Float32 len_sq = c * c + d * d;
    _Float32 param = dot / len_sq;

    _Float32 xx, yy;

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

    _Float32 dx = nextX - xx;
    _Float32 dy = nextY - yy;
    _Float32 distance = std::sqrt(dx * dx + dy * dy);

    if (distance < _radius && distance) {
        _Float32 overlap = _radius - distance;

        if (distance > 0) {
            _Float32 direction_x = dx / distance;
            _Float32 direction_y = dy / distance;

            // Resolve the collision based on the fixed time step
            _center.first += overlap * direction_x;
            _center.second += overlap * direction_y;

            _Float32 dot_product = (_speed.first * direction_x + _speed.second * direction_y) * 2.0f;
            _speed.first -= dot_product * direction_x;
            _speed.second -= dot_product * direction_y;

            return true;
        }
    }
    return false;
}
