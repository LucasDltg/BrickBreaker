#include "../include/PowerUp.h"
#include "../include/BrickBreaker.h"
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
{}

void SpeedUpPowerUp::applyPowerUp(BrickBreaker &game)
{
    /*for (auto &ball : game.getBalls())
    {
        ball->setSpeed(ball->getSpeed() * 1.5f);
    }   */
}

void MultiBallPowerUp::applyPowerUp(BrickBreaker &game)
{
    SDL_Rect platformRect = game.getPlatform().getRect();
    Ball ball = Ball(game.getBallRadius(), std::pair<_Float32, _Float32>{static_cast<_Float32>(platformRect.x) + static_cast<_Float32>(platformRect.w) / 2.0f, 
                    static_cast<_Float32>(platformRect.y) - game.getBallRadius()}, SDL_Color{255, 0, 0, 0}, std::pair<_Float32, _Float32>{0, game.getInitialBallSpeed()});

    game.addBall(ball);
}

void ExtraLifePowerUp::applyPowerUp(BrickBreaker &game)
{

}