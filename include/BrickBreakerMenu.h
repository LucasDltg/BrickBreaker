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
    BrickBreakerMenuLevel(std::string path) : _path(path) { std::filesystem::path p(path); _name = p.stem().string(); }

    private:
    std::string _name;
    std::string _path;
};

class BrickBreakerMenu : public SDLComponent
{
public:
    BrickBreakerMenu(std::shared_ptr<SDL_Renderer> renderer, std::string directory_path = "./assets/levels");
    void handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2) override;
    void update(uint64_t delta_time) override;
    std::shared_ptr<SDL_Surface> render() override;
    void initSurface() override;

private:
    uint32_t getPadding() const;
    uint32_t getFontSize() const;
    void handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize);
    void reloadBackground();
    size_t _selectedLevel;
    uint32_t _num_rows;
    uint32_t _num_columns;
    std::vector<BrickBreakerMenuLevel> _levels;
    std::unique_ptr<BrickBreaker> _brickBreaker;
    std::unique_ptr<BrickBreaker> _background;
    uint32_t _current_page;
    uint32_t _num_pages;
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _font;
};

#endif // BRICKBREAKERMENU_H