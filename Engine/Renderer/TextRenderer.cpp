#include "pch.h"
#include "TextRenderer.h"
#include "Math/Vector3.h"
#include "Renderer.h"
#include "Texture.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>


namespace nu
{
	bool TextRenderer::Create(const Renderer& renderer, const std::string& text, const Color& color) 
	{
		// create a surface using the font, text string and color
		SDL_Color c{ (uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), 255 };
		SDL_Surface* surface = TTF_RenderText_Solid(m_font->m_ttfFont, text.c_str(), text.size(), c);
		if (surface == nullptr) 
		{
			std::cerr << "Could not create surface.\n";
			return false;
		}

		// create a texture from the surface, only textures can render to the renderer
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.m_renderer, surface);
		if (texture == nullptr) 
		{
			SDL_DestroySurface(surface);
			std::cerr << "Could not create texture" << SDL_GetError() << std::endl;
			return false;
		}

		// free the surface, no longer needed after creating the texture
		SDL_DestroySurface(surface);

		m_texture = std::make_shared<Texture>(texture);

		return true;
	}

	void TextRenderer::Draw(const Renderer& renderer, float x, float y) 
	{
		renderer.DrawTexture(*m_texture, x, y);
	}
}