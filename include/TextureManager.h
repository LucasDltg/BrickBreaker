#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SDL2/SDL_image.h>
#include <map>
#include <memory>
#include <iostream>

// we can add later a surcharge of the loadTextureFromFile function to manage user defined textures

/**
 * @brief The TextureManager class manages SDL textures.
 */
class TextureManager
{
public:
    /**
     * @brief Loads default textures.
     * 
     * @param renderer A shared pointer to the SDL renderer.
     */
    void loadDefaultTextures(const std::shared_ptr<SDL_Renderer>& renderer);

    /**
     * @brief Loads a texture from file.
     * 
     * @param filename The path to the image file.
     * @param key The key to associate with the loaded texture.
     * @param renderer A shared pointer to the SDL renderer.
     * @param blending_mode The blending mode for the texture (optional).
     * @return True if the texture was loaded successfully, false otherwise.
     */
    bool loadTextureFromFile(const std::string& filename, const std::string& key, const std::shared_ptr<SDL_Renderer>& renderer, const SDL_BlendMode blending_mode = SDL_BLENDMODE_INVALID);

    /**
     * @brief Loads a texture from an existing SDL texture.
     * 
     * @param texture A shared pointer to the SDL texture.
     * @param key The key to associate with the loaded texture.
     * @return True if the texture was loaded successfully, false otherwise.
     */
    bool loadTexture(std::shared_ptr<SDL_Texture> texture, const std::string& key);

    /**
     * @brief Retrieves a texture by key, if it does not exist, returns the default texture, if it does not exist, returns nullptr.
     * 
     * @param key The key of the texture to retrieve.
     * @return A shared pointer to the requested SDL texture.
     */
    const std::shared_ptr<SDL_Texture> getTexture(const std::string& key);

    /**
     * @brief Updates the textures when renderer is updated (automatically called when resizing).
     * 
     * @param renderer A shared pointer to the SDL renderer.
     */
    void updateTextures(const std::shared_ptr<SDL_Renderer>& renderer);

    /**
     * @brief Clears all loaded textures (automatically called)
     */
    void clearTextures();

private:
    /**
     * @brief Creates a default texture.
     * 
     * @param renderer A shared pointer to the SDL renderer.
     * @return A shared pointer to the created default texture.
     */
    std::shared_ptr<SDL_Texture> createDefaultTexture(const std::shared_ptr<SDL_Renderer>& renderer);

    /**
     * @brief The TextureData struct represents data for a texture.
     */
    typedef struct TextureData
    {
        std::string filename; // the filename of the texture
        SDL_BlendMode blending_mode; // the blending mode of the texture
        std::shared_ptr<SDL_Texture> texture; // the texture
    } TextureData;

    std::map<std::string, TextureData> _textures; ///< The textures.
};

#endif // TEXTUREMANAGER_H
