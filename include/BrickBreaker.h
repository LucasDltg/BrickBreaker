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

enum class BrickShape {RECTANGLE, HEXAGON, TRIANGLE};

class BrickBreaker : public SDLComponent {
public:
    static constexpr float BRICK_HEIGHT_LIMIT = 0.4f;

    BrickBreaker(const std::string& filename);
    ~BrickBreaker();

    void handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2) override;
    void update(uint64_t delta_time) override;
    std::shared_ptr<SDL_Surface> render() override;
    void initSurface(uint32_t width, uint32_t height) override;
    
    void addBall(Ball& ball);
    
    const Platform& getPlatform() const;
    Platform& getPlatform();
    const _Float32 getBallRadius() const;
    const _Float32 getInitialBallSpeed() const;
    const _Float32 getInitialPlatformSpeed() const;
    std::vector<Ball>& getBalls();

private:
    std::vector<std::unique_ptr<Brick>> bricks;
    std::vector<Ball> balls;
    std::vector<std::unique_ptr<PowerUp>> powerUps;
    Platform platform;
    std::pair<int32_t, int32_t> gridDimensions;
    BrickShape brickShape;
    int32_t start_duration;
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> font;
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> renderer;
    
    void createBricksFromLevel(const std::string& filename);
    void handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize);
};

#endif
