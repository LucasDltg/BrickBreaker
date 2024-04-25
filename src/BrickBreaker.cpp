#include "../include/BrickBreaker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "../include/Brick.h"
#include "../include/Ball.h"
#include "../include/Platform.h"

BrickBreaker::BrickBreaker(std::shared_ptr<SDL_Renderer> renderer, const std::string& filename)
: SDLComponent(), _platform(), _start_duration(1000), _font(nullptr, TTF_CloseFont)
{
    createBricksFromLevel(filename);
    
    _font = std::unique_ptr<TTF_Font, void(*)(TTF_Font*)>(TTF_OpenFont("./assets/fonts/arial/arial.ttf", 24), TTF_CloseFont);
    if (!_font.get())
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
}

void BrickBreaker::initSurface()
{
    SDL_Color color;
    color.r = 0; color.g = 0; color.b = 255; color.a = 0;
    std::pair<uint32_t, uint32_t> center = {_surface->w / 2, _surface->h * 3/ 4};
    std::pair<_Float32, _Float32> speed = {getInitialBallSpeed() / 3, -getInitialBallSpeed()};
    _balls.push_back(Ball(getBallRadius(), center, color, speed));

    color.r = 255; color.g = 0; color.b = 0; color.a = 0;
    _platform.setRect({static_cast<_Float32>(_surface->w) / 2.0f - static_cast<_Float32>(_surface->w) / 14.0f, static_cast<_Float32>(_surface->h) * 0.9f, static_cast<_Float32>(_surface->w) / 7.0f, static_cast<_Float32>(_surface->h) / 30.0f});
    _platform.setColor(color);

    for (auto& brick : _bricks)
    {
        brick->calculateVerticesWithPosition(_gridDimensions, {static_cast<_Float32>(_surface->w), static_cast<_Float32>(_surface->h * BrickBreaker::_BRICK_HEIGHT_LIMIT)});
    }

    _textureManager.loadTexture("assets/textures/small_crack.png", "small", _renderer, SDL_BLENDMODE_MUL);
    _textureManager.loadTexture("assets/textures/medium_crack.png", "medium", _renderer, SDL_BLENDMODE_MUL);
    _textureManager.loadTexture("assets/textures/big_crack.png", "big", _renderer, SDL_BLENDMODE_MUL);
    _textureManager.loadTexture("assets/textures/ball.png", typeid(Ball).name(), _renderer);
    _textureManager.loadTexture("assets/textures/platform.png", typeid(Platform).name(), _renderer);
    _textureManager.loadTexture("assets/textures/bubble_duplicate.png", typeid(DuplicateBallPowerUp).name(), _renderer);
    _textureManager.loadTexture("assets/textures/bubble_multi.png", typeid(AddBallPowerUp).name(), _renderer);
    _textureManager.loadTexture("assets/textures/bubble_extend.png", typeid(ExtendPlatformPowerUp).name(), _renderer);
    _textureManager.loadTexture("assets/textures/bubble_speed.png", typeid(SpeedUpPowerUp).name(), _renderer);
}

void BrickBreaker::handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize)
{
    for (auto& brick : _bricks)
    {
        brick->calculateVerticesWithPosition(_gridDimensions, {static_cast<_Float32>(newSize.first), static_cast<_Float32>(newSize.second * BrickBreaker::_BRICK_HEIGHT_LIMIT)});
    }
    
    for (auto& ball : _balls)
    {
        std::pair<_Float32, _Float32> center = ball.getCenter();

        uint32_t newX = static_cast<uint32_t>(static_cast<float>(center.first) * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
        uint32_t newY = static_cast<uint32_t>(static_cast<float>(center.second) * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second)));

        ball.setCenter({newX, newY});
        ball.setSpeed({ball.getSpeed().first * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)), ball.getSpeed().second * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second))});
        ball.setRadius(ball.getRadius() * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
    }

    for (auto& powerUp : _powerUps)
    {
        std::pair<_Float32, _Float32> center = powerUp->getCenter();

        uint32_t newX = static_cast<uint32_t>(static_cast<float>(center.first) * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
        uint32_t newY = static_cast<uint32_t>(static_cast<float>(center.second) * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second)));

        powerUp->setCenter({newX, newY});
        powerUp->setSpeed({powerUp->getSpeed().first * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)), powerUp->getSpeed().second * (static_cast<float>(newSize.second) / static_cast<float>(previousSize.second))});
        powerUp->setRadius(powerUp->getRadius() * (static_cast<float>(newSize.first) / static_cast<float>(previousSize.first)));
    }

    // Resize platform
    SDL_FRect rect = _platform.getRect();
    rect.x = static_cast<_Float32>(rect.x) * (static_cast<_Float32>(newSize.first) / static_cast<float>(previousSize.first));
    rect.y = static_cast<_Float32>(rect.y) * (static_cast<_Float32>(newSize.second) / static_cast<float>(previousSize.second));
    rect.w = static_cast<_Float32>(rect.w) * (static_cast<_Float32>(newSize.first) / static_cast<float>(previousSize.first));
    rect.h = static_cast<_Float32>(rect.h) * (static_cast<_Float32>(newSize.second) / static_cast<float>(previousSize.second));
    _platform.setRect(rect);
}

