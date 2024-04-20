#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../include/Platform.h"
#include <iostream>

Platform::Platform(std::string image_path)
: rect({ 0, 0, 0, 0 }), color({ 0, 0, 0, 0 }), speed_x(0)
{
    if (image_path.empty())
        return;

    surface = std::shared_ptr<SDL_Surface>(IMG_Load(image_path.c_str()), SDL_FreeSurface);
    if (!surface)
    {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    }
}

void Platform::setRect(const SDL_Rect& rect) {
    this->rect = rect;
}

void Platform::setColor(const SDL_Color& color) {
    this->color = color;
}

const SDL_Rect& Platform::getRect() const {
    return rect;
}

SDL_Rect& Platform::getRect() {
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

std::shared_ptr<SDL_Surface> Platform::getSurface() const {
    return surface;
}

const int32_t Platform::getSpeedX() const {
    return speed_x;
}