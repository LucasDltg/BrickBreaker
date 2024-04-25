#include "../include/TextureManager.h"
#include <iostream>
#include <algorithm>

void TextureManager::loadTexture(const char* fileName, const char* key, const std::shared_ptr<SDL_Renderer> renderer, int32_t blending_mode)
{
    _textures[key] = std::tuple<std::string, SDL_BlendMode, std::shared_ptr<SDL_Texture>>(std::string(fileName), SDL_BLENDMODE_INVALID, std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer.get(), fileName), SDL_DestroyTexture));
    //std::make_pair(std::string(fileName), std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer.get(), fileName), SDL_DestroyTexture));
    if (!std::get<2>(_textures[key]).get())
    {
        std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
    }
    
    // parcourt les 4 premiers bits de flags pour savoir si on doit appliquer une couleur à la texture
    if (blending_mode != SDL_BLENDMODE_INVALID)
        SDL_SetTextureBlendMode(std::get<2>(_textures[key]).get(), static_cast<SDL_BlendMode>(blending_mode & 0xF));

}

std::shared_ptr<SDL_Texture> TextureManager::getTexture(const char* key)
{
    return std::get<2>(_textures[key]);
}

void TextureManager::updateTextures(const std::shared_ptr<SDL_Renderer> renderer)
{
    for (auto& texture : _textures)
    {
        std::shared_ptr<SDL_Texture>& texture_ptr = std::get<2>(texture.second);
        texture_ptr = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer.get(), std::get<0>(texture.second).c_str()), SDL_DestroyTexture);
        if (!std::get<2>(texture.second).get())
        {
            std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
        }

        if (std::get<1>(texture.second) != SDL_BLENDMODE_INVALID)
            SDL_SetTextureBlendMode(std::get<2>(texture.second).get(), std::get<1>(texture.second));
    }
}

void TextureManager::clearTextures()
{
    for (auto& texture : _textures)
    {
        std::get<2>(texture.second).reset();
    }
}