#include "../include/PowerUp.h"
#include <memory>
#include <iostream>

std::unique_ptr<PowerUp> initPowerUp(std::string name)
{
    std::cout << "Creating power up: " << name << std::endl;
    if (name == "SpeedUp")
        return std::make_unique<SpeedUpPowerUp>();
    else if (name == "MultiBall")
        return std::make_unique<MultiBallPowerUp>();
    else if (name == "ExtraLife")
        return std::make_unique<ExtraLifePowerUp>();
    else
        return nullptr;
}


PowerUp::PowerUp()
{
    // Initialize the power up
}

void SpeedUpPowerUp::applyPowerUp()
{
    // Increase the speed of the ball
}

void SpeedUpPowerUp::removePowerUp()
{
    // Decrease the speed of the ball
}

void MultiBallPowerUp::applyPowerUp()
{
    
}

void MultiBallPowerUp::removePowerUp()
{
    // Remove the extra ball
}

void ExtraLifePowerUp::applyPowerUp()
{
    // Increase the number of lives
}

void ExtraLifePowerUp::removePowerUp()
{
    // Decrease the number of lives
}

