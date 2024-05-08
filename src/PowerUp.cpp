#include "../include/PowerUp.h"
#include "../include/Breakout.h"

std::unique_ptr<PowerUp> initPowerUp(const std::string& name)
{
    if (name == "SpeedUp")
        return std::make_unique<SpeedUpPowerUp>();
    else if (name == "AddBall")
        return std::make_unique<AddBallPowerUp>();
    else if (name == "DuplicateBall")
        return std::make_unique<DuplicateBallPowerUp>();
    else if (name == "ExtendPaddle")
        return std::make_unique<ExtendPaddlePowerUp>();
    else
        return nullptr;
}

PowerUp::PowerUp(const int32_t current_duration)
: Circle(), _current_duration(current_duration), _active(false)
{}

const bool PowerUp::isActive() const
{
    return _active;
}

void PowerUp::decrementDuration(uint64_t delta_time)
{
    if (_active)
        _current_duration -= delta_time;
}

const int64_t PowerUp::getDuration() const
{
    return _current_duration;
}

AddBallPowerUp::AddBallPowerUp()
: PowerUp(0)
{}

void AddBallPowerUp::applyPowerUp(Breakout &game)
{
    _active = true;
    
    const SDL_FRect& paddle_rect = game.getPaddle().getRect();
    Ball ball = Ball(game.getBallRadius(), std::pair<_Float32, _Float32>{static_cast<_Float32>(paddle_rect.x) + static_cast<_Float32>(paddle_rect.w) / 2.0f, 
                    static_cast<_Float32>(paddle_rect.y) - game.getBallRadius()}, std::pair<_Float32, _Float32>{0, game.getInitialBallSpeed()});

    game.addBall(ball);
}

void AddBallPowerUp::unApplyPowerUp(Breakout &game)
{}

DuplicateBallPowerUp::DuplicateBallPowerUp()
: PowerUp(0)
{}

void DuplicateBallPowerUp::applyPowerUp(Breakout &game)
{
    _active = true;
    
    for (auto &ball : game.getBalls()) {
        
        Ball duplicate_ball = Ball(ball.getRadius(), ball.getCenter(), ball.getSpeed());
        
        double original_angle = atan2(ball.getSpeed().second, ball.getSpeed().first);
        double new_angle = original_angle + (30 * M_PI / 180); // Add 30° to the original angle
        double speed_magnitude = sqrt(pow(ball.getSpeed().first, 2) + pow(ball.getSpeed().second, 2));
        double new_speed_x = cos(new_angle) * speed_magnitude;
        double new_speed_y = sin(new_angle) * speed_magnitude;
        
        duplicate_ball.setSpeed(std::make_pair(new_speed_x, new_speed_y));
        game.addBall(duplicate_ball);
    }
}

void DuplicateBallPowerUp::unApplyPowerUp(Breakout &game)
{}

ExtendPaddlePowerUp::ExtendPaddlePowerUp()
: PowerUp(5000)
{}

void ExtendPaddlePowerUp::applyPowerUp(Breakout &game)
{
    _active = true;
    Paddle& p = game.getPaddle();
    p.setRect({p.getRect().x - (p.getRect().w * 0.25f), p.getRect().y, p.getRect().w * 1.5f, p.getRect().h});
}

void ExtendPaddlePowerUp::unApplyPowerUp(Breakout &game)
{
    Paddle& p = game.getPaddle();
    p.setRect({p.getRect().x + (p.getRect().w * 0.25f), p.getRect().y, p.getRect().w / 1.5f, p.getRect().h});
}

SpeedUpPowerUp::SpeedUpPowerUp()
: PowerUp(5000)
{}

void SpeedUpPowerUp::applyPowerUp(Breakout &game)
{
    _active = true;
    for (auto &ball : game.getBalls())
    {
        ball.setSpeed({ball.getSpeed().first * 1.2f, ball.getSpeed().second * 1.2f});
    }
}

void SpeedUpPowerUp::unApplyPowerUp(Breakout &game)
{
    for (auto &ball : game.getBalls())
    {
        ball.setSpeed({ball.getSpeed().first / 1.2f, ball.getSpeed().second / 1.2f});
    }
}