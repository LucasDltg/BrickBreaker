#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include "../include/BrickBreakerMenu.h"

BrickBreakerMenu::BrickBreakerMenu(std::shared_ptr<SDL_Renderer> renderer, std::string directory_path)
: SDLComponent(), _selectedLevel(0), _num_rows(3), _num_columns(3), _brickBreaker(nullptr), _background(nullptr), _current_page(0), _font(nullptr, nullptr)
{
    _font = std::unique_ptr<TTF_Font, void(*)(TTF_Font*)>(TTF_OpenFont("./assets/fonts/arial/arial.ttf", getFontSize()), TTF_CloseFont);
    if (!_font.get())
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
        if (entry.is_regular_file()) {
            if(entry.path().extension() != ".lvl")
                continue;
            
            _levels.push_back({entry.path().string()});
        }
    }

    if(_levels.empty())
    {
        std::cerr << "No levels found in " << directory_path << std::endl;
        return;
    }
    _num_pages = _levels.size() / _num_rows / _num_columns + (_levels.size() % (_num_rows * _num_columns) != 0);
    srand(time(nullptr));
}

void BrickBreakerMenu::handleResize(std::pair<int, int> previousSize, std::pair<int, int> newSize)
{
    TTF_SetFontSize(_font.get(), getFontSize());
}

