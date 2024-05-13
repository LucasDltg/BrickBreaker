#ifndef BREAKOUTMENU_H
#define BREAKOUTMENU_H

#include "../include/SDLComponent.h"
#include "../include/Breakout.h"
#include <filesystem>

/**
 * @brief Represents a level in the BreakoutMenu.
 */
class BreakoutMenuLevel
{
    friend class BreakoutMenu;
public:
    /**
     * @brief Constructor for BreakoutMenuLevel.
     * 
     * @param path The path to the level file.
     */
    BreakoutMenuLevel(const std::string& path) : _path(path) { std::filesystem::path p(path); _name = p.stem().string(); }

private:
    std::string _name; ///< The name of the level.
    std::string _path; ///< The path to the level file.
};

/**
 * @brief Represents the main menu of the Breakout game.
 */
class BreakoutMenu : public SDLComponent
{
public:
    /**
     * @brief Constructor for BreakoutMenu.
     * 
     * @param directory_path The directory path containing level files. Default is "./assets/levels".
     */
    BreakoutMenu(const std::string& directory_path = "./assets/levels");

    /**
     * @brief Handles SDL events.
     * 
     * @param event The SDL event to handle.
     * @param data1 Additional shared data for event handling.
     * @param data2 Additional shared data for event handling.
     */
    void handleEvents() override;

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
    const std::shared_ptr<SDL_Texture> render(const std::shared_ptr<SDL_Renderer> renderer) override;

    /**
     * @brief Initializes the menu surface.
     */
    void initSurface(const std::shared_ptr<SDL_Renderer> renderer) override;

private:
    /**
     * @brief Get the padding for menu elements.
     * 
     * @return The padding for menu elements.
     */
    uint32_t getPadding() const;

    /**
     * @brief Get the font size for menu elements.
     * 
     * @return The font size for menu elements.
     */
    uint32_t getFontSize() const;

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
    void reloadBackground(std::shared_ptr<SDL_Renderer> renderer);

    void onResize(const uint32_t width, const uint32_t height, std::shared_ptr<SDL_Renderer> renderer) override;
    
    size_t _selected_level; ///< Index of the currently selected level.
    uint32_t _num_rows; ///< Number of rows in the menu grid.
    uint32_t _num_columns; ///< Number of columns in the menu grid.
    std::vector<BreakoutMenuLevel> _levels; ///< List of menu levels.
    int32_t _launch_level; ///< Flag to launch the selected level.
    std::unique_ptr<Breakout> _breakout; ///< Instance of Breakout for playing levels.
    std::unique_ptr<Breakout> _background; ///< Background instance of Breakout for the menu.
    uint32_t _current_page; ///< Current page of the menu.
    uint32_t _num_pages; ///< Total number of pages in the menu.
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _font; ///< Font used for menu text.
};

#endif // BREAKOUTMENU_H
