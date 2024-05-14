#include "../include/SDLApp.h"

// faire un système de focus pour les fenêtres

const _Float32 SDLApp::_MIN_SCREEN_WIDTH = 1280;
const _Float32 SDLApp::_MIN_SCREEN_HEIGHT = 720;

SDLApp::SDLApp(const int32_t screen_width, const int32_t screen_height, const uint32_t flags, const std::string& font_path)
    : _window(nullptr, SDL_DestroyWindow), _renderer(nullptr, SDL_DestroyRenderer), _font(nullptr), _is_running(false), _components(), _window_dimensions()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        throw std::runtime_error("SDL initialization failed: " + std::string(SDL_GetError()));
    
    uint32_t img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags))
        throw std::runtime_error("SDL_image initialization failed: " + std::string(IMG_GetError()));

    if(TTF_Init() == -1)
        throw std::runtime_error("SDL_ttf initialization failed: " + std::string(TTF_GetError()));

    _window.reset(SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  screen_width, screen_height, SDL_WINDOW_SHOWN | flags), SDL_DestroyWindow);
    if (!_window.get())
        throw std::runtime_error("Window creation failed: " + std::string(SDL_GetError()));

    _renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE), SDL_DestroyRenderer);
    if (!_renderer.get())
        throw std::runtime_error("Renderer creation failed: " + std::string(SDL_GetError()));

    _font.reset(TTF_OpenFont(font_path.c_str(), 24), TTF_CloseFont);
    if (!_font.get())
        std::cerr << "Failed to load font: " << TTF_GetError() << " some functionalities may not work" << std::endl;
    
    SDL_SetWindowMinimumSize(_window.get(), _MIN_SCREEN_WIDTH, _MIN_SCREEN_HEIGHT);

    _window_dimensions = {screen_width, screen_height};
}

void SDLApp::addComponent(const std::shared_ptr<SDLComponent>& obj, const SDL_FRect location, const bool draw_fps)
{
    // no need to lock the mutex here, as the main is adding the components before the main loop starts so no other thread is running
    _components.push_back({.component = obj, .location = location, .fps = 60, .draw_fps = draw_fps});
    obj->setSurfaceDimensions(static_cast<uint32_t>((location.w - location.x) * _window_dimensions.first), static_cast<uint32_t>((location.h - location.y) * _window_dimensions.second), _renderer);
    obj->onResize(static_cast<uint32_t>((location.w - location.x) * _window_dimensions.first), static_cast<uint32_t>((location.h - location.y) * _window_dimensions.second), _renderer);
    obj->initSurface(_renderer);
}

void SDLApp::run(std::set<int32_t> targets, const int32_t desired_fps)
{
    _is_running = true;
    // launch a thread for each component
    std::vector<std::thread> threads;
    for (size_t i = 0; i < _components.size(); i++)
    {
        if(targets.size() > 0 && targets.find(i) == targets.end())
            continue;
        _components[i].component->_is_running = true;
        threads.push_back(std::thread(&SDLApp::loop, this, std::ref(_components[i]), 90));
    }

    // Execute the main loop (events, render)
    const std::chrono::microseconds frame_time = std::chrono::microseconds(1000000) / desired_fps;
    std::chrono::steady_clock::time_point last_time, fps_time;
    fps_time = last_time = std::chrono::steady_clock::now();
    uint32_t frame_count = 0;

    while(_is_running)
    {
        // ------ handle events ------
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                _is_running = false;
                break;
            }

            for (size_t i(0); i < _components.size(); i++)
            {
                if(!_components[i].component->_is_running || (targets.size() > 0 && targets.find(i) == targets.end()))
                    continue;
                _components[i].component->_mutex.lock();
                handleEvents(_components[i], event);
                _components[i].component->_mutex.unlock();
            }

            if(event.type == SDL_WINDOWEVENT && (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED))
                _window_dimensions = {event.window.data1, event.window.data2};
        }   

        // ------ render ------
        SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);
        SDL_RenderClear(_renderer.get());
        for (size_t i(0); i < _components.size(); i++)
        {
            if(!_components[i].component->_is_running || (targets.size() > 0 && targets.find(i) == targets.end()))
                continue;

            // wait for a notification from the component
            // std::unique_lock<std::mutex> lock(_components[i].component->_mutex);
            // _components[i].component->_mutex.lock();
            std::unique_lock<std::mutex> lock(_components[i].component->_mutex);
            _components[i].component->_cv.wait(lock, [&] { return true; });

            render(_components[i]);
            _components[i].component->_mutex.unlock();
        }
        SDL_RenderPresent(_renderer.get());

        // ------ fps ------
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_time);

        if (elapsed_time < frame_time)
            std::this_thread::sleep_for(std::chrono::microseconds(frame_time - elapsed_time));

        frame_count++;
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - fps_time).count() >= 1)
        {
            // std::cout << "Thread main running at " << 1e6 * frame_count / std::chrono::duration_cast<std::chrono::microseconds>(current_time - fps_time).count() << " fps" << std::endl;
            fps_time = std::chrono::steady_clock::now();
            frame_count = 0;
        }
        
        last_time = std::chrono::steady_clock::now();

        // quit if no component is running
        if (std::all_of(_components.begin(), _components.end(), [](const ComponentData& obj) { return !obj.component->_is_running; }))
            _is_running = false;
    }

    // stop all threads
    for(auto i : targets)
        _components[i].component->_is_running = false;

    for(auto& thread : threads)
    {
        // std::cout << "Thread " << thread.get_id() << " stopped" << std::endl;
        thread.join();
    }
}

