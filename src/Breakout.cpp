#include "../include/Breakout.h"
#include "../include/Ball.h"
#include "../include/Paddle.h"

Breakout::Breakout(const std::string& filename, bool run, bool is_background)
: SDLComponent(run), _paddle(), _start_duration(1000), _font(nullptr, TTF_CloseFont), _is_background(is_background), _is_rendered_flipped(0), _lives(3)
{
    createBricksFromLevel(filename);
    
    _font = std::unique_ptr<TTF_Font, void(*)(TTF_Font*)>(TTF_OpenFont("../assets/fonts/arial/arial.ttf", 24), TTF_CloseFont);
    if (!_font.get())
        throw std::runtime_error("Failed to load font: " + std::string(TTF_GetError()));
}

void Breakout::initSurface(const std::shared_ptr<SDL_Renderer> renderer)
{
    // std::cout << "Breakout::initSurface " << _texture_size.first << " " << _texture_size.second << std::endl;
    std::pair<uint32_t, uint32_t> center = {_texture_size.first / 2, _texture_size.second * 3/ 4};
    std::pair<_Float32, _Float32> speed = {getInitialBallSpeed() / 3, -getInitialBallSpeed()};
    _balls.push_back(Ball(getBallRadius(), center, speed));

    _paddle.setRect({static_cast<_Float32>(_texture_size.first) / 2.0f - static_cast<_Float32>(_texture_size.first) / 14.0f, static_cast<_Float32>(_texture_size.second) * 0.9f, static_cast<_Float32>(_texture_size.first) / 7.0f, static_cast<_Float32>(_texture_size.second) / 30.0f});

    for (auto& brick : _bricks)
    {
        brick->calculateVerticesWithPosition(_grid_dimensions, {static_cast<_Float32>(_texture_size.first), static_cast<_Float32>(_texture_size.second * Breakout::_BRICK_HEIGHT_LIMIT)});
    }

    _texture_manager.loadDefaultTextures(renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/small_crack.png", "small", renderer, SDL_BLENDMODE_MUL);
    _texture_manager.loadTextureFromFile("../assets/textures/medium_crack.png", "medium", renderer, SDL_BLENDMODE_MUL);
    _texture_manager.loadTextureFromFile("../assets/textures/big_crack.png", "big", renderer, SDL_BLENDMODE_MUL);
    _texture_manager.loadTextureFromFile("../assets/textures/infinite_resistance.png", "infinite", renderer, SDL_BLENDMODE_MUL);
    _texture_manager.loadTextureFromFile("../assets/textures/ball.png", typeid(Ball).name(), renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/paddle.png", typeid(Paddle).name(), renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/bubble_duplicate.png", typeid(DuplicateBallPowerUp).name(), renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/bubble_multi.png", typeid(AddBallPowerUp).name(), renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/bubble_extend.png", typeid(ExtendPaddlePowerUp).name(), renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/bubble_speed.png", typeid(SpeedUpPowerUp).name(), renderer);
    _texture_manager.loadTextureFromFile("../assets/textures/bubble_flip.png", typeid(FlipRendererPowerUp).name(), renderer);
}

void Breakout::handleResize(const std::pair<int32_t, int32_t>& previous_size, const std::pair<int32_t, int32_t>& new_size)
{
    for (auto& brick : _bricks)
    {
        brick->calculateVerticesWithPosition(_grid_dimensions, {static_cast<_Float32>(new_size.first), static_cast<_Float32>(new_size.second * Breakout::_BRICK_HEIGHT_LIMIT)});
    }
    
    for (auto& ball : _balls)
    {
        std::pair<_Float32, _Float32> center = ball.getCenter();

        uint32_t new_x = static_cast<uint32_t>(static_cast<_Float32>(center.first) * (static_cast<_Float32>(new_size.first) / static_cast<_Float32>(previous_size.first)));
        uint32_t new_y = static_cast<uint32_t>(static_cast<_Float32>(center.second) * (static_cast<_Float32>(new_size.second) / static_cast<_Float32>(previous_size.second)));

        ball.setCenter({new_x, new_y});
        ball.setSpeed({ball.getSpeed().first * (static_cast<_Float32>(new_size.first) / static_cast<_Float32>(previous_size.first)), ball.getSpeed().second * (static_cast<_Float32>(new_size.second) / static_cast<_Float32>(previous_size.second))});
        ball.setRadius(ball.getRadius() * (static_cast<_Float32>(new_size.first) / static_cast<_Float32>(previous_size.first)));
    }

    for (auto& power_up : _power_ups)
    {
        std::pair<_Float32, _Float32> center = power_up->getCenter();

        uint32_t new_x = static_cast<uint32_t>(static_cast<_Float32>(center.first) * (static_cast<_Float32>(new_size.first) / static_cast<_Float32>(previous_size.first)));
        uint32_t new_y = static_cast<uint32_t>(static_cast<_Float32>(center.second) * (static_cast<_Float32>(new_size.second) / static_cast<_Float32>(previous_size.second)));

        power_up->setCenter({new_x, new_y});
        power_up->setSpeed({power_up->getSpeed().first * (static_cast<_Float32>(new_size.first) / static_cast<_Float32>(previous_size.first)), power_up->getSpeed().second * (static_cast<_Float32>(new_size.second) / static_cast<_Float32>(previous_size.second))});
        power_up->setRadius(power_up->getRadius() * (static_cast<_Float32>(new_size.first) / static_cast<_Float32>(previous_size.first)));
    }

    // Resize Paddle
    SDL_FRect rect = _paddle.getRect();
    rect.x = rect.x * (static_cast<_Float32>(new_size.first) / static_cast<_Float32>(previous_size.first));
    rect.y = rect.y * (static_cast<_Float32>(new_size.second) / static_cast<_Float32>(previous_size.second));
    rect.w = rect.w * (static_cast<_Float32>(new_size.first) / static_cast<_Float32>(previous_size.first));
    rect.h = rect.h * (static_cast<_Float32>(new_size.second) / static_cast<_Float32>(previous_size.second));
    _paddle.setRect(rect);
}

void Breakout::createBricksFromLevel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open level file: " << filename << std::endl;
        _is_running = false;
        return;
    }

    std::string magic_sequence;
    file >> magic_sequence;
    if (magic_sequence == "rectangle")
        _brick_shape = BrickShape::RECTANGLE;
    else if (magic_sequence == "triangle")
        _brick_shape = BrickShape::TRIANGLE;
    else if (magic_sequence == "hexagon")
        _brick_shape = BrickShape::HEXAGON;
    else
    {
        std::cerr << "Invalid magic sequence in level file: " << filename << std::endl;
        _is_running = false;
        return;
    }

    file >> _grid_dimensions.first >> _grid_dimensions.second;
    if (_grid_dimensions.first <= 0 || _grid_dimensions.second <= 0)
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
        int32_t pos_x, pos_y, resistance;
        std::string color_hex;
        std::string power_up;

        if (!(iss >> pos_x >> pos_y >> resistance >> color_hex))
        {
            std::cerr << "Invalid line format in level file: " << filename << std::endl;
            continue;
        }
        iss >> power_up;
        
        if(pos_x < 0 || pos_x >= _grid_dimensions.first || pos_y < 0 || pos_y >= _grid_dimensions.second)
        {
            std::cerr << "Invalid brick position in level file: " << filename << std::endl;
            continue;
        }

        uint32_t mapped_color;
        std::stringstream(color_hex) >> std::hex >> mapped_color >> std::dec;

        if (_brick_shape == BrickShape::RECTANGLE)
            _bricks.push_back(std::make_unique<BrickRectangular>(std::make_pair(pos_x, pos_y), _grid_dimensions, std::make_pair(_texture_size.first, _texture_size.second * Breakout::_BRICK_HEIGHT_LIMIT), mapped_color, resistance, power_up));
        else if (_brick_shape == BrickShape::TRIANGLE)
            _bricks.push_back(std::make_unique<BrickTriangular>(std::make_pair(pos_x, pos_y), _grid_dimensions, std::make_pair(_texture_size.first, _texture_size.second * Breakout::_BRICK_HEIGHT_LIMIT), mapped_color, resistance, power_up));
        else if (_brick_shape == BrickShape::HEXAGON)
            _bricks.push_back(std::make_unique<BrickHexagonal>(std::make_pair(pos_x, pos_y), _grid_dimensions, std::make_pair(_texture_size.first, _texture_size.second * Breakout::_BRICK_HEIGHT_LIMIT), mapped_color, resistance, power_up));
    }

    file.close();
}