void BrickBreakerMenu::handleEvents(SDL_Event& event, std::shared_ptr<void> data1, std::shared_ptr<void> data2)
{
    // il faut gérer le cas où la fenêtre est redimensionnée et que le jeu est en cours
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        handleResize({*(int*)data1.get(), *(int*)data2.get()}, {event.window.data1, event.window.data2});
        
        _background->setSurfaceDimensions(event.window.data1, event.window.data2);
        _background->handleEvents(event, data1, data2);
    }

    if (_brickBreaker != nullptr)
    {
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
            _brickBreaker->setSurfaceDimensions(event.window.data1, event.window.data2);
        
        _brickBreaker->handleEvents(event, data1, data2);
        return;
    }
    
    
    if (event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_RETURN:
                _brickBreaker = std::make_unique<BrickBreaker>(_renderer, _levels[_selectedLevel + _current_page * _num_rows * _num_columns]._path);
                _brickBreaker->setSurfaceDimensions(_surface->w, _surface->h);
                _brickBreaker->initSurface();
                break;
            case SDLK_LEFT:
                if(static_cast<int>(_selectedLevel) % _num_columns == 0)
                {
                    _current_page = (_current_page + _num_pages - 1) % _num_pages;
                    _selectedLevel = _num_columns * _selectedLevel / _num_columns + _num_columns - 1;
                }
                else
                    _selectedLevel -= 1;
                break;
            case SDLK_RIGHT:
                if(_selectedLevel % _num_columns == _num_columns - 1)
                {
                    _current_page = (_current_page + 1) % _num_pages;
                    _selectedLevel = _num_columns * _selectedLevel / _num_columns - _num_columns + 1;
                }
                else if (_selectedLevel + _current_page * _num_rows * _num_columns < _levels.size() - 1)
                    _selectedLevel += 1;
                
                break;
            case SDLK_UP:
                _selectedLevel = (_selectedLevel + _num_columns * (_num_rows - 1)) % (_num_rows * _num_columns);
                while (_selectedLevel + _current_page * _num_rows * _num_columns >= _levels.size())
                    _selectedLevel -= _num_columns;
                break;
            case SDLK_DOWN:                
                _selectedLevel = (_selectedLevel + _num_columns) % (_num_rows * _num_columns);
                while (_selectedLevel + _current_page * _num_rows * _num_columns >= _levels.size())
                    _selectedLevel -= _num_columns;
            break;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int x = event.button.x;
        int y = event.button.y;

        // check if a level was clicked
        for (size_t i(_current_page * _num_rows * _num_columns); i < _levels.size() && i < _num_rows * _num_columns * (_current_page + 1); ++i)
        {
            const uint32_t padding = getPadding();
            const _Float32 totalWidth = _surface->w - 2 * padding;
            const _Float32 totalHeight = _surface->h - 2 * padding;
            const _Float32 rectWidth = (totalWidth - (_num_columns - 1) * padding / 2) / _num_columns;
            const _Float32 rectHeight = (totalHeight - (_num_rows - 1) * padding / 2) / _num_rows;

            int32_t row = (i % (_num_rows * _num_columns)) / _num_columns;
            int32_t col = (i % (_num_rows * _num_columns)) % _num_columns;

            if (x >= padding + col * (rectWidth + padding / 2) && x <= padding + col * (rectWidth + padding / 2) + rectWidth && y >= padding + row * (rectHeight + padding / 2) && y <= padding + row * (rectHeight + padding / 2) + rectHeight)
            {
                _brickBreaker = std::make_unique<BrickBreaker>(_renderer, _levels[i]._path);
                _brickBreaker->setSurfaceDimensions(_surface->w, _surface->h);
                _brickBreaker->initSurface();
                break;
            }
        }

        // check if a page was clicked
        if(y >= static_cast<int32_t>(_surface->h - getPadding() / 2))
        {
            int totalWidth = _num_pages * getPadding() / 4 + (_num_pages - 1) * getPadding() / 2;
            int startX = _surface->w / 2 - totalWidth / 2;
            for (size_t i(0); i < _num_pages; ++i)
            {
                SDL_Rect pageRect = {
                    static_cast<int32_t>(startX + i * getPadding() / 2 + i * getPadding() / 4),
                    static_cast<int32_t>(_surface->h - getPadding() / 2),                                    
                    static_cast<int32_t>(getPadding() / 4),
                    static_cast<int32_t>(getPadding() / 4)};
                if (x >= pageRect.x && x <= pageRect.x + pageRect.w)
                {
                    _current_page = i;
                    _selectedLevel = 0;
                    break;
                }
            }
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        int x = event.motion.x;
        int y = event.motion.y;

        for (size_t i(_current_page * _num_rows * _num_columns); i < _levels.size() && i < _num_rows * _num_columns * (_current_page + 1); ++i)
        {
            const uint32_t padding = getPadding();
            const _Float32 totalWidth = _surface->w - 2 * padding;
            const _Float32 totalHeight = _surface->h - 2 * padding;
            const _Float32 rectWidth = (totalWidth - (_num_columns - 1) * padding / 2) / _num_columns;
            const _Float32 rectHeight = (totalHeight - (_num_rows - 1) * padding / 2) / _num_rows;

            int32_t row = (i % (_num_rows * _num_columns)) / _num_columns;
            int32_t col = (i % (_num_rows * _num_columns)) % _num_columns;
            
            if (x >= padding + col * (rectWidth + padding / 2) && x <= padding + col * (rectWidth + padding / 2) + rectWidth && y >= padding + row * (rectHeight + padding / 2) && y <= padding + row * (rectHeight + padding / 2) + rectHeight)
            {
                _selectedLevel = i % (_num_rows * _num_columns);
                break;
            }
        }
    }
    else if (event.type == SDL_MOUSEWHEEL)
    {
        if (event.wheel.y > 0 || event.wheel.x > 0)
        {
            _current_page = (_current_page + _num_pages - 1) % _num_pages;
            _selectedLevel = (_num_rows * _num_columns * _current_page) + (_selectedLevel % (_num_rows * _num_columns));
        }
        else if (event.wheel.y < 0 || event.wheel.x < 0)
        {
            _current_page = (_current_page + 1) % _num_pages;
            _selectedLevel = (_num_rows * _num_columns * _current_page) + (_selectedLevel % (_num_rows * _num_columns));
        }
    }
}

void BrickBreakerMenu::update(uint64_t delta_time)
{
    if (_brickBreaker != nullptr)
    {
        _brickBreaker->update(delta_time);
        if (!_brickBreaker->isRunning())
        {
            _brickBreaker = nullptr;
            reloadBackground();
        }
    }
    else // update background with an automated platform
    {
        _background->update(delta_time);
        
        const SDL_FRect& platformRect = _background->getPlatform().getRect();
        const Ball& ball = _background->getBalls().front();
        _Float32 initialPlatformSpeed = _background->getInitialPlatformSpeed();
        
        if (ball.getCenter().first < platformRect.x)
            _background->getPlatform().setSpeedX(-initialPlatformSpeed);
        else if (ball.getCenter().first > (platformRect.x + platformRect.w))
            _background->getPlatform().setSpeedX(initialPlatformSpeed);


        if (!_background->isRunning())
        {
            reloadBackground();
        }
    }
}

