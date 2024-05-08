#ifndef SDLAPP_H
#define SDLAPP_H

#include "../include/SDLComponent.h"
#include <SDL2/SDL_ttf.h>
#include <vector>

/**
 * @brief The SDLApp class represents the main application that manages the SDL window and components.
 * Multiple components can be added to the SDLApp.
 */
class SDLApp {
public:
    /**
     * @brief Minimum screen width for the SDL window.
     */
    static constexpr _Float32 _MIN_SCREEN_WIDTH = 1092;

    /**
     * @brief Minimum screen height for the SDL window.
     */
    static constexpr _Float32 _MIN_SCREEN_HEIGHT = 600;
    
    /**
     * @brief Constructor for SDLApp.
     * 
     * @param screen_width The width of the SDL window.
     * @param screen_height The height of the SDL window.
     * @param window_flags Additional flags for creating the SDL window (optional).
     */
    SDLApp(const int32_t screen_width, const int32_t screen_height, const uint32_t window_flags = 0);

    /**
     * @brief Adds a component to the SDLApp.
     * 
     * @param component The SDL component to add.
     */
    void addComponent(const std::shared_ptr<SDLComponent>& component);

    /**
     * @brief Runs the main loop of the SDL application.
     * The main loop handles events, updates the applications state and renders the scene.
     */
    void run();

    /**
     * @brief Gets the SDL window.
     * 
     * @return A shared pointer to the SDL window.
     */
    const std::shared_ptr<SDL_Window> getWindow() const;

    /**
     * @brief Gets the SDL renderer.
     * 
     * @return A shared pointer to the SDL renderer.
     */
    const std::shared_ptr<SDL_Renderer> getRenderer() const;
    
private:
    std::shared_ptr<SDL_Window> _window; ///< The SDL window.
    std::shared_ptr<SDL_Renderer> _renderer; ///< The SDL renderer.
    bool _is_running; ///< Flag indicating whether the application is running.
    std::vector<std::shared_ptr<SDLComponent>> _components; ///< Vector of SDL components.
    uint64_t _last_time; ///< Timestamp of the last frame.
    std::pair<uint32_t, uint32_t> _window_dimensions; ///< Dimensions of the SDL window.

    /**
     * @brief Handles SDL events.
     */
    void handleEvents();

    /**
     * @brief Updates the application state.
     * 
     * @param delta_time The time elapsed since the last update.
     */
    void update(const uint64_t delta_time);

    /**
     * @brief Renders the scene.
     */
    void render();
};

#endif