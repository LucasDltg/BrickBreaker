#ifndef POWERUP_H
#define POWERUP_H

#include "../include/Circle.h"
#include <memory>

class Breakout;

/**
 * @brief The PowerUp class represents a base class for power-up objects.
 */
class PowerUp : public Circle
{
public:
    /**
     * @brief Constructor for PowerUp.
     * 
     * @param current_duration The duration of the power-up (optional).
     */
    PowerUp(const int32_t current_duration = 0);

    /**
     * @brief Applies the power-up effect.
     * 
     * This method should be overridden by subclasses to apply the specific power-up effect.
     * 
     * @param game The Breakout game instance.
     */
    virtual void applyPowerUp(Breakout &game) = 0;

    /**
     * @brief Removes the power-up effect.
     * 
     * This method should be overridden by subclasses to remove the specific power-up effect.
     * 
     * @param game The Breakout game instance.
     */
    virtual void unApplyPowerUp(Breakout &game) = 0;

    /**
     * @brief Checks if the power-up is active.
     * 
     * @return True if the power-up is active, false otherwise.
     */
    const bool isActive() const;

    /**
     * @brief Decrements the duration of the power-up.
     * 
     * @param delta_time The time elapsed since the last update.
     */
    void decrementDuration(const uint64_t delta_time);

    /**
     * @brief Retrieves the remaining duration of the power-up.
     * 
     * @return The remaining duration of the power-up.
     */
    const int64_t getDuration() const;

protected:
    int64_t _current_duration; ///< The current duration of the power-up.
    bool _active; ///< Flag indicating whether the power-up is active.
};

/**
 * @brief Initializes a specific type of power-up.
 * 
 * @param name The name of the power-up.
 * @return A unique pointer to the initialized power-up object.
 */
std::unique_ptr<PowerUp> initPowerUp(const std::string& name);

/**
 * @brief The SpeedUpPowerUp class represents a power-up that speeds up the ball.
 */
class SpeedUpPowerUp : public PowerUp
{
public:
    /**
     * @brief Constructor for SpeedUpPowerUp.
     */
    SpeedUpPowerUp();

    /**
     * @brief Applies the power-up effect.
     * 
     * Speeds up the ball.
     * 
     * @param game The Breakout game instance.
     */
    void applyPowerUp(Breakout &game) override;

    /**
     * @brief Removes the power-up effect.
     * 
     * Slows down the ball.
     * 
     * @param game The Breakout game instance.
     */
    void unApplyPowerUp(Breakout &game) override;
};

/**
 * @brief The AddBallPowerUp class represents a power-up that adds a ball.
 */
class AddBallPowerUp : public PowerUp
{
public:
    /**
     * @brief Constructor for AddBallPowerUp.
     */
    AddBallPowerUp();

    /**
     * @brief Applies the power-up effect.
     * 
     * Adds a ball to the game.
     * 
     * @param game The Breakout game instance.
     */
    void applyPowerUp(Breakout &game) override;

    /**
     * @brief Removes the power-up effect.
     * 
     * Does nothing.
     * 
     * @param game The Breakout game instance.
     */
    void unApplyPowerUp(Breakout &game) override;
};

/**
 * @brief The DuplicateBallPowerUp class represents a power-up that duplicates the current ball.
 */
class DuplicateBallPowerUp : public PowerUp
{
public:
    /**
     * @brief Constructor for DuplicateBallPowerUp.
     */
    DuplicateBallPowerUp();

    /**
     * @brief Applies the power-up effect.
     * 
     * Duplicates the current ball.
     * 
     * @param game The Breakout game instance.
     */
    void applyPowerUp(Breakout &game) override;

    /**
     * @brief Removes the power-up effect.
     * 
     * Does nothing.
     * 
     * @param game The Breakout game instance.
     */
    void unApplyPowerUp(Breakout &game) override;
};

/**
 * @brief The ExtendPaddlePowerUp class represents a power-up that extends the Paddle size.
 */
class ExtendPaddlePowerUp : public PowerUp
{
public:
    /**
     * @brief Constructor for ExtendPaddlePowerUp.
     */
    ExtendPaddlePowerUp();

    /**
     * @brief Applies the power-up effect.
     * 
     * Extends the Paddle size.
     * 
     * @param game The Breakout game instance.
     */
    void applyPowerUp(Breakout &game) override;

    /**
     * @brief Removes the power-up effect.
     * 
     * Shrinks the Paddle size.
     * 
     * @param game The Breakout game instance.
     */
    void unApplyPowerUp(Breakout &game) override;
};

#endif // POWERUP_H
