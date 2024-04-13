#ifndef POWERUP_H
#define POWERUP_H

#include <cstdint>
#include <string>
#include <memory>
#include "../include/Circle.hpp"

class PowerUp : public Circle
{
public:
    PowerUp();
    virtual void applyPowerUp() = 0;
    virtual void removePowerUp() = 0;
};

std::unique_ptr<PowerUp> initPowerUp(std::string name);

class SpeedUpPowerUp : public PowerUp
{
    public:
    void applyPowerUp() override;
    void removePowerUp() override;
};

class MultiBallPowerUp : public PowerUp
{
    public:
    void applyPowerUp() override;
    void removePowerUp() override;
};

class ExtraLifePowerUp : public PowerUp
{
    public:
    void applyPowerUp() override;
    void removePowerUp() override;
};

#endif // POWERUP_H