void BrickBreaker::createBricksFromLevel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << filename << std::endl;
        _is_running = false;
        return;
    }

    std::string magicSequence;
    file >> magicSequence;
    if (magicSequence == "rectangle")
        _brickShape = BrickShape::RECTANGLE;
    else if (magicSequence == "triangle")
        _brickShape = BrickShape::TRIANGLE;
    else if (magicSequence == "hexagon")
        _brickShape = BrickShape::HEXAGON;
    else
    {
        std::cerr << "Invalid magic sequence in level file: " << filename << std::endl;
        _is_running = false;
        return;
    }

    file >> _gridDimensions.first >> _gridDimensions.second;
    if (_gridDimensions.first <= 0 || _gridDimensions.second <= 0)
    {
        std::cerr << "Invalid grid dimensions in level file: " << filename << std::endl;
        _is_running = false;
        return;
    }

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        int32_t posX, posY, resistance;
        std::string colorHex;
        std::string power_up;

        if (!(iss >> posX >> posY >> resistance >> colorHex))
        {
            std::cerr << "Invalid line format in level file: " << filename << std::endl;
            continue;
        }
        iss >> power_up;
        
        if(posX < 0 || posX >= _gridDimensions.first || posY < 0 || posY >= _gridDimensions.second)
        {
            std::cerr << "Invalid brick position in level file: " << filename << std::endl;
            continue;
        }

        uint32_t mappedColor;
        std::stringstream(colorHex) >> std::hex >> mappedColor >> std::dec;

        if (_brickShape == BrickShape::RECTANGLE)
            _bricks.push_back(std::make_unique<BrickRectangular>(std::make_pair(posX, posY), _gridDimensions, std::make_pair(_surface->w, _surface->h * BrickBreaker::_BRICK_HEIGHT_LIMIT), mappedColor, resistance, power_up, _renderer));
        else if (_brickShape == BrickShape::TRIANGLE)
            _bricks.push_back(std::make_unique<BrickTriangular>(std::make_pair(posX, posY), _gridDimensions, std::make_pair(_surface->w, _surface->h * BrickBreaker::_BRICK_HEIGHT_LIMIT), mappedColor, resistance, power_up, _renderer));
        else if (_brickShape == BrickShape::HEXAGON)
            _bricks.push_back(std::make_unique<BrickHexagonal>(std::make_pair(posX, posY), _gridDimensions, std::make_pair(_surface->w, _surface->h * BrickBreaker::_BRICK_HEIGHT_LIMIT), mappedColor, resistance, power_up, _renderer));
    }

    file.close();
}

void BrickBreaker::addBall(Ball& ball)
{
    _balls.push_back(ball);
}

const Platform& BrickBreaker::getPlatform() const
{
    return _platform;
}

void BrickBreaker::handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2)
{
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        handleResize({*(int*)data1.get(), *(int*)data2.get()}, {event.window.data1, event.window.data2});
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        _platform.setRect({event.motion.x - _platform.getRect().w / 2, _platform.getRect().y, _platform.getRect().w, _platform.getRect().h});
    }

    const uint8_t* state = SDL_GetKeyboardState(nullptr);
    if(state[SDL_SCANCODE_ESCAPE])
    {
        _is_running = false;
    }
    else if (state[SDL_SCANCODE_LEFT])
    {
        _platform.setSpeedX(-getInitialPlatformSpeed());
    }
    else if (state[SDL_SCANCODE_RIGHT])
    {
        _platform.setSpeedX(getInitialPlatformSpeed());
    }
    else
    {
        _platform.setSpeedX(0);
    }
}

