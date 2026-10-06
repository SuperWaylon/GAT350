#include "pch.h"
#include "Texture.h"
#include "Renderer.h"

namespace nu
{
    Texture::Texture(SDL_Texture* texture) : m_texture{ texture }
    {
        // cache size
        SDL_GetTextureSize(m_texture, &m_size.x, &m_size.y);
    }

    Texture::~Texture()
    {
        // if texture exists, destroy texture
        if (m_texture) SDL_DestroyTexture(m_texture);
    }

    bool Texture::Load(const std::string& filename, Renderer& renderer)
    {
        // load image onto surface
        SDL_Surface* surface = IMG_Load(filename.c_str());
        if (!surface)
        {
            std::cerr << "Could not load image: " << filename << std::endl;
            return false;
        }

        // create texture from surface, texture is a friend class of renderer
        m_texture = SDL_CreateTextureFromSurface(renderer.m_renderer, surface);
        // once texture is created, surface can be freed up
        SDL_DestroySurface(surface);
        if (!m_texture)
        {
            std::cerr << "Could not create texture: " << filename << std::endl;
            return false;
        }

        // cache size
        SDL_GetTextureSize(m_texture, &m_size.x, &m_size.y);

        return true;
    }
}