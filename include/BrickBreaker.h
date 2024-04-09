#ifndef BRICKBREAKER_H
#define BRICKBREAKER_H

#include "../include/SDLComponent.h"
#include "../include/Brick.h"
#include <memory>
#include <vector>
#include <fstream>

class BrickBreaker : public SDLComponent {
public:
    BrickBreaker(); // Default constructor
    BrickBreaker(const std::string& filename); // Constructor to read level from file
    virtual void handleEvents(SDL_Event& event) override;
    virtual SDL_Surface* render() override;

private:
    std::vector<std::unique_ptr<Brick>> bricks;
    std::pair<uint32_t, uint32_t> gridDimensions;

    // Helper function to create bricks from level data
    void createBricksFromLevel(const std::string& filename);
    void handleResize();

};

#endif