void BrickBreaker::update(uint64_t delta_time)
{
    if(_start_duration > 0)
    {
        _start_duration -= delta_time;
        return;
    }
    
    _platform.update(delta_time, _surface->w);

    for (auto& powerUp : _powerUps)
    {
        if (!powerUp->isActive())
        {
            powerUp->update(delta_time);
            if(powerUp->resolveCollisionWithRectangle(_platform.getRect()))
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
    _powerUps.erase(std::remove_if(_powerUps.begin(), _powerUps.end(), [this](const std::unique_ptr<PowerUp>& powerUp) {
        if (powerUp->getCenter().second + powerUp->getRadius() > _surface->h && !powerUp->isActive())
        {
            return true;
        }
        if(powerUp->isActive() && powerUp->getDuration() <= 0)
        {
            powerUp->unApplyPowerUp(*this);
            return true;
        }
        return false;
    }), _powerUps.end());

    
    for (auto& ball : _balls)
    {
        for(auto& brick : _bricks)
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
                        _powerUps.push_back(std::move(powerUp));
                        _powerUps.back()->setRadius(getBallRadius());
                        _powerUps.back()->setCenter(brick->getCenter());
                        _powerUps.back()->setSpeed({0, static_cast<_Float32>(_surface->h) / 4000.0f});
                    }
                    // we remove the brick from the vector here, because we don't want to check for collision with it anymore
                    _bricks.erase(std::remove_if(_bricks.begin(), _bricks.end(), [&brick](const std::unique_ptr<Brick>& b) {
                        return b.get() == brick.get();
                    }), _bricks.end());
                }
                break;
            }
        }

        // Check for collision with platform
        ball.resolveCollisionWithRectangle(_platform.getRect());

        // Check for collision with borders
        std::vector<std::pair<_Float32, _Float32>> borders = {
            {0, 0},
            {static_cast<_Float32>(_surface->w), 0},
            {static_cast<_Float32>(_surface->w), static_cast<_Float32>(_surface->h)},
            {0, static_cast<_Float32>(_surface->h)},
        };
        
        ball.resolveCollisionWithLine(borders[0], borders[1]);
        ball.resolveCollisionWithLine(borders[1], borders[2]);
        ball.resolveCollisionWithLine(borders[2], borders[3]);
        ball.resolveCollisionWithLine(borders[3], borders[0]);
        
        ball.update(delta_time);
    }

    // remove balls that are out of bounds
    /*balls.erase(std::remove_if(balls.begin(), balls.end(), [this](const Ball& ball) {
        return ball.getCenter().second - ball.getRadius() > surface->h;
    }), balls.end());*/

    if (_bricks.empty() || _balls.empty() || std::all_of(_bricks.begin(), _bricks.end(), [](const std::unique_ptr<Brick>& brick) {
            return brick->getResistance() <= 0;}))
        _start_duration = 2000;
}

