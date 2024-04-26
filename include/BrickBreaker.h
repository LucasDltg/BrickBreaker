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
    static constexpr _Float32 _BRICK_HEIGHT_LIMIT = 0.4f;

    BrickBreaker(std::shared_ptr<SDL_Renderer> renderer, const std::string& filename);

    void handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2) override;
    void update(uint64_t delta_time) override;
    std::shared_ptr<SDL_Surface> render() override;
    void initSurface() override;
    
    void addBall(Ball& ball);
    
    const Platform& getPlatform() const;
    Platform& getPlatform();
    const _Float32 getBallRadius() const;
    const _Float32 getInitialBallSpeed() const;
    const _Float32 getInitialPlatformSpeed() const;
    std::vector<Ball>& getBalls();

private:
    std::vector<std::unique_ptr<Brick>> _bricks;
    std::vector<Ball> _balls;
    std::vector<std::unique_ptr<PowerUp>> _power_ups;
    Platform _platform;
    std::pair<int32_t, int32_t> _grid_dimensions;
    BrickShape _brick_shape;
    int32_t _start_duration;
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _font;
    
    void createBricksFromLevel(const std::string& filename);
    void handleResize(std::pair<int32_t, int32_t> previous_size, std::pair<int32_t, int32_t> new_size);
};

#endif
