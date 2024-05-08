#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include "../include/BreakoutMenu.h"

BreakoutMenu::BreakoutMenu(const std::shared_ptr<SDL_Renderer>& renderer, const std::string& directory_path)
: SDLComponent(), _selected_level(0), _num_rows(3), _num_columns(3), _brick_breaker(nullptr), _background(nullptr), _current_page(0), _font(nullptr, nullptr)
{
    _font = std::unique_ptr<TTF_Font, void(*)(TTF_Font*)>(TTF_OpenFont("./assets/fonts/arial/arial.ttf", getFontSize()), TTF_CloseFont);
    if (!_font.get())
        throw std::runtime_error("Failed to load font: " + std::string(TTF_GetError()));

    for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
        if (entry.is_regular_file()) {
            if(entry.path().extension() != ".lvl")
                continue;
            
            _levels.push_back({entry.path().string()});
        }
    }

    if(_levels.empty())
        throw std::runtime_error("No levels found in directory: " + directory_path);
    
    _num_pages = _levels.size() / _num_rows / _num_columns + (_levels.size() % (_num_rows * _num_columns) != 0);
    srand(time(nullptr));
}

void BreakoutMenu::handleResize(const std::pair<int32_t, int32_t>& previousSize, const std::pair<int32_t, int32_t>& newSize)
{
    TTF_SetFontSize(_font.get(), getFontSize());
}