void Breakout::addBall(const Ball& ball)
{
    _balls.push_back(ball);
}

const Paddle& Breakout::getPaddle() const
{
    return _paddle;
}

void Breakout::handleEvents()
{
    EventData event;

    while(hasEvents())
    {
        event = popEvent();

        if (event.event.type == SDL_WINDOWEVENT && event.event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            handleResize({*(int32_t*)event.data1.get(), *(int32_t*)event.data2.get()}, {event.event.window.data1, event.event.window.data2});
        }
        else if (event.event.type == SDL_MOUSEMOTION)
        {
            _paddle.setRect({event.event.motion.x - _paddle.getRect().w / 2, _paddle.getRect().y, _paddle.getRect().w, _paddle.getRect().h});
        }
    }

    if(_is_background)
    {
        return;
    }

    const uint8_t* state = SDL_GetKeyboardState(nullptr);
    if(state[SDL_SCANCODE_ESCAPE])
    {
        _is_running = false;
    }
    else if (state[SDL_SCANCODE_LEFT])
    {
        _paddle.setSpeedX(-getInitialPaddleSpeed());
    }
    else if (state[SDL_SCANCODE_RIGHT])
    {
        _paddle.setSpeedX(getInitialPaddleSpeed());
    }
    else
    {
        _paddle.setSpeedX(0);
    }
}