std::shared_ptr<SDL_Surface> BrickBreaker::render()
{
    // reset surface
    SDL_FillRect(_surface.get(), nullptr, SDL_MapRGB(_surface->format, 0, 0, 0));
    
    // check if all balls are lost or all bricks are destroyed or have resisitance < 0
    if (_balls.empty() || _bricks.empty() || std::all_of(_bricks.begin(), _bricks.end(), [](const std::unique_ptr<Brick>& brick) {
        return brick->getResistance() <= 0;
    }))
    {
        std::string text = _balls.empty() ? "You lost!" : "You won!";
        if (_start_duration <= 0)
            _is_running = false;

        SDL_Color color = {255, 255, 255, 0};
        SDL_Surface* textSurface = TTF_RenderText_Solid(_font.get(), text.c_str(), color);
        SDL_Rect destRect = {_surface->w / 2 - textSurface->w / 2, _surface->h / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
        SDL_BlitSurface(textSurface, nullptr, _surface.get(), &destRect);
        SDL_FreeSurface(textSurface);
        
        return _surface;
    }

    if (_start_duration > 0)
    {
        SDL_FillRect(_surface.get(), nullptr, SDL_MapRGB(_surface->format, 0, 0, 0));
        SDL_Color color = {255, 255, 255, 0};
        std::stringstream ss;
        ss << "Starting in " << std::fixed << std::setprecision(1) << _start_duration / 1000.0f << " seconds";
        SDL_Surface* textSurface = TTF_RenderText_Solid(_font.get(), ss.str().c_str(), color);
        SDL_Rect destRect = {_surface->w / 2 - textSurface->w / 2, _surface->h / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
        SDL_BlitSurface(textSurface, nullptr, _surface.get(), &destRect);
        SDL_FreeSurface(textSurface);
    }

    SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);

    for (const auto& brick : _bricks)
    {
        std::vector<SDL_Vertex> vertices = brick->getVertices();
        std::vector<int32_t> indices = brick->getIndices();

        if (SDL_RenderGeometry(_renderer.get(), nullptr, vertices.data(), vertices.size(), indices.data(), indices.size()) != 0) {
            std::cerr << "Failed to render geometry: " << SDL_GetError() << std::endl;
            return nullptr;
        }
        
        // draw resistance texture on brick
        _Float32 resistancePercentage = brick->getResistancePercentage();
        // skip brick with infinite resistance or full resistance
        if (brick->getResistance() > 0 && resistancePercentage >= 0.25f)
        {
            std::shared_ptr<SDL_Texture> texture;
            if (resistancePercentage >= 0.75f)
                texture = _textureManager.getTexture("big");
            else if (resistancePercentage >= 0.50f)
                texture = _textureManager.getTexture("medium");
            else
                texture = _textureManager.getTexture("small");
        
            vertices = brick->getVerticesWithoutColor();
            if(SDL_RenderGeometry(_renderer.get(), texture.get(), vertices.data(), vertices.size(), indices.data(), indices.size()) != 0)
            {    
                std::cerr << "Failed to render geometry: " << SDL_GetError() << std::endl;
                return nullptr;
            }
        }

        for (size_t i = 0; i < vertices.size(); i++)
        {
            if (SDL_RenderDrawLine(_renderer.get(), static_cast<int32_t>(vertices[i].position.x), static_cast<int32_t>(vertices[i].position.y), static_cast<int32_t>(vertices[(i + 1) % vertices.size()].position.x), static_cast<int32_t>(vertices[(i + 1) % vertices.size()].position.y)) != 0)
            {
                std::cerr << "Failed to render line: " << SDL_GetError() << std::endl;
                return nullptr;
            }
        }
    }

    for (const auto& ball : _balls)
    {
        const std::pair<uint32_t, uint32_t> position = ball.getCenter();
        float radius = ball.getRadius();

        SDL_Rect destRect = {
            static_cast<int>(position.first - radius),
            static_cast<int>(position.second - radius),
            static_cast<int>(2 * radius),
            static_cast<int>(2 * radius)
        };

        SDL_RenderCopy(_renderer.get(), _textureManager.getTexture(typeid(ball).name()).get(), nullptr, &destRect);
    }

    for (const auto& powerUp : _powerUps)
    {
        if(powerUp->isActive())
        {
            continue;
        }
        std::pair<_Float32, _Float32> position = powerUp->getCenter();
        _Float32 radius = powerUp->getRadius();
        SDL_Rect rect = {static_cast<int>(position.first - radius), static_cast<int>(position.second - radius), static_cast<int>(2 * radius), static_cast<int>(2 * radius)};

        std::shared_ptr<SDL_Texture> powerUpSurface = _textureManager.getTexture(typeid(*powerUp).name());
        if (powerUpSurface.get() != nullptr)
        {
            SDL_RenderCopy(_renderer.get(), powerUpSurface.get(), nullptr, &rect);
        }
        else        
            SDL_FillRect(_surface.get(), &rect, SDL_MapRGBA(_surface->format, 255, 255, 0, 0));
    }

    SDL_Color color = _platform.getColor();
    const SDL_FRect& rect = _platform.getRect();
    SDL_Rect destRect = {static_cast<int>(rect.x), static_cast<int>(rect.y), static_cast<int>(rect.w), static_cast<int>(rect.h)};
    std::shared_ptr<SDL_Texture> platformSurface = _textureManager.getTexture(typeid(_platform).name());

    if(platformSurface.get() != nullptr)
        SDL_RenderCopy(_renderer.get(), platformSurface.get(), nullptr, &destRect);
    else
        SDL_FillRect(_surface.get(), &destRect, SDL_MapRGBA(_surface->format, color.r, color.g, color.b, color.a));

    return _surface;
}

const _Float32 BrickBreaker::getBallRadius() const
{
    return static_cast<_Float32>(_surface->w) / 80.0f;
}

const _Float32 BrickBreaker::getInitialBallSpeed() const
{
    return static_cast<_Float32>(_surface->w) / 1600.0f;
}

Platform& BrickBreaker::getPlatform()
{
    return _platform;
}

std::vector<Ball>& BrickBreaker::getBalls()
{
    return _balls;
}

BrickBreaker::~BrickBreaker()
{
    
}

const _Float32 BrickBreaker::getInitialPlatformSpeed() const
{
    return static_cast<_Float32>(_surface->w) / 700.0f;
}