std::shared_ptr<SDL_Surface> BrickBreakerMenu::render()
{
    if (_brickBreaker != nullptr)
    {
        return _brickBreaker->render();
    }

    SDL_FillRect(_surface.get(), nullptr, SDL_MapRGB(_surface->format, 0, 0, 0));
    if (_background != nullptr)
    {
        std::shared_ptr<SDL_Surface> backgroundSurface = _background->render();
        SDL_BlitSurface(backgroundSurface.get(), nullptr, _surface.get(), nullptr);
    }

    const uint32_t padding = getPadding();
    const _Float32 totalWidth = _surface->w - 2 * padding;
    const _Float32 totalHeight = _surface->h - 2 * padding;
    const _Float32 rectWidth = (totalWidth - (_num_columns - 1) * padding / 2) / _num_columns;
    const _Float32 rectHeight = (totalHeight - (_num_rows - 1) * padding / 2) / _num_rows;

    for (size_t i(_num_columns * _num_rows * _current_page); i < _levels.size() && i < _num_columns * _num_rows * (_current_page + 1); ++i)
    {
        int32_t row = (i % (_num_rows * _num_columns)) / _num_columns;
        int32_t col = (i % (_num_rows * _num_columns)) % _num_columns;

        _Float32 x = padding + col * (rectWidth + padding / 2);
        _Float32 y = padding + row * (rectHeight + padding / 2);

        SDL_Rect rect = {static_cast<int32_t>(x), static_cast<int32_t>(y), static_cast<int32_t>(rectWidth), static_cast<int32_t>(rectHeight)};
        if (_textureManager.getTexture("button").get() != nullptr)
        {
            if (i == _selectedLevel + _num_columns * _num_rows * _current_page)
                {rect.x -= padding / 16; rect.y -= padding / 16; rect.w += padding / 8; rect.h += padding / 8;}
            
            SDL_RenderCopy(_renderer.get(), _textureManager.getTexture("button").get(), nullptr, &rect);
        }
        else
        {
            if (i == _selectedLevel + _num_columns * _num_rows * _current_page)
                {rect.x -= padding / 16; rect.y -= padding / 16; rect.w += padding / 8; rect.h += padding / 8;}
            
            SDL_FillRect(_surface.get(), &rect, SDL_MapRGB(_surface->format, 255, 255, 255));
        }

        SDL_Color textColor = {0, 0, 0};
        SDL_Surface* textSurface = TTF_RenderText_Solid(_font.get(), _levels[i]._name.c_str(), textColor);
        SDL_Rect textRect = {static_cast<int32_t>(x + rectWidth / 2 - textSurface->w / 2), static_cast<int32_t>(y + rectHeight / 2 - textSurface->h / 2), textSurface->w, textSurface->h};
        SDL_BlitSurface(textSurface, nullptr, _surface.get(), &textRect);
        SDL_FreeSurface(textSurface);
    }

    // draw squares for pages numbers
    const int totalWidthp = _num_pages * padding / 4 + (_num_pages - 1) * padding / 2;
    int startX = _surface->w / 2 - totalWidthp / 2;
    for (size_t i(0); i < _num_pages; ++i)
    {
        SDL_Rect pageRect = {
            static_cast<int32_t>(startX + i * padding / 2 + i * padding / 4),
            static_cast<int32_t>(_surface->h - padding / 2),                                    
            static_cast<int32_t>(padding / 4),
            static_cast<int32_t>(padding / 4)};
        SDL_FillRect(_surface.get(), &pageRect, SDL_MapRGB(_surface->format, 255, 255, 255));
        if (i == _current_page)
            SDL_FillRect(_surface.get(), &pageRect, SDL_MapRGB(_surface->format, 0, 0, 255));
    }

    return _surface;
}

void BrickBreakerMenu::initSurface()
{
    reloadBackground();
    TTF_SetFontSize(_font.get(), getFontSize());

    _textureManager.loadTexture("./assets/textures/platform.png", "button", _renderer);
}

uint32_t BrickBreakerMenu::getPadding() const
{
    return _surface->w / 10;
}

void BrickBreakerMenu::reloadBackground()
{
    uint32_t level = rand() % _levels.size();
    _background = std::make_unique<BrickBreaker>(_renderer, _levels[level]._path);
    _background->setSurfaceDimensions(_surface->w, _surface->h);
    _background->initSurface();
}

uint32_t BrickBreakerMenu::getFontSize() const
{
    return _surface->w / 45;
}