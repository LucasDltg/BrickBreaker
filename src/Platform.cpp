#include <SDL2/SDL.h>
#include "../include/Platform.h"

Platform::Platform()
    : rect({ 0, 0, 0, 0 }), color({ 0, 0, 0, 0 }), speed_x(0)
{}

Platform::Platform(int pos_x, int pos_y, int width, int height, SDL_Color color)
    : rect({ pos_x, pos_y, width, height }), color(color), speed_x(0)
{}

void Platform::setRect(const SDL_Rect& rect) {
    this->rect = rect;
}

void Platform::setColor(const SDL_Color& color) {
    this->color = color;
}

const SDL_Rect& Platform::getRect() const {
    return rect;
}

const SDL_Color Platform::getColor() const {
    return color;
}

void Platform::setSpeedX(int32_t speed_x)
{
    this->speed_x = speed_x;
}

void Platform::update(uint64_t delta_time, int screen_width)
{
    rect.x += speed_x * delta_time;
    if (rect.x + rect.w > screen_width)
        rect.x = screen_width - rect.w;
    else if (rect.x + speed_x < 0)
        rect.x = 0;
}