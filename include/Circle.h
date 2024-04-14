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
    Circle(_Float32 radius, std::pair<_Float32, _Float32> center, SDL_Color color, std::pair<_Float32, _Float32> speed, std::string image_path = "");

    _Float32 getRadius() const;
    const std::pair<_Float32, _Float32>& getCenter() const;
    SDL_Color getColor() const;
    std::pair<_Float32, _Float32> getSpeed() const;
    std::shared_ptr<SDL_Surface> getSurface() const;

    void setRadius(_Float32 newRadius);
    void setCenter(std::pair<_Float32, _Float32> newCenter);
    void setColor(SDL_Color newColor);
    void setSpeed(std::pair<_Float32, _Float32> newSpeed);

    void update(uint64_t delta_time);
    bool resolveCollisionWithRectangle(const SDL_Rect& rect);
    bool resolveCollisionWithLine(std::pair<_Float32, _Float32> p1, std::pair<_Float32, _Float32> p2);

private:
    _Float32 radius;
    std::pair<_Float32, _Float32> center;
    SDL_Color color;
    std::pair<_Float32, _Float32> speed;
    std::shared_ptr<SDL_Surface> surface;
};

#endif // CIRCLE_H
