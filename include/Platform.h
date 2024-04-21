#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <memory>
#include <string>

class Platform
{
public:
    Platform(std::string image_path = std::string("./assets/textures/platform.png"));

    // Setters and getters
    void setRect(const SDL_FRect& rect);
    void setColor(const SDL_Color& color);
    const SDL_FRect& getRect() const;
    SDL_FRect& getRect();
    const SDL_Color getColor() const;
    void setSpeedX(_Float32 speed_x);
    void update(uint64_t delta_time, int screen_width);
    std::shared_ptr<SDL_Surface> getSurface() const;
    const int32_t getSpeedX() const;

private:
    SDL_FRect rect;
    SDL_Color color;
    std::shared_ptr<SDL_Surface> surface;
    _Float32 speed_x;
};

#endif // PLATFORM_H
