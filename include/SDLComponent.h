#ifndef SDLCOMPONENT_H
#define SDLCOMPONENT_H

#include "../include/TextureManager.h"
#include <mutex>
#include <deque>
#include <atomic>

class SDLApp;
struct ComponentData;

/**
 * @brief The EventData struct represents an SDL event with additional shared data.
 */
struct EventData
{
    SDL_Event event;               ///< The SDL event.
    std::shared_ptr<void> data1;   ///< Additional shared data (optional).
    std::shared_ptr<void> data2;   ///< Additional shared data (optional).
};

/**
 * @brief The SDLComponent class represents a base class for SDL components.
 */
class SDLComponent {
public:
    friend SDLApp;
    friend ComponentData;

    /**
     * @brief Constructor for SDLComponent.
     * 
     * @param renderer The SDL renderer to use.
     */
    SDLComponent();

    /**
     * @brief Sets the dimensions of the surface.
     * 
     * @param width The width of the surface.
     * @param height The height of the surface.
     */
    virtual void setSurfaceDimensions(const uint32_t width, const uint32_t height, const std::shared_ptr<SDL_Renderer> renderer) final;

    /**
     * @brief Handles SDL events.
     * 
     * This method should be overridden by subclasses to handle specific events.
     */
    virtual void handleEvents(const std::shared_ptr<SDL_Renderer> renderer) = 0;

    /**
     * @brief Updates the component.
     * 
     * This method should be overridden by subclasses to update the component's state.
     * 
     * @param delta_time The time elapsed since the last update.
     */
    virtual void update(const uint64_t delta_time, const std::shared_ptr<SDL_Renderer> renderer) = 0;

    /**
     * @brief Renders the component.
     * 
     * This method should be overridden by subclasses to render the component.
     * 
     * @return A shared pointer to the SDL texture representing the rendered component.
     */
    virtual const std::shared_ptr<SDL_Texture> render(const std::shared_ptr<SDL_Renderer> renderer) = 0;

    /**
     * @brief Initializes the surface.
     * 
     * This method should be overridden by subclasses to initialize the surface after the initial size is known.
     * This method is called by the SDLApp after the component is added (so the size is known and can be accessed via _surface)
     * 
     */
    virtual void initSurface(const std::shared_ptr<SDL_Renderer> renderer) = 0;

    /**
     * @brief Push an event onto the event queue.
     * 
     * @param event The event to push.
     */
    void pushEvent(const EventData& event);

    /**
     * @brief Check if the component is running.
     * 
     * @return True if the component is running, false otherwise.
     */
    bool isRunning() const;

    /**
     * @brief Set the running state of the component.
     * 
     * @param running The running state of the component.
     */
    void setRunning(const bool running);

    /**
     * @brief Get the SDL texture.
     * 
     * @return The SDL texture.
     */
    std::shared_ptr<SDL_Texture> getTexture() const;

    virtual void beforeSetSurfaceDimensions(int32_t width, int32_t height)
    {}

protected:
    std::shared_ptr<SDL_Texture> _texture; ///< The SDL texture.
    std::pair<int32_t, int32_t> _texture_size; ///< The size of the texture
    std::atomic<bool> _is_running; ///< Flag indicating if the component is running.
    TextureManager _texture_manager; ///< The texture manager for the component.

    /**
     * @brief Pop an event from the event queue.
     * 
     * @return The popped event.
     */
    EventData popEvent();

    /**
     * @brief Check if there are events in the event queue.
     * 
     * @return True if there are events, false otherwise.
     */
    bool hasEvents() const;

private:
    std::deque<EventData> _events; ///< The events for the component.
};

#endif
