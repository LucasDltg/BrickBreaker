#ifndef POWERUP_H
#define POWERUP_H

#include <cstdint>
#include <string>
#include <memory>
#include "../include/Circle.hpp"

class BrickBreaker;

class PowerUp : public Circle
{
public:
    PowerUp();
    virtual void applyPowerUp(BrickBreaker &game) = 0;
};

std::unique_ptr<PowerUp> initPowerUp(std::string name);

class SpeedUpPowerUp : public PowerUp
{
    public:
    void applyPowerUp(BrickBreaker &game) override;
};

class MultiBallPowerUp : public PowerUp
{
    public:
    void applyPowerUp(BrickBreaker &game) override;
};

class ExtraLifePowerUp : public PowerUp
{
    public:
    void applyPowerUp(BrickBreaker &game) override;
};

#endif // POWERUP_H