void BreakoutMenu::handleEvents(const SDL_Event& event, const std::shared_ptr<void>& data1, const std::shared_ptr<void>& data2)
{
    // il faut gérer le cas où la fenêtre est redimensionnée et que le jeu est en cours
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        handleResize({*(int32_t*)data1.get(), *(int32_t*)data2.get()}, {event.window.data1, event.window.data2});
        
        _background->setSurfaceDimensions(event.window.data1, event.window.data2);
        _background->handleEvents(event, data1, data2);
    }

    if (_brick_breaker != nullptr)
    {
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
            _brick_breaker->setSurfaceDimensions(event.window.data1, event.window.data2);
        
        _brick_breaker->handleEvents(event, data1, data2);
        return;
    }
    
    
    if (event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_RETURN:
                _brick_breaker = std::make_unique<Breakout>(_renderer, _levels[_selected_level + _current_page * _num_rows * _num_columns]._path);
                _brick_breaker->setSurfaceDimensions(_surface->w, _surface->h);
                _brick_breaker->initSurface();
                break;
            case SDLK_LEFT:
                if(static_cast<int32_t>(_selected_level) % _num_columns == 0)
                {
                    _current_page = (_current_page + _num_pages - 1) % _num_pages;
                    _selected_level = _num_columns * _selected_level / _num_columns + _num_columns - 1;
                }
                else
                    _selected_level -= 1;
                break;
            case SDLK_RIGHT:
                if(_selected_level % _num_columns == _num_columns - 1)
                {
                    _current_page = (_current_page + 1) % _num_pages;
                    _selected_level = _num_columns * _selected_level / _num_columns - _num_columns + 1;
                }
                else if (_selected_level + _current_page * _num_rows * _num_columns < _levels.size() - 1)
                    _selected_level += 1;
                
                break;
            case SDLK_UP:
                _selected_level = (_selected_level + _num_columns * (_num_rows - 1)) % (_num_rows * _num_columns);
                while (_selected_level + _current_page * _num_rows * _num_columns >= _levels.size())
                    _selected_level -= _num_columns;
                break;
            case SDLK_DOWN:                
                _selected_level = (_selected_level + _num_columns) % (_num_rows * _num_columns);
                while (_selected_level + _current_page * _num_rows * _num_columns >= _levels.size())
                    _selected_level -= _num_columns;
            break;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int32_t x = event.button.x;
        int32_t y = event.button.y;

        // check if a level was clicked
        for (size_t i(_current_page * _num_rows * _num_columns); i < _levels.size() && i < _num_rows * _num_columns * (_current_page + 1); ++i)
        {
            const uint32_t padding = getPadding();
            const _Float32 total_width = _surface->w - 2 * padding;
            const _Float32 total_height = _surface->h - 2 * padding;
            const _Float32 rect_width = (total_width - (_num_columns - 1) * padding / 2) / _num_columns;
            const _Float32 rect_height = (total_height - (_num_rows - 1) * padding / 2) / _num_rows;

            int32_t row = (i % (_num_rows * _num_columns)) / _num_columns;
            int32_t col = (i % (_num_rows * _num_columns)) % _num_columns;

            if (x >= padding + col * (rect_width + padding / 2) && x <= padding + col * (rect_width + padding / 2) + rect_width && y >= padding + row * (rect_height + padding / 2) && y <= padding + row * (rect_height + padding / 2) + rect_height)
            {
                _brick_breaker = std::make_unique<Breakout>(_renderer, _levels[i]._path);
                _brick_breaker->setSurfaceDimensions(_surface->w, _surface->h);
                _brick_breaker->initSurface();
                break;
            }
        }

        // check if a page was clicked
        if(y >= static_cast<int32_t>(_surface->h - getPadding() / 2))
        {
            int32_t total_width = _num_pages * getPadding() / 4 + (_num_pages - 1) * getPadding() / 2;
            int32_t start_x = _surface->w / 2 - total_width / 2;
            for (size_t i(0); i < _num_pages; ++i)
            {
                SDL_Rect page_rect = {
                    static_cast<int32_t>(start_x + i * getPadding() / 2 + i * getPadding() / 4),
                    static_cast<int32_t>(_surface->h - getPadding() / 2),                                    
                    static_cast<int32_t>(getPadding() / 4),
                    static_cast<int32_t>(getPadding() / 4)};
                if (x >= page_rect.x && x <= page_rect.x + page_rect.w)
                {
                    _current_page = i;
                    _selected_level = 0;
                    break;
                }
            }
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        int32_t x = event.motion.x;
        int32_t y = event.motion.y;

        for (size_t i(_current_page * _num_rows * _num_columns); i < _levels.size() && i < _num_rows * _num_columns * (_current_page + 1); ++i)
        {
            const uint32_t padding = getPadding();
            const _Float32 total_width = _surface->w - 2 * padding;
            const _Float32 total_height = _surface->h - 2 * padding;
            const _Float32 rect_width = (total_width - (_num_columns - 1) * padding / 2) / _num_columns;
            const _Float32 rect_height = (total_height - (_num_rows - 1) * padding / 2) / _num_rows;

            int32_t row = (i % (_num_rows * _num_columns)) / _num_columns;
            int32_t col = (i % (_num_rows * _num_columns)) % _num_columns;
            
            if (x >= padding + col * (rect_width + padding / 2) && x <= padding + col * (rect_width + padding / 2) + rect_width && y >= padding + row * (rect_height + padding / 2) && y <= padding + row * (rect_height + padding / 2) + rect_height)
            {
                _selected_level = i % (_num_rows * _num_columns);
                break;
            }
        }
    }
    else if (event.type == SDL_MOUSEWHEEL)
    {
        if (event.wheel.y > 0 || event.wheel.x > 0)
        {
            _current_page = (_current_page + _num_pages - 1) % _num_pages;
            _selected_level = (_num_rows * _num_columns * _current_page) + (_selected_level % (_num_rows * _num_columns));
        }
        else if (event.wheel.y < 0 || event.wheel.x < 0)
        {
            _current_page = (_current_page + 1) % _num_pages;
            _selected_level = (_num_rows * _num_columns * _current_page) + (_selected_level % (_num_rows * _num_columns));
        }
    }
}

void BreakoutMenu::update(uint64_t delta_time)
{
    if (_brick_breaker != nullptr)
    {
        _brick_breaker->update(delta_time);
        if (!_brick_breaker->isRunning())
        {
            _brick_breaker = nullptr;
            reloadBackground();
        }
    }
    else // update background with an automated platform
    {
        _background->update(delta_time);
        
        const SDL_FRect& platform_rect = _background->getPlatform().getRect();
        const Ball& ball = _background->getBalls().front();
        _Float32 initial_platform_speed = _background->getInitialPlatformSpeed();
        
        if (ball.getCenter().first < platform_rect.x)
            _background->getPlatform().setSpeedX(-initial_platform_speed);
        else if (ball.getCenter().first > (platform_rect.x + platform_rect.w))
            _background->getPlatform().setSpeedX(initial_platform_speed);


        if (!_background->isRunning())
        {
            reloadBackground();
        }
    }
}

