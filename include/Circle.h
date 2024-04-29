#ifndef CIRCLE_H
#define CIRCLE_H

#include <utility>
#include <cstdint>
#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Circle {
public:
    Circle();
    Circle(const _Float32 radius, const std::pair<_Float32, _Float32>& center, const SDL_Color& color, const std::pair<_Float32, _Float32>& speed);

    const _Float32 getRadius() const;
    const std::pair<_Float32, _Float32>& getCenter() const;
    const SDL_Color& getColor() const;
    const std::pair<_Float32, _Float32>& getSpeed() const;

    void setRadius(const _Float32 new_radius);
    void setCenter(const std::pair<_Float32, _Float32>& new_center);
    void setColor(const SDL_Color& new_color);
    void setSpeed(const std::pair<_Float32, _Float32>& new_speed);

    void update(const uint64_t delta_time);
    const bool resolveCollisionWithRectangle(const SDL_FRect& rect);
    const bool resolveCollisionWithLine(const std::pair<_Float32, _Float32>& p1, const std::pair<_Float32, _Float32>& p2);

private:
    _Float32 _radius;
    std::pair<_Float32, _Float32> _center;
    SDL_Color _color;
    std::pair<_Float32, _Float32> _speed;
};

#endif // CIRCLE_H
