#ifndef BREAKOUT_H
#define BREAKOUT_H

#include "../include/SDLComponent.h"
#include "../include/Brick.h"
#include "../include/Ball.h"
#include "../include/Paddle.h"
#include <fstream>
#include <SDL2/SDL_ttf.h>
#include <iomanip>
#include <algorithm>

/**
 * @brief The Breakout class represents the main game logic for the Brick Breaker game.
 */
class Breakout : public SDLComponent {
public:
    /**
     * @brief Represents the maximum surface occupied by bricks.
     */
    static constexpr _Float32 _BRICK_HEIGHT_LIMIT = 0.4f;

    /**
     * @brief Constructor for Breakout.
     * 
     * @param filename The filename of the level configuration file.
     */
    Breakout(const std::string& filename, bool run = false, bool is_background = false);

    /**
     * @brief Handles SDL events.
     * 
     */
    void handleEvents() override;

    /**
     * @brief Updates the game state.
     * 
     * @param delta_time The time elapsed since the last update.
     */
    void update(uint64_t delta_time) override;

    /**
     * @brief Renders the game screen.
     * 
     * @param renderer The SDL renderer to render the screen with.
     * @return A shared pointer to the SDL_Surface representing the rendered screen.
     */
    void render(const std::shared_ptr<SDL_Renderer> renderer) override;

    /**
     * @brief Initializes the game surface.
     * 
     * @param renderer The SDL renderer to initialize the surface with.
     */
    void initSurface(const std::shared_ptr<SDL_Renderer> renderer) override;
    
    /**
     * @brief Adds a ball to the game.
     * 
     * @param ball The ball to add.
     */
    void addBall(const Ball& ball);
    
    /**
     * @brief Get the Paddle object.
     * 
     * @return A const reference to the Paddle object.
     */
    const Paddle& getPaddle() const;

    /**
     * @brief Get the Paddle object.
     * 
     * @return A reference to the Paddle object.
     */
    Paddle& getPaddle();

    /**
     * @brief Get the radius of the ball.
     * 
     * @return The radius of the ball.
     */
    _Float32 getBallRadius() const;

    /**
     * @brief Get the initial speed of the ball.
     * 
     * @return The initial speed of the ball.
     */
    _Float32 getInitialBallSpeed() const;

    /**
     * @brief Get the initial speed of the Paddle.
     * 
     * @return The initial speed of the Paddle.
     */
    _Float32 getInitialPaddleSpeed() const;

    /**
     * @brief Get the vector of balls.
     * 
     * @return A reference to the vector of balls.
     */
    std::vector<Ball>& getBalls();

    /**
     * @brief Increases the flip renderer count.
     * 
     */
    void increaseFlipRenderer();

    /**
     * @brief Decreases the flip renderer count.
     * 
     */
    void decreaseFlipRenderer();

    /**
     * @brief Get the flip renderer count.
     * 
     * @return The flip renderer count.
     */
    int32_t getFlipRenderer() const;

private:
    std::vector<std::unique_ptr<Brick>> _bricks; /**< Vector of bricks in the game. */
    std::vector<Ball> _balls; /**< Vector of balls in the game. */
    std::vector<std::unique_ptr<PowerUp>> _power_ups; /**< Vector of power-ups in the game. */
    Paddle _paddle; /**< The Paddle in the game. */
    std::pair<int32_t, int32_t> _grid_dimensions; /**< The dimensions of the game grid. */
    BrickShape _brick_shape; /**< The shape of the bricks in the game. */
    int32_t _start_duration; /**< The duration of the game start sequence. */
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _font; /**< The font used for rendering text. */
    bool _is_background; /**< Whether the game is in the background. */
    int32_t _is_rendered_flipped; /**< Whether the game is rendered flipped. */
    
    /**
     * @brief Creates bricks from the level configuration file.
     * 
     * @param filename The filename of the level configuration file.
     */
    void createBricksFromLevel(const std::string& filename);

    /**
     * @brief Handles resizing of the game window.
     * 
     * @param previous_size The previous size of the game window.
     * @param new_size The new size of the game window.
     */
    void handleResize(const std::pair<int32_t, int32_t>& previous_size, const std::pair<int32_t, int32_t>& new_size);

    /**
     * @brief Updates the game loop.
     * 
     * @param delta_time The time elapsed since the last update.
     */
    void updateLoop(int64_t delta_time);
};

#endif // BREAKOUT_H
