#ifndef BRICKBREAKERMENU_H
#define BRICKBREAKERMENU_H

#include "../include/SDLComponent.h"
#include "../include/BrickBreaker.h"
#include <vector>
#include <string>
#include <filesystem>
#include <SDL2/SDL_ttf.h>

/**
 * @brief Represents a level in the BrickBreakerMenu.
 */
class BrickBreakerMenuLevel
{
    friend class BrickBreakerMenu;
public:
    /**
     * @brief Constructor for BrickBreakerMenuLevel.
     * 
     * @param path The path to the level file.
     */
    BrickBreakerMenuLevel(const std::string& path) : _path(path) { std::filesystem::path p(path); _name = p.stem().string(); }

private:
    std::string _name; ///< The name of the level.
    std::string _path; ///< The path to the level file.
};

/**
 * @brief Represents the main menu of the BrickBreaker game.
 */
class BrickBreakerMenu : public SDLComponent
{
public:
    /**
     * @brief Constructor for BrickBreakerMenu.
     * 
     * @param renderer A shared pointer to the SDL renderer.
     * @param directory_path The directory path containing level files. Default is "./assets/levels".
     */
    BrickBreakerMenu(const std::shared_ptr<SDL_Renderer>& renderer, const std::string& directory_path = "./assets/levels");

    /**
     * @brief Handles SDL events.
     * 
     * @param event The SDL event to handle.
     * @param data1 Additional shared data for event handling.
     * @param data2 Additional shared data for event handling.
     */
    void handleEvents(const SDL_Event& event, const std::shared_ptr<void>& data1, const std::shared_ptr<void>& data2) override;

    /**
     * @brief Updates the menu interface.
     * 
     * @param delta_time The time elapsed since the last update.
     */
    void update(const uint64_t delta_time) override;

    /**
     * @brief Renders the menu interface.
     * 
     * @return A shared pointer to the SDL_Surface representing the rendered menu interface.
     */
    const std::shared_ptr<SDL_Surface> render() override;

    /**
     * @brief Initializes the menu surface.
     */
    void initSurface() override;

private:
    /**
     * @brief Get the padding for menu elements.
     * 
     * @return The padding for menu elements.
     */
    const uint32_t getPadding() const;

    /**
     * @brief Get the font size for menu elements.
     * 
     * @return The font size for menu elements.
     */
    const uint32_t getFontSize() const;

    /**
     * @brief Handles resizing of the menu interface.
     * 
     * @param previous_size The previous size of the menu interface.
     * @param new_size The new size of the menu interface.
     */
    void handleResize(const std::pair<int32_t, int32_t>& previous_size, const std::pair<int32_t, int32_t>& new_size);

    /**
     * @brief Reloads the background of the menu interface.
     */
    void reloadBackground();
    
    size_t _selected_level; ///< Index of the currently selected level.
    uint32_t _num_rows; ///< Number of rows in the menu grid.
    uint32_t _num_columns; ///< Number of columns in the menu grid.
    std::vector<BrickBreakerMenuLevel> _levels; ///< List of menu levels.
    std::unique_ptr<BrickBreaker> _brick_breaker; ///< Instance of BrickBreaker for playing levels.
    std::unique_ptr<BrickBreaker> _background; ///< Background image for the menu.
    uint32_t _current_page; ///< Current page of the menu.
    uint32_t _num_pages; ///< Total number of pages in the menu.
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _font; ///< Font used for menu text.
};

#endif // BRICKBREAKERMENU_H
