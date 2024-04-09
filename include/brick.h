#ifndef BRICK_H
#define BRICK_H

#include <SDL2/SDL.h>

class Brick
{
public:
    Brick(SDL_Renderer* renderer, int x, int y, int width, int height, SDL_Color color, int resistance);

    void render();

    int getResistance() const;

    void decreaseResistance();

private:
    SDL_Renderer* renderer;
    SDL_Rect rect;
    SDL_Color color;
    int resistance;
};

#endif // BRICK_H
