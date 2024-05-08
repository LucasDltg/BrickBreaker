#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <memory>
#include <string>

/**
 * @brief The Platform class represents the platform in the game.
 */
class Platform
{
public:
    /**
     * @brief Constructor for Platform.
     */
    Platform();

    /**
     * @brief Sets the rectangle representing the platform's position and size.
     * 
     * @param rect The rectangle representing the platform's position and size.
     */
    void setRect(const SDL_FRect& rect);

    /**
     * @brief Retrieves the rectangle representing the platform's position and size.
     * 
     * @return The rectangle representing the platform's position and size.
     */
    const SDL_FRect& getRect() const;

    /**
     * @brief Retrieves the rectangle representing the platform's position and size.
     * 
     * @return The rectangle representing the platform's position and size.
     */
    SDL_FRect& getRect();

    /**
     * @brief Sets the horizontal speed of the platform.
     * 
     * @param speed_x The horizontal speed of the platform.
     */
    void setSpeedX(const _Float32 speed_x);

    /**
     * @brief Updates the position of the platform based on the elapsed time and screen width.
     * 
     * @param delta_time The time elapsed since the last update.
     * @param screen_width The width of the screen.
     */
    void update(const uint64_t delta_time, const int32_t screen_width);

    /**
     * @brief Retrieves the horizontal speed of the platform.
     * 
     * @return The horizontal speed of the platform.
     */
    const int32_t getSpeedX() const;

private:
    SDL_FRect _rect; ///< The rectangle representing the platform's position and size.
    _Float32 _speed_x; ///< The horizontal speed of the platform.
};

#endif // PLATFORM_H
