#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../include/Platform.h"
#include <iostream>

Platform::Platform()
: _rect({ 0, 0, 0, 0 }), _color({ 0, 0, 0, 0 }), _speed_x(0)
{}

void Platform::setRect(const SDL_FRect& rect) {
    _rect = rect;
}

void Platform::setColor(const SDL_Color& color) {
    _color = color;
}

const SDL_FRect& Platform::getRect() const {
    return _rect;
}

SDL_FRect& Platform::getRect() {
    return _rect;
}

const SDL_Color Platform::getColor() const {
    return _color;
}

void Platform::setSpeedX(_Float32 speed_x)
{
    _speed_x = speed_x;
}

void Platform::update(uint64_t delta_time, int screen_width)
{
    _rect.x += _speed_x * delta_time;
    if (_rect.x + _rect.w > screen_width)
        _rect.x = screen_width - _rect.w;
    else if (_rect.x + _speed_x < 0)
        _rect.x = 0;
}

const int32_t Platform::getSpeedX() const {
    return _speed_x;
}