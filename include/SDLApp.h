#ifndef SDLAPP_H
#define SDLAPP_H

#include "../include/SDLComponent.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_audio.h>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>

/**
 * @brief The ComponentData struct represents data for an SDL component.
 */
struct ComponentData
{
    std::shared_ptr<SDLComponent> component;  ///< The SDL component.
    SDL_FRect location;                       ///< The location of the component on the screen.
    int32_t fps;                              ///< Frames per second for the component.
    bool draw_fps;                            ///< Flag indicating whether to draw FPS on the component.
};

/**
 * @brief The SDLApp class represents the main application that manages the SDL window and components.
 * Multiple components can be added to the SDLApp.
 */
class SDLApp {
public:
    /**
     * @brief Minimum screen width for the SDL window.
     */
    static const _Float32 _MIN_SCREEN_WIDTH;

    /**
     * @brief Minimum screen height for the SDL window.
     */
    static const _Float32 _MIN_SCREEN_HEIGHT;
    
    /**
     * @brief Constructor for SDLApp.
     * 
     * @param screen_width The width of the SDL window.
     * @param screen_height The height of the SDL window.
     * @param window_flags Additional flags for the SDL window (optional).
     * @param font_path The path to the font file for rendering text (optional).
     */
    SDLApp(const int32_t screen_width, const int32_t screen_height, const uint32_t window_flags = 0, const std::string& font_path = "assets/arial.ttf");

    /**
     * @brief Add a component to the SDLApp.
     * 
     * @param component The component to add.
     * @param location The location and size of the component on the screen (default is full screen).
     * @param draw_fps Flag indicating whether to draw FPS on the component (default is false).
     */
    void addComponent(const std::shared_ptr<SDLComponent>& component, const SDL_FRect location={0.0f, 0.0f, 1.0f, 1.0f}, const bool draw_fps=false);

    /**
     * @brief Run the SDL application.
     * 
     * @param targets Set of component IDs to target (default is all components).
     * @param desired_fps The desired frames per second for the application (default is 60).
     */
    void run(std::set<int32_t> targets = {}, const int32_t desired_fps=60);
    
private:
    std::shared_ptr<SDL_Window> _window;             ///< The SDL window.
    std::shared_ptr<SDL_Renderer> _renderer;         ///< The SDL renderer.
    std::shared_ptr<TTF_Font> _font;                 ///< The SDL font.
    std::atomic<bool> _is_running;                   ///< Flag to indicate if the application is running.
    std::vector<ComponentData> _components;          ///< The components of the application.
    std::pair<uint32_t, uint32_t> _window_dimensions;///< Dimensions of the SDL window.

    /**
     * @brief Handle SDL events for a target component.
     * 
     * @param target The target component data.
     * @param event The SDL event to handle.
     */
    void handleEvents(ComponentData& target, SDL_Event& event);

    /**
     * @brief Render a target component.
     * 
     * @param target The target component data to render.
     */
    void render(ComponentData& target);

    /**
     * @brief Main loop for a target component.
     * 
     * @param target The target component data.
     * @param desired_fps The desired frames per second for the component.
     */
    void loop(ComponentData& target, const int32_t desired_fps=60);
};

#endif // SDLAPP_H
