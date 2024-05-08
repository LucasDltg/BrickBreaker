#include "../include/Paddle.h"

Paddle::Paddle()
: _rect({ 0, 0, 0, 0 }), _speed_x(0)
{}

void Paddle::setRect(const SDL_FRect& rect) {
    _rect = rect;
}

const SDL_FRect& Paddle::getRect() const {
    return _rect;
}

SDL_FRect& Paddle::getRect() {
    return _rect;
}

void Paddle::setSpeedX(const _Float32 speed_x)
{
    _speed_x = speed_x;
}

void Paddle::update(const uint64_t delta_time, const int32_t screen_width)
{
    _rect.x += _speed_x * delta_time;
    if (_rect.x + _rect.w > screen_width)
        _rect.x = screen_width - _rect.w;
    else if (_rect.x + _speed_x < 0)
        _rect.x = 0;
}

const int32_t Paddle::getSpeedX() const {
    return _speed_x;
}