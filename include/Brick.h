#ifndef BRICK_H
#define BRICK_H

#include <SDL2/SDL.h>

class Brick
{
public:
    Brick(int x, int y, uint32_t color, int resistance);

    int getResistance() const;
    void decreaseResistance();

    void setPosition(int x, int y);
    const std::pair<uint32_t, uint32_t>& getPosition() const;

    const uint32_t& getColor() const;
    const SDL_Rect& getRect() const;


private:
    std::pair<uint32_t, uint32_t> position;
    SDL_Rect rect;
    uint32_t color;
    int resistance;
};

#endif // BRICK_H
