#ifndef BRICK_H
#define BRICK_H

#include <SDL2/SDL.h>

class Brick
{
public:
    static const int WIDTH = 50;
    static const int HEIGHT = 20;
    Brick(int x, int y, uint32_t color, int resistance);

    int getResistance() const;

    void decreaseResistance();

    void setPosition(int x, int y);
    const std::pair<uint32_t, uint32_t>& getPosition() const;

    const uint32_t& getColor() const;


private:
    std::pair<uint32_t, uint32_t> position;
    uint32_t color;
    int resistance;
};

#endif // BRICK_H
