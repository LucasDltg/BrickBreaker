#ifndef BRICK_H
#define BRICK_H

#include "../include/PowerUp.h"
#include <vector>
#include <string>
#include <memory>

/**
 * @brief Enumeration representing different shapes of bricks.
 */
enum class BrickShape {RECTANGLE, HEXAGON, TRIANGLE};

/**
 * @brief Represents a brick in Breakout.
 */
class Brick
{
public:
    /**
     * @brief Constructor for Brick.
     * 
     * @param position The position of the brick in grid coordinates.
     * @param grid_dimensions The dimensions of the grid.
     * @param surface_size The size of the surface.
     * @param color The color of the brick.
     * @param resistance The resistance of the brick.
     * @param power_up_name The name of the power-up associated with the brick.
     */
    Brick(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name);

    // Member functions
    /**
     * @brief Get the resistance of the brick.
     * 
     * @return The resistance of the brick.
     */
    int32_t getResistance() const;

    /**
     * @brief Decrease the resistance of the brick.
     */
    void decreaseResistance();

    /**
     * @brief Get the resistance percentage of the brick.
     * 
     * @return The resistance percentage of the brick.
     */
    _Float32 getResistancePercentage() const;

    /**
     * @brief Get the color of the brick.
     * 
     * @return The color of the brick.
     */
    uint32_t getColor() const;

    /**
     * @brief Get the power-up associated with the brick.
     * 
     * @return A pointer to the power-up associated with the brick.
     */
    const std::unique_ptr<PowerUp> getPowerUp();

    /**
     * @brief Get the vertices of the brick.
     * 
     * @return The vertices of the brick.
     */
    const std::vector<SDL_Vertex>& getVertices() const;

    /**
     * @brief Get the vertices of the brick without color information.
     * 
     * @return The vertices of the brick without color information.
     */
    const std::vector<SDL_Vertex> getVerticesWithoutColor() const;

    /**
     * @brief Get the indices of the brick.
     * 
     * @return The indices of the brick.
     */
    virtual const std::vector<int32_t> getIndices() const = 0;

    /**
     * @brief Calculate the vertices of the brick with its position.
     * 
     * @param grid_dimensions The dimensions of the grid.
     * @param surface_size The size of the surface.
     */
    virtual void calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size) = 0;

    /**
     * @brief Get the center position of the brick.
     * 
     * @return The center position of the brick.
     */
    virtual const std::pair<_Float32, _Float32> getCenter() const = 0;

protected:
    std::pair<uint32_t, uint32_t> _position; ///< The position of the brick.
    std::vector<SDL_Vertex> _vertices; ///< The vertices of the brick.
    uint32_t _color; ///< The color of the brick.
    int32_t _resistance; ///< The resistance of the brick.
    int32_t _max_resistance; ///< The maximum resistance of the brick.
    std::unique_ptr<PowerUp> _powerUp; ///< The power-up associated with the brick.

    /**
     * @brief Generates tint and shade colors for the vertices.
     * 
     * @param original_color The original color of the brick.
     * @param vertices_count The number of vertices.
     * @return std::vector<SDL_Color> The generated tint and shade colors.
     */
    const std::vector<SDL_Color> GenerateTintAndShadeColors(const SDL_Color& original_color, const uint32_t vertices_count);
};

/**
 * @brief Represents a rectangular brick.
 */
class BrickRectangular : public Brick
{
public:
    /**
     * @brief Constructor for BrickRectangular.
     * 
     * @param position The position of the brick.
     * @param grid_dimensions The dimensions of the grid.
     * @param surface_size The size of the surface.
     * @param color The color of the brick.
     * @param resistance The resistance of the brick.
     * @param power_up_name The name of the power-up associated with the brick.
     */
    BrickRectangular(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name);

    /**
     * @brief Calculate the vertices of the rectangular brick with its position.
     * 
     * @param grid_dimensions The dimensions of the grid.
     * @param surface_size The size of the surface.
     */
    void calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size) override;

    /**
     * @brief Get the indices of the rectangular brick.
     * 
     * @return The indices of the rectangular brick.
     */
    const std::vector<int32_t> getIndices() const override;

    /**
     * @brief Get the center position of the rectangular brick.
     * 
     * @return The center position of the rectangular brick.
     */
    const std::pair<_Float32, _Float32> getCenter() const override;
};

/**
 * @brief Represents a triangular brick.
 */
class BrickTriangular : public Brick
{
public:
    /**
     * @brief Constructor for BrickTriangular.
     * 
     * @param position The position of the brick.
     * @param grid_dimensions The dimensions of the grid.
     * @param surface_size The size of the surface.
     * @param color The color of the brick.
     * @param resistance The resistance of the brick.
     * @param power_up_name The name of the power-up associated with the brick.
     */
    BrickTriangular(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name);

    /**
     * @brief Calculate the vertices of the triangular brick with its position.
     * 
     * @param grid_dimensions The dimensions of the grid.
     * @param surface_size The size of the surface.
     */
    void calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size) override;

    /**
     * @brief Get the indices of the triangular brick.
     * 
     * @return The indices of the triangular brick.
     */
    const std::vector<int32_t> getIndices() const override;

    /**
     * @brief Get the center position of the triangular brick.
     * 
     * @return The center position of the triangular brick.
     */
    const std::pair<_Float32, _Float32> getCenter() const override;
};

/**
 * @brief Represents a hexagonal brick.
 */
class BrickHexagonal : public Brick
{
public:
    /**
     * @brief Constructor for BrickHexagonal.
     * 
     * @param position The position of the brick.
     * @param grid_dimensions The dimensions of the grid.
     * @param surface_size The size of the surface.
     * @param color The color of the brick.
     * @param resistance The resistance of the brick.
     * @param power_up_name The name of the power-up associated with the brick.
     */
    BrickHexagonal(const std::pair<uint32_t, uint32_t>& position, const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size, const uint32_t color, const int32_t resistance, const std::string& power_up_name);

    /**
     * @brief Calculate the vertices of the hexagonal brick with its position.
     * 
     * @param grid_dimensions The dimensions of the grid.
     * @param surface_size The size of the surface.
     */
    void calculateVerticesWithPosition(const std::pair<_Float32, _Float32>& grid_dimensions, const std::pair<_Float32, _Float32>& surface_size) override;

    /**
     * @brief Get the indices of the hexagonal brick.
     * 
     * @return The indices of the hexagonal brick.
     */
    const std::vector<int32_t> getIndices() const override;

    /**
     * @brief Get the center position of the hexagonal brick.
     * 
     * @return The center position of the hexagonal brick.
     */
    const std::pair<_Float32, _Float32> getCenter() const override;
};

#endif // BRICK_H
