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
    void loadTexture(const char* fileName, const char* key, const std::shared_ptr<SDL_Renderer> renderer);
    std::shared_ptr<SDL_Texture> getTexture(const char* key);
    void updateTextures(const std::shared_ptr<SDL_Renderer> renderer);
    void clearTextures();

private:
    std::map<std::string, std::pair<std::string, std::shared_ptr<SDL_Texture>> > textures; 
    // on utilise des shared_ptr pour les textures malgré le fait qu'il faut quelles soit détruites avant le renderer, car le renderer est créé avant les textures et dont détruit après
};

#endif // TEXTUREMANAGER_H