void Breakout::update(uint64_t delta_time)
{
    // check if the game is starting
    if(_start_duration > 0)
    {
        _start_duration -= delta_time;
        return;
    }
    
    // use CCD technic to avoid tunneling
    const uint32_t max_delta_time = 1000 / 60;
    uint32_t n_loop = std::floor(delta_time / max_delta_time);
    
    for(uint32_t i(0); i < n_loop; i++)
    {
        updateLoop(max_delta_time);
    }
    updateLoop(delta_time - n_loop * max_delta_time);


    if (_bricks.empty() || _balls.empty() || std::all_of(_bricks.begin(), _bricks.end(), [](const std::unique_ptr<Brick>& brick) {
            return brick->getResistance() <= 0;}))
        _start_duration = 2000;
}

void Breakout::render(const std::shared_ptr<SDL_Renderer> renderer)
{
    // reset surface
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderFillRect(renderer.get(), nullptr);
    
    // check if all balls are lost or all bricks are destroyed or have resisitance < 0
    if (_balls.empty() || _bricks.empty() || std::all_of(_bricks.begin(), _bricks.end(), [](const std::unique_ptr<Brick>& brick) {
        return brick->getResistance() <= 0;
    }))
    {
        _is_rendered_flipped = 0;
        if(_is_background)
        {
            _is_running = false;
            return;
        }
        std::string text = _balls.empty() ? "You lost!" : "You won!";
        if (_start_duration <= 0)
            _is_running = false;

        SDL_Color color = {255, 255, 255, 0};
        SDL_Surface* s = TTF_RenderText_Solid(_font.get(), text.c_str(), color);
        std::shared_ptr<SDL_Surface> text_surface = std::shared_ptr<SDL_Surface>(s, SDL_FreeSurface);
        SDL_Rect dest_rect = {_texture_size.first / 2 - text_surface->w / 2, _texture_size.second / 2 - text_surface->h / 2, text_surface->w, text_surface->h};
        SDL_CreateTextureFromSurface(renderer.get(), text_surface.get());
        SDL_RenderCopy(renderer.get(), SDL_CreateTextureFromSurface(renderer.get(), text_surface.get()), nullptr, &dest_rect);
        
        return;
    }

    if (_start_duration > 0 && !_is_background)
    {
        SDL_Color color = {255, 255, 255, 0};
        std::stringstream ss;
        ss << "Starting in " << std::fixed << std::setprecision(1) << _start_duration / 1000.0f << " seconds";
        SDL_Surface* s = TTF_RenderText_Solid(_font.get(), ss.str().c_str(), color);
        std::shared_ptr<SDL_Surface> text_surface = std::shared_ptr<SDL_Surface>(s, SDL_FreeSurface);
        SDL_Rect dest_rect = {_texture_size.first / 2 - text_surface->w / 2, _texture_size.second / 2 - text_surface->h / 2, text_surface->w, text_surface->h};
        SDL_CreateTextureFromSurface(renderer.get(), text_surface.get());
        SDL_RenderCopy(renderer.get(), SDL_CreateTextureFromSurface(renderer.get(), text_surface.get()), nullptr, &dest_rect);
    }

    for (const auto& brick : _bricks)
    {
        std::vector<SDL_Vertex> vertices = brick->getVertices();
        std::vector<int32_t> indices = brick->getIndices();

        if (SDL_RenderGeometry(renderer.get(), nullptr, vertices.data(), vertices.size(), indices.data(), indices.size()) != 0) {
            std::cerr << "Failed to render geometry: " << SDL_GetError() << std::endl;
            return;
        }
        
        // draw resistance texture on brick
        _Float32 resistance_percentage = brick->getResistancePercentage();
        // skip brick with full resistance
        if (resistance_percentage >= 0.25f || brick->getResistance() == -1)
        {
            std::shared_ptr<SDL_Texture> texture;
            if(brick->getResistance() == -1)
                texture = _texture_manager.getTexture("infinite");
            else if (resistance_percentage >= 0.75f)
                texture = _texture_manager.getTexture("big");
            else if (resistance_percentage >= 0.50f)
                texture = _texture_manager.getTexture("medium");
            else
                texture = _texture_manager.getTexture("small");
        
            vertices = brick->getVerticesWithoutColor();
            if(SDL_RenderGeometry(renderer.get(), texture.get(), vertices.data(), vertices.size(), indices.data(), indices.size()) != 0)
            {    
                std::cerr << "Failed to render geometry: " << SDL_GetError() << std::endl;
                return;
            }
        }

        for (size_t i = 0; i < vertices.size(); i++)
        {
            if (SDL_RenderDrawLine(renderer.get(), static_cast<int32_t>(vertices[i].position.x), static_cast<int32_t>(vertices[i].position.y), static_cast<int32_t>(vertices[(i + 1) % vertices.size()].position.x), static_cast<int32_t>(vertices[(i + 1) % vertices.size()].position.y)) != 0)
            {
                std::cerr << "Failed to render line: " << SDL_GetError() << std::endl;
                return;
            }
        }
    }

    for (const auto& ball : _balls)
    {
        const std::pair<uint32_t, uint32_t> position = ball.getCenter();
        _Float32 radius = ball.getRadius();

        SDL_Rect dest_rect = {
            static_cast<int32_t>(position.first - radius),
            static_cast<int32_t>(position.second - radius),
            static_cast<int32_t>(2 * radius),
            static_cast<int32_t>(2 * radius)
        };

        SDL_RenderCopy(renderer.get(), _texture_manager.getTexture(typeid(ball).name()).get(), nullptr, &dest_rect);
    }

    for (const auto& power_up : _power_ups)
    {
        if(power_up->isActive())
        {
            continue;
        }
        std::pair<_Float32, _Float32> position = power_up->getCenter();
        _Float32 radius = power_up->getRadius();
        SDL_Rect rect = {static_cast<int32_t>(position.first - radius), static_cast<int32_t>(position.second - radius), static_cast<int32_t>(2 * radius), static_cast<int32_t>(2 * radius)};

        std::shared_ptr<SDL_Texture> power_up_surface = _texture_manager.getTexture(typeid(*power_up).name());
        SDL_RenderCopy(renderer.get(), power_up_surface.get(), nullptr, &rect);
    }

    const SDL_FRect& rect = _paddle.getRect();
    SDL_Rect dest_rect = {static_cast<int32_t>(rect.x), static_cast<int32_t>(rect.y), static_cast<int32_t>(rect.w), static_cast<int32_t>(rect.h)};
    std::shared_ptr<SDL_Texture> Paddle_surface = _texture_manager.getTexture(typeid(_paddle).name());
    SDL_RenderCopy(renderer.get(), Paddle_surface.get(), nullptr, &dest_rect);

    // draw lives
    SDL_Color color = {255, 255, 255, 0};
    std::stringstream ss;
    ss << "Lives: " << _lives;
    SDL_Surface* s = TTF_RenderText_Solid(_font.get(), ss.str().c_str(), color);
    std::shared_ptr<SDL_Surface> text_surface = std::shared_ptr<SDL_Surface>(s, SDL_FreeSurface);
    SDL_Rect lives_rect = {_texture_size.first - text_surface->w - 10, 10, text_surface->w, text_surface->h};
    SDL_CreateTextureFromSurface(renderer.get(), text_surface.get());
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderFillRect(renderer.get(), &lives_rect);
    SDL_RenderCopy(renderer.get(), SDL_CreateTextureFromSurface(renderer.get(), text_surface.get()), nullptr, &lives_rect);
}

