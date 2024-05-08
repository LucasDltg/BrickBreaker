#ifndef CIRCLE_H
#define CIRCLE_H

#include <utility>
#include <cstdint>
#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/**
 * @brief The Circle class represents a circle in 2D space.
 */
class Circle {
public:
    /**
     * @brief Default constructor for Circle.
     */
    Circle();

    /**
     * @brief Constructor for Circle.
     * 
     * @param radius The radius of the circle.
     * @param center The center coordinates of the circle.
     * @param speed The speed of the circle.
     */
    Circle(const _Float32 radius, const std::pair<_Float32, _Float32>& center, const std::pair<_Float32, _Float32>& speed);

    /**
     * @brief Retrieves the radius of the circle.
     * 
     * @return The radius of the circle.
     */
    const _Float32 getRadius() const;

    /**
     * @brief Retrieves the center coordinates of the circle.
     * 
     * @return The center coordinates of the circle.
     */
    const std::pair<_Float32, _Float32>& getCenter() const;

    /**
     * @brief Retrieves the speed of the circle.
     * 
     * @return The speed of the circle.
     */
    const std::pair<_Float32, _Float32>& getSpeed() const;

    /**
     * @brief Sets the radius of the circle.
     * 
     * @param new_radius The new radius of the circle.
     */
    void setRadius(const _Float32 new_radius);

    /**
     * @brief Sets the center coordinates of the circle.
     * 
     * @param new_center The new center coordinates of the circle.
     */
    void setCenter(const std::pair<_Float32, _Float32>& new_center);

    /**
     * @brief Sets the speed of the circle.
     * 
     * @param new_speed The new speed of the circle.
     */
    void setSpeed(const std::pair<_Float32, _Float32>& new_speed);

    /**
     * @brief Updates the position of the circle based on the elapsed time.
     * 
     * @param delta_time The time elapsed since the last update.
     */
    void update(const uint64_t delta_time);

    /**
     * @brief Resolves collision between the circle and a rectangle.
     * 
     * @param rect The rectangle with which the collision is to be resolved.
     * @return True if collision occurred, false otherwise.
     */
    const bool resolveCollisionWithRectangle(const SDL_FRect& rect);

    /**
     * @brief Resolves collision between the circle and a line segment.
     * 
     * @param p1 The starting point of the line segment.
     * @param p2 The ending point of the line segment.
     * @param delta_time The time elapsed since the last update.
     * @return True if collision occurred, false otherwise.
     */
    const bool resolveCollisionWithLine(const std::pair<_Float32, _Float32>& p1, const std::pair<_Float32, _Float32>& p2, uint64_t delta_time);

private:
    _Float32 _radius; ///< The radius of the circle.
    std::pair<_Float32, _Float32> _center; ///< The center coordinates of the circle.
    std::pair<_Float32, _Float32> _speed; ///< The speed of the circle.
};

#endif // CIRCLE_H
