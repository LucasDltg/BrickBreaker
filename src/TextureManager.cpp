#include "../include/TextureManager.h"

void TextureManager::loadDefaultTextures(const std::shared_ptr<SDL_Renderer>& renderer)
{
    _textures["default"] = {.filename = "default", .blending_mode = SDL_BLENDMODE_NONE, .texture = createDefaultTexture(renderer)};
}

bool TextureManager::loadTextureFromFile(const std::string& filename, const std::string& key, const std::shared_ptr<SDL_Renderer>& renderer, const SDL_BlendMode blending_mode)
{
    if(key == "default")
    {
        std::cerr << "Cannot use 'default' as a key" << std::endl;
        return false;
    }
    
    _textures[key] = {.filename = filename, .blending_mode = blending_mode, .texture = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer.get(), filename.c_str()), SDL_DestroyTexture)};
    if (!_textures[key].texture.get())
    {
        std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (blending_mode != SDL_BLENDMODE_INVALID)
        SDL_SetTextureBlendMode(_textures[key].texture.get(), blending_mode);

    return true;
}

bool TextureManager::loadTexture(std::shared_ptr<SDL_Texture> texture, const std::string& key)
{
    if(key == "default")
    {
        std::cerr << "Cannot use 'default' as a key" << std::endl;
        return false;
    }

    // we don't care about the blending mode here
    _textures[key] = {.filename = "", .blending_mode = SDL_BLENDMODE_INVALID, .texture = texture};
    return true;
}

const std::shared_ptr<SDL_Texture> TextureManager::getTexture(const std::string& key)
{
    auto it = _textures.find(key);
    if (it == _textures.end() || it->second.texture.get() == nullptr)
    {
        auto it_default = _textures.find("default");
        if (it_default == _textures.end())
        {
            std::cerr << "No default texture found" << std::endl;
            return nullptr;
        }
        return it_default->second.texture;
    }
    return it->second.texture;
}

void TextureManager::updateTextures(const std::shared_ptr<SDL_Renderer>& renderer)
{
    for (auto& texture : _textures)
    {
        if(texture.first == "default")
        {
            texture.second.texture = createDefaultTexture(renderer);
            continue;
        }
        else if(texture.second.filename.empty()) // skip texture if it was loaded from an existing SDL_Texture
        {
            continue;
        }

        texture.second.texture.reset(IMG_LoadTexture(renderer.get(), texture.second.filename.c_str()), SDL_DestroyTexture);
        if (!texture.second.texture.get())
        {
            std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
            continue;
        }

        if (texture.second.blending_mode != SDL_BLENDMODE_INVALID)
            SDL_SetTextureBlendMode(texture.second.texture.get(), texture.second.blending_mode);
    }
}

void TextureManager::clearTextures()
{
    for (auto& texture : _textures)
    {
        texture.second.texture.reset();
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
    // SDL_SetTextureScaleMode(texture.get(), SDL_ScaleModeNearest);

    SDL_RenderDrawLine(renderer.get(), 0, 0, width-1, height-1);
    SDL_RenderDrawLine(renderer.get(), width-1, 0, 0, height-1);
    
    SDL_RenderDrawLine(renderer.get(), 0, 0, 0, height);
    SDL_RenderDrawLine(renderer.get(), width-1, 0, width-1, height);

    SDL_RenderDrawLine(renderer.get(), 0, 0, width, 0);
    SDL_RenderDrawLine(renderer.get(), 0, height-1, width, height-1);
    
    SDL_SetRenderTarget(renderer.get(), nullptr);
    return texture;
}
