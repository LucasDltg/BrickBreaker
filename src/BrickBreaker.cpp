#include "../include/BrickBreaker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../include/Brick.h"
#include "../include/Ball.h"
#include "../include/Platform.h"

BrickBreaker::BrickBreaker(const std::string& filename)
: SDLComponent(), start_duration(3000)
{
    createBricksFromLevel(filename);
    
    font = TTF_OpenFont("./assets/fonts/arial/arial.ttf", 24);
    if (!font)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
}

void BrickBreaker::initSurface(uint32_t width, uint32_t height)
{
    setSurfaceDimensions(width, height);
    SDL_Color color;
    color.r = 0; color.g = 0; color.b = 255; color.a = 0;
    std::pair<uint32_t, uint32_t> center = {surface->w / 2, surface->h / 4};
    std::pair<_Float32, _Float32> speed = {getInitialBallSpeed()/2, getInitialBallSpeed()};
    balls.push_back(Ball(getBallRadius(), center, color, speed));

    color.r = 255; color.g = 0; color.b = 0; color.a = 0;
    platform.setRect({surface->w / 2 - surface->w / 14, surface->h * 9 / 10, surface->w / 7, surface->h / 30});
    platform.setColor(color);
}

void BrickBreaker::handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize)
{
    for (auto& ball : balls)
    {
        std::pair<_Float32, _Float32> center = ball.getCenter();

        uint32_t newX = static_cast<uint32_t>(static_cast<float>(center.first) * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
        uint32_t newY = static_cast<uint32_t>(static_cast<float>(center.second) * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second)));

        ball.setCenter({newX, newY});
        ball.setSpeed({ball.getSpeed().first * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)), ball.getSpeed().second * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second))});
        ball.setRadius(ball.getRadius() * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
    }

    for (auto& powerUp : powerUps)
    {
        std::pair<_Float32, _Float32> center = powerUp->getCenter();

        uint32_t newX = static_cast<uint32_t>(static_cast<float>(center.first) * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
        uint32_t newY = static_cast<uint32_t>(static_cast<float>(center.second) * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second)));

        powerUp->setCenter({newX, newY});
        powerUp->setSpeed({powerUp->getSpeed().first * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)), powerUp->getSpeed().second * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second))});
        powerUp->setRadius(powerUp->getRadius() * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
    }

    // Resize platform
    SDL_Rect rect = platform.getRect();
    rect.x = static_cast<uint32_t>(static_cast<float>(rect.x) * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
    rect.y = static_cast<uint32_t>(static_cast<float>(rect.y) * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second)));
    rect.w = static_cast<uint32_t>(static_cast<float>(rect.w) * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
    rect.h = static_cast<uint32_t>(static_cast<float>(rect.h) * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second)));
    platform.setRect(rect); 
}

void BrickBreaker::createBricksFromLevel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << filename << std::endl;
        return;
    }

    std::string magicSequence;
    file >> magicSequence;
    if (magicSequence == "rectangle")
    {
        brickShape = BrickShape::RECTANGLE;
    }
    else
    {
        std::cerr << "Invalid magic sequence in level file: " << filename << std::endl;
        return;
    }

    file >> gridDimensions.first >> gridDimensions.second;

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        int posX, posY, resistance;
        std::string colorHex;
        std::string power_up;

        if (!(iss >> posX >> posY >> resistance >> colorHex))
        {
            std::cerr << "Invalid line format in level file: " << filename << std::endl;
            continue;
        }
        iss >> power_up;

        Uint32 colorValue;
        std::stringstream(colorHex) >> std::hex >> colorValue >> std::dec;
        SDL_Color color = { (uint8_t)((colorValue >> 24) & 0xFF), (uint8_t)((colorValue >> 16) & 0xFF), (uint8_t)((colorValue >> 8) & 0xFF), (uint8_t)(colorValue & 0xFF)};

        uint32_t mappedColor = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);

        bricks.push_back(Brick(posX, posY, mappedColor, resistance, power_up));
    }

    file.close();
}

void BrickBreaker::addBall(Ball& ball)
{
    balls.push_back(ball);
}

const Platform& BrickBreaker::getPlatform() const
{
    return platform;
}

void BrickBreaker::handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2)
{
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        handleResize({*(int*)data1.get(), *(int*)data2.get()}, {event.window.data1, event.window.data2});
    }
    else if(event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_LEFT:
                platform.setSpeedX(surface->w / -700);
                break;
            case SDLK_RIGHT:
                platform.setSpeedX(surface->w / 700);
                break;
            default:
                break;
        }
    }
    else if(event.type == SDL_KEYUP)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_LEFT:
            case SDLK_RIGHT:
                platform.setSpeedX(0);
                break;
            case SDLK_ESCAPE:
                is_running = false;
                break;
            default:
                break;
        }
    }
    
}

