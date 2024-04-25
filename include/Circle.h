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
    Circle(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed);

    _Float32 getRadius() const;
    const std::pair<_Float32, _Float32>& getCenter() const;
    SDL_Color getColor() const;
    std::pair<_Float32, _Float32> getSpeed() const;

    void setRadius(_Float32 new_radius);
    void setCenter(std::pair<_Float32, _Float32> new_center);
    void setColor(SDL_Color new_color);
    void setSpeed(std::pair<_Float32, _Float32> new_speed);

    void update(uint64_t delta_time);
    bool resolveCollisionWithRectangle(const SDL_FRect& rect);
    bool resolveCollisionWithLine(std::pair<_Float32, _Float32> p1, std::pair<_Float32, _Float32> p2);

private:
    _Float32 _radius;
    std::pair<_Float32, _Float32> _center;
    SDL_Color _color;
    std::pair<_Float32, _Float32> _speed;
};

#endif // CIRCLE_H