void SDLApp::handleEvents(ComponentData& target, SDL_Event& event)
{
    if(event.type == SDL_WINDOWEVENT && (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED))
    {   
        SDL_Event m_event;
        m_event.type = SDL_WINDOWEVENT;
        m_event.window.event = SDL_WINDOWEVENT_RESIZED;
        m_event.window.data1 = static_cast<uint32_t>((target.location.w - target.location.x) * event.window.data1);
        m_event.window.data2 = static_cast<uint32_t>((target.location.h - target.location.y) * event.window.data2);
        int32_t old_width = static_cast<int32_t>((target.location.w - target.location.x) * _window_dimensions.first);
        int32_t old_height = static_cast<int32_t>((target.location.h - target.location.y) * _window_dimensions.second);

        target.component->setSurfaceDimensions(static_cast<uint32_t>((target.location.w - target.location.x) * _window_dimensions.first), static_cast<uint32_t>((target.location.h - target.location.y) * _window_dimensions.second), _renderer);
        target.component->onResize(static_cast<uint32_t>((target.location.w - target.location.x) * _window_dimensions.first), static_cast<uint32_t>((target.location.h - target.location.y) * _window_dimensions.second), _renderer);
        // send new size and old size to the component
        target.component->pushEvent({.event = m_event, .data1 = std::make_shared<int32_t>(old_width), .data2 = std::make_shared<int32_t>(old_height)});
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEMOTION)
    {   
        SDL_Event m_event = event;
        int32_t old_x = m_event.motion.x;
        int32_t old_y = m_event.motion.y;
        m_event.motion.x = static_cast<int32_t>((event.motion.x - static_cast<int32_t>(target.location.x * _window_dimensions.first)));
        m_event.motion.y = static_cast<int32_t>((event.motion.y - static_cast<int32_t>(target.location.y * _window_dimensions.second)));

        if(m_event.motion.x < 0 || m_event.motion.y < 0 || m_event.motion.x > static_cast<int32_t>(target.location.w * _window_dimensions.first) || m_event.motion.y > static_cast<int32_t>(target.location.h * _window_dimensions.second))
            return;    
        target.component->pushEvent({.event = m_event, .data1 = std::make_shared<int32_t>(old_x), .data2 = std::make_shared<int32_t>(old_y)});
    }
    else
    {
        target.component->pushEvent({.event = event, .data1 = nullptr, .data2 = nullptr});
    }
}

void SDLApp::render(ComponentData& target)
{
    SDL_SetRenderTarget(_renderer.get(), target.component->_texture.get());
    target.component->render(_renderer);
    SDL_Rect dest_rect = {static_cast<int32_t>(target.location.x * _window_dimensions.first) + 1, static_cast<int32_t>(target.location.y * _window_dimensions.second) + 1, static_cast<int32_t>((target.location.w - target.location.x) * _window_dimensions.first) - 2, static_cast<int32_t>((target.location.h - target.location.y) * _window_dimensions.second) - 2};
            
    // draw fps on right top corner
    if(target.draw_fps && _font)
    {
        SDL_Color color = {0, 255, 0, 255};
        std::string s = std::to_string(target.fps);
        while (s.length() < 2)
            s = "  " + s;
        int32_t w, h;
        TTF_SizeText(_font.get(), s.c_str(), &w, &h);
        SDL_Rect fps_rect = {0, 0, dest_rect.w / 20, static_cast<int32_t>(static_cast<_Float32>(h) * static_cast<_Float32>(dest_rect.w) / static_cast<_Float32>(w) / 20.0f)};
        std::shared_ptr<SDL_Surface> text_surface(TTF_RenderText_Solid(_font.get(), s.c_str(), color), SDL_FreeSurface);
        std::shared_ptr<SDL_Texture> text_texture(SDL_CreateTextureFromSurface(_renderer.get(), text_surface.get()), SDL_DestroyTexture);
        SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);
        SDL_RenderFillRect(_renderer.get(), &fps_rect);
        SDL_RenderCopy(_renderer.get(), text_texture.get(), nullptr, &fps_rect);
    }

    // copy the component's texture to the renderer (the window)
    SDL_SetRenderTarget(_renderer.get(), nullptr);
    SDL_RenderCopy(_renderer.get(), target.component->_texture.get(), nullptr, &dest_rect);
}

void SDLApp::loop(ComponentData& target, const int32_t desired_fps)
{
    const std::chrono::microseconds frame_time = std::chrono::microseconds(1000000) / desired_fps;
    std::chrono::steady_clock::time_point last_time, fps_time, delta_time;
    fps_time = last_time = delta_time = std::chrono::steady_clock::now();
    uint32_t frame_count = 0;
    
    while (_is_running && target.component->_is_running)
    {        
        target.component->_mutex.lock();
        // handle events stored in a deque in the component
        target.component->handleEvents();

        std::chrono::steady_clock::time_point ttime = std::chrono::steady_clock::now();
        target.component->update(std::chrono::duration_cast<std::chrono::milliseconds>(ttime - delta_time).count());
        delta_time = ttime;

        target.component->_mutex.unlock();
        target.component->_cv.notify_one();


        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_time);

        if (elapsed_time < frame_time)
            std::this_thread::sleep_for(std::chrono::microseconds(frame_time - elapsed_time));

        frame_count++;
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - fps_time).count() >= 1)
        {
            double fps = 1e6 * frame_count / std::chrono::duration_cast<std::chrono::microseconds>(current_time - fps_time).count();
            target.fps = round(fps);
            // std::cout << "Thread " << std::this_thread::get_id() << " running at " << fps << " fps" << std::endl;
            fps_time = std::chrono::steady_clock::now();
            frame_count = 0;
        }
        
        last_time = std::chrono::steady_clock::now();
    }
}