#include "../include/PowerUp.h"
#include "../include/BrickBreaker.h"
#include <memory>
#include <iostream>

std::unique_ptr<PowerUp> initPowerUp(std::string name)
{
    if (name == "SpeedUp")
        return std::make_unique<SpeedUpPowerUp>();
    else if (name == "AddBall")
        return std::make_unique<AddBallPowerUp>();
    else if (name == "DuplicateBall")
        return std::make_unique<DuplicateBallPowerUp>();
    else if (name == "ExtendPlatform")
        return std::make_unique<ExtendPlatformPowerUp>();
    else
        return nullptr;
}

PowerUp::PowerUp(int32_t current_duration)
: Circle(), _current_duration(current_duration), _active(false)
{}

bool PowerUp::isActive() const
{
    return _active;
}

void PowerUp::decrementDuration(uint64_t delta_time)
{
    if (_active)
        _current_duration -= delta_time;
}

int64_t PowerUp::getDuration() const
{
    return _current_duration;
}

AddBallPowerUp::AddBallPowerUp()
: PowerUp(0)
{}

void AddBallPowerUp::applyPowerUp(BrickBreaker &game)
{
    _active = true;
    
    const SDL_FRect& platform_rect = game.getPlatform().getRect();
    Ball ball = Ball(game.getBallRadius(), std::pair<_Float32, _Float32>{static_cast<_Float32>(platform_rect.x) + static_cast<_Float32>(platform_rect.w) / 2.0f, 
                    static_cast<_Float32>(platform_rect.y) - game.getBallRadius()}, SDL_Color{255, 0, 0, 0}, std::pair<_Float32, _Float32>{0, game.getInitialBallSpeed()});

    game.addBall(ball);
}

void AddBallPowerUp::unApplyPowerUp(BrickBreaker &game)
{}

DuplicateBallPowerUp::DuplicateBallPowerUp()
: PowerUp(0)
{}

void DuplicateBallPowerUp::applyPowerUp(BrickBreaker &game)
{
    _active = true;
    
    for (auto &ball : game.getBalls()) {
        
        Ball duplicate_ball = Ball(ball.getRadius(), ball.getCenter(), ball.getColor(), ball.getSpeed());
        
        double original_angle = atan2(ball.getSpeed().second, ball.getSpeed().first);
        double new_angle = original_angle + (30 * M_PI / 180); // Add 30° to the original angle
        double speed_magnitude = sqrt(pow(ball.getSpeed().first, 2) + pow(ball.getSpeed().second, 2));
        double new_speed_x = cos(new_angle) * speed_magnitude;
        double new_speed_y = sin(new_angle) * speed_magnitude;
        
        duplicate_ball.setSpeed(std::make_pair(new_speed_x, new_speed_y));
        game.addBall(duplicate_ball);
    }
}

void DuplicateBallPowerUp::unApplyPowerUp(BrickBreaker &game)
{}

ExtendPlatformPowerUp::ExtendPlatformPowerUp()
: PowerUp(5000)
{}

void ExtendPlatformPowerUp::applyPowerUp(BrickBreaker &game)
{
    _active = true;
    Platform& p = game.getPlatform();
    p.setRect({p.getRect().x , p.getRect().y, p.getRect().w * 1.5f, p.getRect().h});
    // p.setRect({static_cast<int32_t>(p.getRect().x - (p.getRect().w * 0.25f)), p.getRect().y, static_cast<int32_t>(p.getRect().w * 1.5f), p.getRect().h});
}

void ExtendPlatformPowerUp::unApplyPowerUp(BrickBreaker &game)
{
    Platform& p = game.getPlatform();
    p.setRect({p.getRect().x , p.getRect().y, p.getRect().w / 1.5f, p.getRect().h});
    // p.setRect({static_cast<int32_t>(p.getRect().x + (p.getRect().w * 0.25f)), p.getRect().y, static_cast<int32_t>(p.getRect().w / 1.5f), p.getRect().h});
}

SpeedUpPowerUp::SpeedUpPowerUp()
: PowerUp(5000)
{}

void SpeedUpPowerUp::applyPowerUp(BrickBreaker &game)
{
    _active = true;
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