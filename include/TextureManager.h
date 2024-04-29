#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <map>
#include <string>
#include <memory>

class TextureManager
{
public:
    void loadTexture(const std::string& filename, const std::string& key, const std::shared_ptr<SDL_Renderer>& renderer, const int32_t blending_mode = SDL_BLENDMODE_INVALID);
    const std::shared_ptr<SDL_Texture> getTexture(const std::string& key);
    void updateTextures(const std::shared_ptr<SDL_Renderer>& renderer);
    void clearTextures();

private:
    std::map<std::string, std::tuple<std::string, SDL_BlendMode, std::shared_ptr<SDL_Texture>>> _textures; 
    // on utilise des shared_ptr pour les textures malgré le fait qu'il faut quelles soit détruites avant le renderer, car le renderer est créé avant les textures et dont détruit après
};

#endif // TEXTUREMANAGER_H