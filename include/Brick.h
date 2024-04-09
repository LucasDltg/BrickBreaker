#ifndef BRICK_H
#define BRICK_H

#include <SDL2/SDL.h>

class Brick
{
public:
    static const int WIDTH = 50;
    static const int HEIGHT = 20;
    Brick(int x, int y, int width, int height, SDL_Color color, int resistance);

    int getResistance() const;

    void decreaseResistance();

    const SDL_Rect& getRect() const;

    const SDL_Color& getColor() const;


private:
    SDL_Rect rect;
    SDL_Color color;
    int resistance;
};

#endif // BRICK_H
