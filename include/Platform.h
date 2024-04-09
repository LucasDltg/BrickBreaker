#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>

class Platform
{
public:
    Platform(int pos_x, int pos_y, int width, int height, SDL_Color color);

    // Setters and getters
    void setRect(const SDL_Rect& rect);
    void setColor(const SDL_Color& color);
    const SDL_Rect& getRect() const;
    const SDL_Color getColor() const;
    void setSpeedX(int32_t speed_x);
    void update(uint64_t delta_time, int screen_width);

private:
    SDL_Rect rect;
    SDL_Color color;
    int32_t speed_x;
};

#endif // PLATFORM_H
