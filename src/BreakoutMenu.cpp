#include "../include/BreakoutMenu.h"

BreakoutMenu::BreakoutMenu(const std::string& directory_path)
: SDLComponent(), _selected_level(0), _num_rows(3), _num_columns(3), _launch_level(-1), _breakout(nullptr), _background(nullptr), _current_page(0), _font(nullptr, nullptr)
{
    _font = std::unique_ptr<TTF_Font, void(*)(TTF_Font*)>(TTF_OpenFont("../assets/fonts/arial/arial.ttf", getFontSize()), TTF_CloseFont);
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
    (void)previousSize;
    (void)newSize;
    TTF_SetFontSize(_font.get(), getFontSize());
}

void BreakoutMenu::handleEvents()
{
    EventData event;

    while (hasEvents())
    {
        event = popEvent();

        // il faut gérer le cas où la fenêtre est redimensionnée et que le jeu est en cours
        if (event.event.type == SDL_WINDOWEVENT && event.event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            handleResize({*(int32_t*)event.data1.get(), *(int32_t*)event.data2.get()}, {event.event.window.data1, event.event.window.data2});

            // only send resize event to background
            if(_background)
                _background->pushEvent(event);
        }

        if (_breakout != nullptr)
        {            
            _breakout->pushEvent(event);
            continue;
        }
        
        if (event.event.type == SDL_KEYDOWN)
        {
            switch(event.event.key.keysym.sym)
            {
                case SDLK_RETURN:
                    _launch_level = true;
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
        else if (event.event.type == SDL_MOUSEBUTTONDOWN)
        {
            int32_t x = event.event.button.x;
            int32_t y = event.event.button.y;

            // check if a level was clicked
            for (size_t i(_current_page * _num_rows * _num_columns); i < _levels.size() && i < _num_rows * _num_columns * (_current_page + 1); ++i)
            {
                const uint32_t padding = getPadding();
                const _Float32 total_width = _texture_size.first - 2 * padding;
                const _Float32 total_height = _texture_size.second - 2 * padding;
                const _Float32 rect_width = (total_width - (_num_columns - 1) * padding / 2) / _num_columns;
                const _Float32 rect_height = (total_height - (_num_rows - 1) * padding / 2) / _num_rows;

                int32_t row = (i % (_num_rows * _num_columns)) / _num_columns;
                int32_t col = (i % (_num_rows * _num_columns)) % _num_columns;

                if (x >= padding + col * (rect_width + padding / 2) && x <= padding + col * (rect_width + padding / 2) + rect_width && y >= padding + row * (rect_height + padding / 2) && y <= padding + row * (rect_height + padding / 2) + rect_height)
                {
                    _launch_level = true;
                    break;
                }
            }

            // check if a page was clicked
            if(y >= static_cast<int32_t>(_texture_size.second - getPadding() / 2))
            {
                int32_t total_width = _num_pages * getPadding() / 4 + (_num_pages - 1) * getPadding() / 2;
                int32_t start_x = _texture_size.first / 2 - total_width / 2;
                for (size_t i(0); i < _num_pages; ++i)
                {
                    SDL_Rect page_rect = {
                        static_cast<int32_t>(start_x + i * getPadding() / 2 + i * getPadding() / 4),
                        static_cast<int32_t>(_texture_size.second - getPadding() / 2),                                    
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
        else if (event.event.type == SDL_MOUSEMOTION)
        {
            int32_t x = event.event.motion.x;
            int32_t y = event.event.motion.y;

            for (size_t i(_current_page * _num_rows * _num_columns); i < _levels.size() && i < _num_rows * _num_columns * (_current_page + 1); ++i)
            {
                const uint32_t padding = getPadding();
                const _Float32 total_width = _texture_size.first - 2 * padding;
                const _Float32 total_height = _texture_size.second - 2 * padding;
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
        else if (event.event.type == SDL_MOUSEWHEEL)
        {
            if (event.event.wheel.y > 0 || event.event.wheel.x > 0)
            {
                _current_page = (_current_page + _num_pages - 1) % _num_pages;
                _selected_level = (_num_rows * _num_columns * _current_page) + (_selected_level % (_num_rows * _num_columns));
            }
            else if (event.event.wheel.y < 0 || event.event.wheel.x < 0)
            {
                _current_page = (_current_page + 1) % _num_pages;
                _selected_level = (_num_rows * _num_columns * _current_page) + (_selected_level % (_num_rows * _num_columns));
            }
        }
    }

    // handle events for breakout
    if (_breakout != nullptr)
        _breakout->handleEvents();
    else
        _background->handleEvents();
}

void BreakoutMenu::update(uint64_t delta_time)
{
    if (_breakout != nullptr)
    {
        _breakout->update(delta_time);
    }
    else // update background with an automated Paddle
    {
        _background->update(delta_time);
        
        const SDL_FRect& paddle_rect = _background->getPaddle().getRect();
        const Ball& ball = _background->getBalls().front();
        _Float32 initial_paddle_speed = _background->getInitialPaddleSpeed();
        
        if (ball.getCenter().first < paddle_rect.x)
            _background->getPaddle().setSpeedX(-initial_paddle_speed);
        else if (ball.getCenter().first > (paddle_rect.x + paddle_rect.w))
            _background->getPaddle().setSpeedX(initial_paddle_speed);
    }
}

void BreakoutMenu::render(const std::shared_ptr<SDL_Renderer> renderer)
{
    if(_launch_level != -1)
    {
        _breakout = std::make_unique<Breakout>(_levels[_selected_level + _current_page * _num_rows * _num_columns]._path, true);
        _breakout->setSurfaceDimensions(_texture_size.first, _texture_size.second, renderer);
        _breakout->initSurface(renderer);
        _launch_level = -1;
    }
    
    if (_breakout != nullptr)
    {
        if (!_breakout->isRunning())
        {
            _breakout = nullptr;
            reloadBackground(renderer);
        }
        else
        {
            SDL_SetRenderTarget(renderer.get(), _breakout->getTexture().get());
            _breakout->render(renderer);
            SDL_SetRenderTarget(renderer.get(), _texture.get());
            SDL_RenderCopyEx(renderer.get(), _breakout->getTexture().get(), nullptr, nullptr, 0, nullptr, _breakout->getFlipRenderer() ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
        }
        return;
    }

    // draw background
    if (_background != nullptr)
    {
        if(!_background->isRunning())
            reloadBackground(renderer);
        
        SDL_SetRenderTarget(renderer.get(), _background->getTexture().get());
        _background->render(renderer);
        SDL_SetRenderTarget(renderer.get(), _texture.get());
        SDL_RenderCopyEx(renderer.get(), _background->getTexture().get(), nullptr, nullptr, 0, nullptr, _background->getFlipRenderer() ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
    }

    const uint32_t padding = getPadding();
    const _Float32 total_width = _texture_size.first - 2 * padding;
    const _Float32 total_height = _texture_size.second - 2 * padding;
    const _Float32 rect_width = (total_width - (_num_columns - 1) * padding / 2) / _num_columns;
    const _Float32 rect_height = (total_height - (_num_rows - 1) * padding / 2) / _num_rows;

    // draw squares for levels
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
            
            SDL_RenderCopy(renderer.get(), _texture_manager.getTexture("button").get(), nullptr, &rect);
        }

        SDL_Color text_color = {0, 0, 0, 255};
        std::shared_ptr<SDL_Surface> text_surface(TTF_RenderText_Solid(_font.get(), _levels[i]._name.c_str(), text_color), SDL_FreeSurface);
        SDL_Rect text_rect = {static_cast<int32_t>(x + rect_width / 2 - text_surface->w / 2), static_cast<int32_t>(y + rect_height / 2 - text_surface->h / 2), text_surface->w, text_surface->h};
        SDL_CreateTextureFromSurface(renderer.get(), text_surface.get());
        SDL_RenderCopy(renderer.get(), SDL_CreateTextureFromSurface(renderer.get(), text_surface.get()), nullptr, &text_rect);
    }

    // draw circles for pages numbers
    const int32_t total_width_p = _num_pages * padding / 4 + (_num_pages - 1) * padding / 2;
    int32_t start_x = _texture_size.first / 2 - total_width_p / 2;
    for (size_t i(0); i < _num_pages; ++i)
    {
        SDL_Rect page_rect = {
            static_cast<int32_t>(start_x + i * padding / 2 + i * padding / 4),
            static_cast<int32_t>(_texture_size.second - padding / 2),                                    
            static_cast<int32_t>(padding / 4),
            static_cast<int32_t>(padding / 4)};
        if (i == _current_page)
            SDL_RenderCopy(renderer.get(), _texture_manager.getTexture("page_button_selected").get(), nullptr, &page_rect);
        else
            SDL_RenderCopy(renderer.get(), _texture_manager.getTexture("page_button_not_selected").get(), nullptr, &page_rect);
        // draw number
        SDL_Color color = {0, 0, 0, 0};
        std::stringstream ss;
        ss << i + 1;
        SDL_Surface* s = TTF_RenderText_Solid(_font.get(), ss.str().c_str(), color);
        std::shared_ptr<SDL_Surface> text_surface = std::shared_ptr<SDL_Surface>(s, SDL_FreeSurface);
        int32_t r_w = static_cast<int32_t>(static_cast<_Float32>(text_surface->w) / 1.5f);
        int32_t r_h = static_cast<int32_t>(static_cast<_Float32>(text_surface->h) / 1.5f);
        SDL_Rect dest_rect = {page_rect.x + page_rect.w / 2 - r_w / 2, page_rect.y + page_rect.h / 2 - r_h / 2, r_w, r_h};
        SDL_CreateTextureFromSurface(renderer.get(), text_surface.get());
        SDL_RenderCopy(renderer.get(), SDL_CreateTextureFromSurface(renderer.get(), text_surface.get()), nullptr, &dest_rect);
    }
}

void BreakoutMenu::initSurface(const std::shared_ptr<SDL_Renderer> renderer)
{
    reloadBackground(renderer);
    TTF_SetFontSize(_font.get(), getFontSize());

    _texture_manager.loadDefaultTextures(renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/paddle.png", "button", renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/level_selected.png", "page_button_selected", renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/level_not_selected.png", "page_button_not_selected", renderer);
}

uint32_t BreakoutMenu::getPadding() const
{
    return _texture_size.first / 10;
}

void BreakoutMenu::reloadBackground(std::shared_ptr<SDL_Renderer> renderer)
{
    uint32_t level = rand() % _levels.size();
    _background = std::make_unique<Breakout>(_levels[level]._path, true, true);
    _background->setSurfaceDimensions(_texture_size.first, _texture_size.second, renderer);
    _background->initSurface(renderer);
}

uint32_t BreakoutMenu::getFontSize() const
{
    return _texture_size.first / 45;
}

void BreakoutMenu::onResize(const uint32_t width, const uint32_t height, std::shared_ptr<SDL_Renderer> renderer)
{
    if(_breakout != nullptr)
        _breakout->setSurfaceDimensions(width, height, renderer);

    if(_background != nullptr)
        _background->setSurfaceDimensions(width, height, renderer);
}