const std::shared_ptr<SDL_Surface> BreakoutMenu::render()
{
    if (_brick_breaker != nullptr)
    {
        return _brick_breaker->render();
    }

    SDL_FillRect(_surface.get(), nullptr, SDL_MapRGB(_surface->format, 0, 0, 0));
    if (_background != nullptr)
    {
        std::shared_ptr<SDL_Surface> backgroundSurface = _background->render();
        SDL_BlitSurface(backgroundSurface.get(), nullptr, _surface.get(), nullptr);
    }

    const uint32_t padding = getPadding();
    const _Float32 total_width = _surface->w - 2 * padding;
    const _Float32 total_height = _surface->h - 2 * padding;
    const _Float32 rect_width = (total_width - (_num_columns - 1) * padding / 2) / _num_columns;
    const _Float32 rect_height = (total_height - (_num_rows - 1) * padding / 2) / _num_rows;

    for (size_t i(_num_columns * _num_rows * _current_page); i < _levels.size() && i < _num_columns * _num_rows * (_current_page + 1); ++i)
    {
        int32_t row = (i % (_num_rows * _num_columns)) / _num_columns;
        int32_t col = (i % (_num_rows * _num_columns)) % _num_columns;

        _Float32 x = padding + col * (rect_width + padding / 2);
        _Float32 y = padding + row * (rect_height + padding / 2);

        SDL_Rect rect = {static_cast<int32_t>(x), static_cast<int32_t>(y), static_cast<int32_t>(rect_width), static_cast<int32_t>(rect_height)};
        if (_texture_manager.getTexture("button").get() != nullptr)
        {
            if (i == _selected_level + _num_columns * _num_rows * _current_page)
                {rect.x -= padding / 16; rect.y -= padding / 16; rect.w += padding / 8; rect.h += padding / 8;}
            
            SDL_RenderCopy(_renderer.get(), _texture_manager.getTexture("button").get(), nullptr, &rect);
        }
        else
        {
            if (i == _selected_level + _num_columns * _num_rows * _current_page)
                {rect.x -= padding / 16; rect.y -= padding / 16; rect.w += padding / 8; rect.h += padding / 8;}
            
            SDL_FillRect(_surface.get(), &rect, SDL_MapRGB(_surface->format, 255, 255, 255));
        }

        SDL_Color text_color = {0, 0, 0};
        SDL_Surface* text_surface = TTF_RenderText_Solid(_font.get(), _levels[i]._name.c_str(), text_color);
        SDL_Rect text_rect = {static_cast<int32_t>(x + rect_width / 2 - text_surface->w / 2), static_cast<int32_t>(y + rect_height / 2 - text_surface->h / 2), text_surface->w, text_surface->h};
        SDL_BlitSurface(text_surface, nullptr, _surface.get(), &text_rect);
        SDL_FreeSurface(text_surface);
    }

    // draw squares for pages numbers
    const int32_t total_width_p = _num_pages * padding / 4 + (_num_pages - 1) * padding / 2;
    int32_t start_x = _surface->w / 2 - total_width_p / 2;
    for (size_t i(0); i < _num_pages; ++i)
    {
        SDL_Rect page_rect = {
            static_cast<int32_t>(start_x + i * padding / 2 + i * padding / 4),
            static_cast<int32_t>(_surface->h - padding / 2),                                    
            static_cast<int32_t>(padding / 4),
            static_cast<int32_t>(padding / 4)};
        SDL_RenderCopy(_renderer.get(), _texture_manager.getTexture("page_button_not_selected").get(), nullptr, &page_rect);
        if (i == _current_page)
            SDL_RenderCopy(_renderer.get(), _texture_manager.getTexture("page_button_selected").get(), nullptr, &page_rect);
    }

    return _surface;
}

void BreakoutMenu::initSurface()
{
    reloadBackground();
    TTF_SetFontSize(_font.get(), getFontSize());

    _texture_manager.loadDefaultTextures(_renderer);
    _texture_manager.loadTexture("./assets/textures/platform.png", "button", _renderer);
    _texture_manager.loadTexture("./assets/textures/platform.png", "page_button_selected", _renderer);
    _texture_manager.loadTexture("./assets/textures/ball.png", "page_button_not_selected", _renderer);
}

const uint32_t BreakoutMenu::getPadding() const
{
    return _surface->w / 10;
}

void BreakoutMenu::reloadBackground()
{
    uint32_t level = rand() % _levels.size();
    _background = std::make_unique<Breakout>(_renderer, _levels[level]._path);
    _background->setSurfaceDimensions(_surface->w, _surface->h);
    _background->initSurface();
}

const uint32_t BreakoutMenu::getFontSize() const
{
    return _surface->w / 45;
}