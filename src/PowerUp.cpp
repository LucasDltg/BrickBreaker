#include "../include/PowerUp.h"
#include "../include/BrickBreaker.h"
#include <memory>
#include <iostream>

std::unique_ptr<PowerUp> initPowerUp(std::string name)
{
    if (name == "SpeedUp")
        return std::make_unique<SpeedUpPowerUp>();
    else if (name == "MultiBall")
        return std::make_unique<MultiBallPowerUp>();
    else if (name == "ExtendPlatform")
        return std::make_unique<ExtendPlatformPowerUp>();
    else
        return nullptr;
}

PowerUp::PowerUp(int32_t current_duration, std::string image_path)
: Circle(image_path), current_duration(current_duration), active(false)
{}

bool PowerUp::isActive() const
{
    return active;
}

void PowerUp::decrementDuration(uint64_t delta_time)
{
    if (active)
        current_duration -= delta_time;
}

int64_t PowerUp::getDuration() const
{
    return current_duration;
}

MultiBallPowerUp::MultiBallPowerUp()
: PowerUp()
{}

void MultiBallPowerUp::applyPowerUp(BrickBreaker &game)
{
    active = true;
    
    SDL_Rect platformRect = game.getPlatform().getRect();
    Ball ball = Ball(game.getBallRadius(), std::pair<_Float32, _Float32>{static_cast<_Float32>(platformRect.x) + static_cast<_Float32>(platformRect.w) / 2.0f, 
                    static_cast<_Float32>(platformRect.y) - game.getBallRadius()}, SDL_Color{255, 0, 0, 0}, std::pair<_Float32, _Float32>{0, game.getInitialBallSpeed()});

    game.addBall(ball);
}

void MultiBallPowerUp::unApplyPowerUp(BrickBreaker &game)
{}

ExtendPlatformPowerUp::ExtendPlatformPowerUp()
: PowerUp(5000, "./assets/textures/bubble_extend.png")
{}

void ExtendPlatformPowerUp::applyPowerUp(BrickBreaker &game)
{
    active = true;
    Platform& p = game.getPlatform();
    p.setRect({p.getRect().x, p.getRect().y, static_cast<int32_t>(p.getRect().w * 1.5f), p.getRect().h});
}

void ExtendPlatformPowerUp::unApplyPowerUp(BrickBreaker &game)
{
    Platform& p = game.getPlatform();
    p.setRect({p.getRect().x, p.getRect().y, static_cast<int32_t>(p.getRect().w / 1.5f), p.getRect().h});
}

SpeedUpPowerUp::SpeedUpPowerUp()
: PowerUp(5000, "./assets/textures/bubble_speed.png")
{}

void SpeedUpPowerUp::applyPowerUp(BrickBreaker &game)
{
    active = true;
    for (auto &ball : game.getBalls())
    {
        ball.setSpeed({ball.getSpeed().first * 1.2f, ball.getSpeed().second * 1.2f});
    }
}

void SpeedUpPowerUp::unApplyPowerUp(BrickBreaker &game)
{
    for (auto &ball : game.getBalls())
    {
        ball.setSpeed({ball.getSpeed().first / 1.2f, ball.getSpeed().second / 1.2f});
    }
}