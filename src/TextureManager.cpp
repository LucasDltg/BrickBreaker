#include "../include/TextureManager.h"
#include <iostream>
#include <algorithm>

void TextureManager::loadTexture(const char* fileName, const char* key, const std::shared_ptr<SDL_Renderer> renderer)
{
    textures[key] = std::make_pair(std::string(fileName), std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer.get(), fileName), SDL_DestroyTexture));
    if (!textures[key].second.get())
    {
        std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
    }
}

std::shared_ptr<SDL_Texture> TextureManager::getTexture(const char* key)
{
    return textures[key].second;
}

void TextureManager::updateTextures(const std::shared_ptr<SDL_Renderer> renderer)
{
    for (auto& texture : textures)
    {
        texture.second.second = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer.get(), texture.second.first.c_str()), SDL_DestroyTexture);
        if (!texture.second.second)
        {
            std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
        }
    }
}