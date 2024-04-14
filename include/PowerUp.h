#ifndef POWERUP_H
#define POWERUP_H

#include <cstdint>
#include <string>
#include <memory>
#include "../include/Circle.h"

class BrickBreaker;

class PowerUp : public Circle
{
public:
    PowerUp(int32_t current_duration = 0);
    virtual void applyPowerUp(BrickBreaker &game) = 0;
    virtual void unApplyPowerUp(BrickBreaker &game) = 0;
    bool isActive() const;
    void decrementDuration(uint64_t delta_time);
    int64_t getDuration() const;

protected:
    int64_t current_duration;
    bool active;
};

std::unique_ptr<PowerUp> initPowerUp(std::string name);

class SpeedUpPowerUp : public PowerUp
{
    public:
    SpeedUpPowerUp();
    void applyPowerUp(BrickBreaker &game) override;
    void unApplyPowerUp(BrickBreaker &game) override;
};

class MultiBallPowerUp : public PowerUp
{
    public:
    MultiBallPowerUp();
    void applyPowerUp(BrickBreaker &game) override;
    void unApplyPowerUp(BrickBreaker &game) override;
};

class ExtendPlatformPowerUp : public PowerUp
{
    public:
    ExtendPlatformPowerUp();
    void applyPowerUp(BrickBreaker &game) override;
    void unApplyPowerUp(BrickBreaker &game) override;
};

#endif // POWERUP_H