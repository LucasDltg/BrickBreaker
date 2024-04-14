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
: SDLComponent(), start_duration(2000)
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
    std::pair<uint32_t, uint32_t> center = {surface->w / 2, surface->h * 3/ 4};
    std::pair<_Float32, _Float32> speed = {getInitialBallSpeed() / 3, -getInitialBallSpeed()};
    balls.push_back(Ball(getBallRadius(), center, color, speed));

    color.r = 255; color.g = 0; color.b = 0; color.a = 0;
    platform.setRect({surface->w / 2 - surface->w / 14, surface->h * 9 / 10, surface->w / 7, surface->h / 30});
    platform.setColor(color);

    for (auto& brick : bricks)
    {
        brick->calculateVerticesWithPosition(gridDimensions, {static_cast<_Float32>(width), static_cast<_Float32>(height * BrickBreaker::BRICK_HEIGHT_LIMIT)});
    }
}

void BrickBreaker::handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize)
{
    setSurfaceDimensions(newSize.first, newSize.second);

    for (auto& brick : bricks)
    {
        brick->calculateVerticesWithPosition(gridDimensions, {static_cast<_Float32>(newSize.first), static_cast<_Float32>(newSize.second * BrickBreaker::BRICK_HEIGHT_LIMIT)});
    }
    
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
        is_running = false;
        return;
    }

    std::string magicSequence;
    file >> magicSequence;
    if (magicSequence == "rectangle")
    {
        brickShape = BrickShape::RECTANGLE;
    }
    else if (magicSequence == "triangle")
    {
        brickShape = BrickShape::TRIANGLE;
    }
    else
    {
        std::cerr << "Invalid magic sequence in level file: " << filename << std::endl;
        is_running = false;
        return;
    }

    file >> gridDimensions.first >> gridDimensions.second;
    if (gridDimensions.first == 0 || gridDimensions.second == 0)
    {
        std::cerr << "Invalid grid dimensions in level file: " << filename << std::endl;
        is_running = false;
        return;
    }

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

        if (brickShape == BrickShape::RECTANGLE)
            bricks.push_back(std::make_unique<BrickRectangular>(std::make_pair(posX, posY), gridDimensions, std::make_pair(surface->w, surface->h * BrickBreaker::BRICK_HEIGHT_LIMIT), mappedColor, resistance, power_up));
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
            std::vector<SDL_Vertex> v = brick->getVertices();
            bool collision = false;
            
            for (size_t i = 0; i < v.size(); i++)
            {
                if (ball.resolveCollisionWithLine({v[i].position.x, v[i].position.y}, {v[(i + 1) % v.size()].position.x, v[(i + 1) % v.size()].position.y}))
                {
                    collision = true;
                    break;
                }
            }

            if(collision)
            {
                brick->decreaseResistance();
                if(brick->getResistance() == 0)
                {
                    std::unique_ptr<PowerUp> powerUp = brick->getPowerUp();
                    if(powerUp)
                    {
                        powerUps.push_back(std::move(powerUp));
                        powerUps.back()->setRadius(getBallRadius() / 2);
                        powerUps.back()->setCenter({brick->getCenter().first - powerUps.back()->getRadius() / 2, brick->getCenter().second - powerUps.back()->getRadius() / 2});
                        powerUps.back()->setSpeed({0, static_cast<_Float32>(surface->h) / 4000.0f});
                    }
                    bricks.erase(std::remove_if(bricks.begin(), bricks.end(), [&brick](const std::unique_ptr<Brick>& b) {
                        return b.get() == brick.get();
                    }), bricks.end());
                    
                    if (bricks.empty())
                        start_duration = 2000;
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
    if (bricks.empty() || balls.empty())
    {
        std::string text = bricks.empty() ? "You won!" : "You lost!";
        if (start_duration < 0)
            is_running = false;

        if (font)
        {
            SDL_Color color = {255, 255, 255, 0};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
            SDL_Rect destRect = {surface->w / 2 - textSurface->w / 2, surface->h / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
            SDL_BlitSurface(textSurface, nullptr, surface.get(), &destRect);
            SDL_FreeSurface(textSurface);
        }
        return surface.get();
    }

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

    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface.get());
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    for (const auto& brick : bricks)
    {
        std::vector<SDL_Vertex> vertices = brick->getVertices();
        std::vector<int32_t> indices = brick->getIndices();
        if (SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), indices.data(), 6) != 0) {
            std::cerr << "Failed to render geometry: " << SDL_GetError() << std::endl;
            SDL_DestroyRenderer(renderer);
            return nullptr;
        }
    }
    SDL_RenderPresent(renderer);
    SDL_DestroyRenderer(renderer);

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
        _Float32 radius = powerUp->getRadius();
        SDL_Rect rect = {static_cast<int>(position.first - radius), static_cast<int>(position.second - radius), radius*2, radius*2};

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