_Float32 Breakout::getBallRadius() const
{
    return static_cast<_Float32>(_texture_size.first) / 80.0f;
}

_Float32 Breakout::getInitialBallSpeed() const
{
    return static_cast<_Float32>(_texture_size.first) / 3000.0f;
}

Paddle& Breakout::getPaddle()
{
    return _paddle;
}

std::vector<Ball>& Breakout::getBalls()
{
    return _balls;
}

_Float32 Breakout::getInitialPaddleSpeed() const
{
    return static_cast<_Float32>(_texture_size.first) / 700.0f;
}

void Breakout::updateLoop(int64_t delta_time)
{
    _paddle.update(delta_time, _texture_size.first);

    for (auto& power_up : _power_ups)
    {
        if (!power_up->isActive())
        {
            power_up->update(delta_time);
            if(power_up->resolveCollisionWithRectangle(_paddle.getRect()))
            {
                power_up->applyPowerUp(*this);
            }
        }
        else
        {
            power_up->decrementDuration(delta_time);
        }
    }

    // remove power-up if it is out of bounds or its duration is over
    _power_ups.erase(std::remove_if(_power_ups.begin(), _power_ups.end(), [this](const std::unique_ptr<PowerUp>& power_up) {
        if (power_up->getCenter().second + power_up->getRadius() > _texture_size.second && !power_up->isActive())
        {
            return true;
        }
        if(power_up->isActive() && power_up->getDuration() <= 0)
        {
            power_up->unApplyPowerUp(*this);
            return true;
        }
        return false;
    }), _power_ups.end());

    
    for (auto& ball : _balls)
    {
        for(auto& brick : _bricks)
        {
            std::vector<SDL_Vertex> v = brick->getVertices();
            bool collision = false;
            
            for (size_t i = 0; i < v.size(); i++)
            {
                if (ball.resolveCollisionWithLine({v[i].position.x, v[i].position.y}, {v[(i + 1) % v.size()].position.x, v[(i + 1) % v.size()].position.y}, delta_time))
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
                    std::unique_ptr<PowerUp> power_up = brick->getPowerUp();
                    if(power_up)
                    {
                        _power_ups.push_back(std::move(power_up));
                        _power_ups.back()->setRadius(getBallRadius());
                        _power_ups.back()->setCenter(brick->getCenter());
                        _power_ups.back()->setSpeed({0, static_cast<_Float32>(_texture_size.second) / 4000.0f});
                    }
                    // we remove the brick from the vector here, because we don't want to check for collision with it anymore
                    _bricks.erase(std::remove_if(_bricks.begin(), _bricks.end(), [&brick](const std::unique_ptr<Brick>& b) {
                        return b.get() == brick.get();
                    }), _bricks.end());
                }
                break;
            }
        }

        // Check for collision with Paddle
        ball.resolveCollisionWithRectangle(_paddle.getRect());

        // Check for collision with borders
        std::vector<std::pair<_Float32, _Float32>> borders = {
            {0, 0},
            {static_cast<_Float32>(_texture_size.first), 0},
            {static_cast<_Float32>(_texture_size.first), static_cast<_Float32>(_texture_size.second)},
            {0, static_cast<_Float32>(_texture_size.second)}
        };
        
        ball.resolveCollisionWithLine(borders[0], borders[1], delta_time);
        ball.resolveCollisionWithLine(borders[1], borders[2], delta_time);
        ball.resolveCollisionWithLine(borders[3], borders[0], delta_time);
        
        // we only lose life is there is one ball left
        if(ball.resolveCollisionWithLine(borders[2], borders[3], delta_time) && _balls.size() == 1)
        {
            _lives--;
            if(_lives == 0)
                _balls.clear();
        }

        ball.update(delta_time);
    }
    
    if(_balls.size() > 1)
    {
        // remove balls that are out of bounds
        _balls.erase(std::remove_if(_balls.begin(), _balls.end(), [this](const Ball& ball) {
            return ball.getCenter().second - ball.getRadius() > this->getPaddle().getRect().y + this->getPaddle().getRect().h;
        }), _balls.end());   
    }
}

void Breakout::increaseFlipRenderer()
{
    _is_rendered_flipped++;
}

void Breakout::decreaseFlipRenderer()
{
    _is_rendered_flipped--;
}

int32_t Breakout::getFlipRenderer() const
{
    return _is_rendered_flipped;
}
