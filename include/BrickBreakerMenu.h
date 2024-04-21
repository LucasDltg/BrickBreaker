#ifndef BRICKBREAKERMENU_H
#define BRICKBREAKERMENU_H

#include "../include/SDLComponent.h"
#include "../include/BrickBreaker.h"
#include <vector>
#include <string>
#include <filesystem>
#include <SDL2/SDL_ttf.h>

class BrickBreakerMenuLevel
{
    friend class BrickBreakerMenu;
    public:
    BrickBreakerMenuLevel(std::string path) : path(path) { std::filesystem::path p(path); name = p.stem().string(); }

    private:
    SDL_Rect rect;
    std::string name;
    std::string path;
    std::shared_ptr<SDL_Surface> surface;
};

class BrickBreakerMenu : public SDLComponent
{
public:
    BrickBreakerMenu(std::string directory_path = "./assets/levels");
    void handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2) override;
    void update(uint64_t delta_time) override;
    std::shared_ptr<SDL_Surface> render() override;
    void initSurface(uint32_t width, uint32_t height) override;

private:
    uint32_t getPadding() const;
    uint32_t getFontSize() const;
    void handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize);
    void reloadBackground();
    size_t selectedLevel;
    uint32_t num_rows;
    uint32_t num_columns;
    std::vector<BrickBreakerMenuLevel> levels;
    std::unique_ptr<BrickBreaker> brickBreaker;
    std::unique_ptr<BrickBreaker> background;
    uint32_t current_page;
    uint32_t num_pages;

    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> font;
};

#endif // BRICKBREAKERMENU_H