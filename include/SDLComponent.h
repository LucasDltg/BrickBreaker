#ifndef SDLCOMPONENT_H
#define SDLCOMPONENT_H

#include "../include/TextureManager.h"

/**
 * @brief The SDLComponent class represents a base class for SDL components.
 */
class SDLComponent {
public:
    /**
     * @brief Constructor for SDLComponent.
     */
    SDLComponent();

    /**
     * @brief Sets the dimensions of the surface.
     * 
     * @param width The width of the surface.
     * @param height The height of the surface.
     */
    virtual void setSurfaceDimensions(const uint32_t width, const uint32_t height) final;

    /**
     * @brief Handles SDL events.
     * 
     * This method should be overridden by subclasses to handle specific events.
     * 
     * @param event The SDL event to handle.
     * @param data1 Additional shared data (optional).
     * @param data2 Additional shared data (optional).
     */
    virtual void handleEvents(const SDL_Event& event, const std::shared_ptr<void>& data1, const std::shared_ptr<void>& data2) = 0;

    /**
     * @brief Updates the component.
     * 
     * This method should be overridden by subclasses to update the component's state.
     * 
     * @param delta_time The time elapsed since the last update.
     */
    virtual void update(const uint64_t delta_time) = 0;

    /**
     * @brief Renders the component.
     * 
     * This method should be overridden by subclasses to render the component.
     * 
     * @return A shared pointer to the SDL surface representing the rendered component.
     */
    virtual const std::shared_ptr<SDL_Surface> render() = 0;

    /**
     * @brief Initializes the surface.
     * 
     * This method should be overridden by subclasses to initialize the surface after the initial size is known.
     * This method is called by the SDLApp after the component is added (so the size is known and can be accessed via _surface)
     */
    virtual void initSurface() = 0;

    /**
     * @brief Checks if the component is running.
     * 
     * @return True if the component is running, false otherwise.
     */
    bool isRunning() const;

protected:
    std::shared_ptr<SDL_Surface> _surface; ///< The SDL surface.
    bool _is_running; ///< Flag indicating whether the component is running.
    std::shared_ptr<SDL_Renderer> _renderer; ///< The SDL renderer.
    TextureManager _texture_manager; ///< The texture manager for the component.
};

#endif
