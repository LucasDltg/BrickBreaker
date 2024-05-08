#include "../include/TextureManager.h"
#include <iostream>
#include <algorithm>

void TextureManager::loadDefaultTextures(const std::shared_ptr<SDL_Renderer>& renderer)
{
    _textures["default"] = std::tuple<std::string, SDL_BlendMode, std::shared_ptr<SDL_Texture>>("default", SDL_BLENDMODE_NONE, createDefaultTexture(renderer));
}

void TextureManager::loadTexture(const std::string& filename, const std::string& key, const std::shared_ptr<SDL_Renderer>& renderer, const int32_t blending_mode)
{
    _textures[key] = std::tuple<std::string, SDL_BlendMode, std::shared_ptr<SDL_Texture>>(filename, SDL_BLENDMODE_INVALID, std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer.get(), filename.c_str()), SDL_DestroyTexture));
    if (!std::get<2>(_textures[key]).get())
    {
        std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
        return;
    }
    
    // parcourt les 4 premiers bits de flags pour savoir si on doit appliquer une couleur à la texture
    if (blending_mode != SDL_BLENDMODE_INVALID)
        SDL_SetTextureBlendMode(std::get<2>(_textures[key]).get(), static_cast<SDL_BlendMode>(blending_mode & 0xF));

}

const std::shared_ptr<SDL_Texture> TextureManager::getTexture(const std::string& key)
{
    auto it = _textures.find(key);
    if (std::get<2>(it->second) == nullptr)
    {
        auto it_default = _textures.find("default");
        if (it_default == _textures.end())
        {
            std::cerr << "No default texture found" << std::endl;
            return nullptr;
        }
        return std::get<2>(it_default->second);
    }
    return std::get<2>(it->second);
}

void TextureManager::updateTextures(const std::shared_ptr<SDL_Renderer>& renderer)
{
    for (auto& texture : _textures)
    {
        if(std::get<0>(texture.second) == "default")
        {
            std::get<2>(texture.second) = createDefaultTexture(renderer);
            continue;
        }

        std::shared_ptr<SDL_Texture>& texture_ptr = std::get<2>(texture.second);
        texture_ptr = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer.get(), std::get<0>(texture.second).c_str()), SDL_DestroyTexture);
        if (!std::get<2>(texture.second).get())
        {
            std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
            continue;
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

std::shared_ptr<SDL_Texture> TextureManager::createDefaultTexture(const std::shared_ptr<SDL_Renderer>& renderer)
{
    uint32_t width = 32;
    uint32_t height = 32;

    std::shared_ptr<SDL_Texture> texture(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height), SDL_DestroyTexture);
    if (!texture.get())
    {
        std::cerr << "Failed to create default texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    // draw green lines on the texture (diagonals, vertical and horizontal lines)
    SDL_SetRenderTarget(renderer.get(), texture.get());
    SDL_SetRenderDrawColor(renderer.get(), 0, 255, 0, 255);
    SDL_SetTextureScaleMode(texture.get(), SDL_ScaleModeNearest);

    SDL_RenderDrawLine(renderer.get(), 0, 0, width-1, height-1);
    SDL_RenderDrawLine(renderer.get(), width-1, 0, 0, height-1);
    
    SDL_RenderDrawLine(renderer.get(), 0, 0, 0, height);
    SDL_RenderDrawLine(renderer.get(), width-1, 0, width-1, height);

    SDL_RenderDrawLine(renderer.get(), 0, 0, width, 0);
    SDL_RenderDrawLine(renderer.get(), 0, height-1, width, height-1);
    
    SDL_SetRenderTarget(renderer.get(), nullptr);
    return texture;
}
