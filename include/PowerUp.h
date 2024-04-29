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
    PowerUp(const int32_t current_duration = 0);
    virtual void applyPowerUp(BrickBreaker &game) = 0;
    virtual void unApplyPowerUp(BrickBreaker &game) = 0;
    const bool isActive() const;
    void decrementDuration(const uint64_t delta_time);
    const int64_t getDuration() const;

protected:
    int64_t _current_duration;
    bool _active;
};

std::unique_ptr<PowerUp> initPowerUp(const std::string& name);

class SpeedUpPowerUp : public PowerUp
{
    public:
    SpeedUpPowerUp();
    void applyPowerUp(BrickBreaker &game) override;
    void unApplyPowerUp(BrickBreaker &game) override;
};

class AddBallPowerUp : public PowerUp
{
    public:
    AddBallPowerUp();
    void applyPowerUp(BrickBreaker &game) override;
    void unApplyPowerUp(BrickBreaker &game) override;
};

class DuplicateBallPowerUp : public PowerUp
{
    public:
    DuplicateBallPowerUp();
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