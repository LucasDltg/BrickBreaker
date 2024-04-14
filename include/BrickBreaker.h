#ifndef BRICKBREAKER_H
#define BRICKBREAKER_H

#include "../include/SDLComponent.h"
#include "../include/Brick.h"
#include "../include/Ball.h"
#include "../include/Platform.h"
#include <memory>
#include <vector>
#include <fstream>
#include <SDL2/SDL_ttf.h>

enum class BrickShape {RECTANGLE, HEXAGON};

class BrickBreaker : public SDLComponent {
public:
    static constexpr float BRICK_HEIGHT_LIMIT = 0.3f;

    BrickBreaker(const std::string& filename); // Constructor to read level from file
    virtual void handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2) override;
    virtual void update(uint64_t delta_time) override;
    virtual SDL_Surface* render() override;
    virtual void initSurface(uint32_t width, uint32_t height) override;
    virtual void addBall(std::unique_ptr<Ball> ball);
    virtual const Platform& getPlatform() const;

    _Float32 getBallRadius() const;
    _Float32 getInitialBallSpeed() const;
    void incrementLifeCount();
    void decrementLifeCount();

private:
    std::vector<std::unique_ptr<Brick>> bricks;
    std::vector<std::unique_ptr<Ball>> balls;
    std::vector<std::unique_ptr<PowerUp>> powerUps;
    std::unique_ptr<Platform> platform;
    std::pair<uint32_t, uint32_t> gridDimensions;
    BrickShape brickShape;
    uint32_t lifeCount;
    TTF_Font* font;
    

    // Helper function to create bricks from level data
    void createBricksFromLevel(const std::string& filename);
    void handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize);
};

#endif
