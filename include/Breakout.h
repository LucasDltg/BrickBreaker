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
    Breakout(const std::string& filename);

    /**
     * @brief Handles SDL events.
     * 
     * @param event The SDL event to handle.
     * @param data1 Additional shared data for event handling.
     * @param data2 Additional shared data for event handling.
     */
    void handleEvents(const SDL_Event& event, const std::shared_ptr<void>& data1, const std::shared_ptr<void>& data2) override;

    /**
     * @brief Updates the game state.
     * 
     * @param delta_time The time elapsed since the last update.
     */
    void update(uint64_t delta_time) override;

    /**
     * @brief Renders the game screen.
     * 
     * @return A shared pointer to the SDL_Surface representing the rendered screen.
     */
    const std::shared_ptr<SDL_Surface> render() override;

    /**
     * @brief Initializes the game surface.
     */
    void initSurface() override;
    
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

private:
    std::vector<std::unique_ptr<Brick>> _bricks; /**< Vector of bricks in the game. */
    std::vector<Ball> _balls; /**< Vector of balls in the game. */
    std::vector<std::unique_ptr<PowerUp>> _power_ups; /**< Vector of power-ups in the game. */
    Paddle _paddle; /**< The Paddle in the game. */
    std::pair<int32_t, int32_t> _grid_dimensions; /**< The dimensions of the game grid. */
    BrickShape _brick_shape; /**< The shape of the bricks in the game. */
    int32_t _start_duration; /**< The duration of the game start sequence. */
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _font; /**< The font used for rendering text. */
    
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