void BrickBreaker::update(uint64_t delta_time)
{
    if(start_duration > 0)
    {
        start_duration -= delta_time;
        return;
    }
    
    platform.update(delta_time, surface->w);

    for (auto& powerUp : powerUps)
    {
        if (!powerUp->isActive())
        {
            powerUp->update(delta_time);
            if(powerUp->resolveCollisionWithRectangle(platform.getRect()))
            {
                powerUp->applyPowerUp(*this);
            }
        }
        else
        {
            powerUp->decrementDuration(delta_time);
        }
    }

    // remove power-up if it is out of bounds or its duration is over
    powerUps.erase(std::remove_if(powerUps.begin(), powerUps.end(), [this](const std::unique_ptr<PowerUp>& powerUp) {
        if (powerUp->getCenter().second + powerUp->getRadius() > surface->h && !powerUp->isActive())
        {
            return true;
        }
        if(powerUp->isActive() && powerUp->getDuration() <= 0)
        {
            powerUp->unApplyPowerUp(*this);
            return true;
        }
        return false;
    }), powerUps.end());

    
    for (auto& ball : balls)
    {
        for(auto& brick : bricks)
        {
            int brickWidth = surface->w / gridDimensions.first;
            int brickHeight = surface->h * BrickBreaker::BRICK_HEIGHT_LIMIT / gridDimensions.second;
            const std::pair<uint32_t, uint32_t> position = brick.getPosition();
            SDL_Rect rect = {(int)position.first * brickWidth, (int)position.second * brickHeight, brickWidth, brickHeight};
       
            if(ball.resolveCollisionWithRectangle(rect))
            {
                brick.decreaseResistance();
                if(brick.getResistance() == 0)
                {
                    std::unique_ptr<PowerUp> powerUp = brick.getPowerUp();
                    if(powerUp)
                    {
                        powerUps.push_back(std::move(powerUp));
                        powerUps.back()->setCenter({static_cast<_Float32>(rect.x) + static_cast<_Float32>(rect.w) / 2.0f, static_cast<_Float32>(rect.y) + static_cast<_Float32>(rect.h) / 2.0f});
                        powerUps.back()->setSpeed({0, static_cast<_Float32>(surface->h) / 4000.0f});
                        powerUps.back()->setRadius(static_cast<_Float32>(surface->w) / 80.0f);
                    }
                    bricks.erase(std::remove_if(bricks.begin(), bricks.end(), [&brick](const Brick& b) {return &b == &brick;}), bricks.end());
                }
                break;
            }
        }


        ball.resolveCollisionWithRectangle(platform.getRect());

        // collisions with borders
        SDL_Rect borders[4] = {
            {0, 0, surface->w, 30},
            {0, 0, 30, surface->h},
            {0, surface->h - 1, surface->w, 30},
            {surface->w - 1, 0, 30, surface->h}
        };
        
        for (const auto& border : borders)
        {
            ball.resolveCollisionWithRectangle(border);
        }

        ball.update(delta_time);

    }
}

SDL_Surface* BrickBreaker::render()
{
    SDL_FillRect(surface.get(), nullptr, SDL_MapRGB(surface->format, 0, 0, 0));

    if (start_duration > 0)
    {
        SDL_FillRect(surface.get(), nullptr, SDL_MapRGB(surface->format, 0, 0, 0));
        if (font)
        {
            SDL_Color color = {255, 255, 255, 0};
            std::stringstream ss;
            ss << "Starting in " << std::fixed << std::setprecision(1) << start_duration / 1000.0f << " seconds";
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, ss.str().c_str(), color);
            SDL_Rect destRect = {surface->w / 2 - textSurface->w / 2, surface->h / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
            SDL_BlitSurface(textSurface, nullptr, surface.get(), &destRect);
            SDL_FreeSurface(textSurface);
        }
    }

    if (bricks.empty() || balls.empty() || gridDimensions.first == 0 || gridDimensions.second == 0)
    {
        if (font)
        {
            SDL_Color color = {255, 255, 255, 0};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, "You won!", color);
            SDL_Rect destRect = {surface->w / 2 - textSurface->w / 2, surface->h / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
            SDL_BlitSurface(textSurface, nullptr, surface.get(), &destRect);
            SDL_FreeSurface(textSurface);
        }
        is_running = false;
        return surface.get();
    }

    int brickWidth = surface->w / gridDimensions.first;
    int brickHeight = surface->h * BrickBreaker::BRICK_HEIGHT_LIMIT / gridDimensions.second;
    for (const auto& brick : bricks)
    {
       const std::pair<uint32_t, uint32_t> position = brick.getPosition();
       SDL_Rect rect = {(int)position.first * brickWidth, (int)position.second * brickHeight, brickWidth, brickHeight};
       SDL_FillRect(surface.get(), &rect, brick.getColor());
    }

    for (const auto& ball : balls)
    {
        const std::pair<uint32_t, uint32_t> position = ball.getCenter();
        float radius = ball.getRadius();
        // SDL_Color color = ball->getColor();

        SDL_Rect destRect = {
            static_cast<int>(position.first - radius),
            static_cast<int>(position.second - radius),
            static_cast<int>(2 * radius),
            static_cast<int>(2 * radius)
        };

        SDL_BlitScaled(ball.getSurface().get(), nullptr, surface.get(), &destRect);

    }

    for (const auto& powerUp : powerUps)
    {
        if(powerUp->isActive())
        {
            continue;
        }
        std::pair<_Float32, _Float32> position = powerUp->getCenter();
        SDL_Rect rect = {static_cast<int>(position.first), static_cast<int>(position.second), 20, 20};

        SDL_FillRect(surface.get(), &rect, SDL_MapRGBA(surface->format, 255, 255, 0, 0));
    }


    SDL_Color color = platform.getColor();
    SDL_FillRect(surface.get(), &(platform.getRect()), SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));

    return surface.get();
}

const _Float32 BrickBreaker::getBallRadius() const
{
    return static_cast<_Float32>(surface->w) / 60.0f;
}

const _Float32 BrickBreaker::getInitialBallSpeed() const
{
    return static_cast<_Float32>(surface->w) / 1600.0f;
}

Platform& BrickBreaker::getPlatform()
{
    return platform;
}

std::vector<Ball>& BrickBreaker::getBalls()
{
    return balls;
}