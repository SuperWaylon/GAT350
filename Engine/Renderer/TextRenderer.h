#pragma once
#include "Font.h"
#include "Math/Vector3.h"
#include "Texture.h"

struct SDL_Texture;

namespace nu
{
	class Renderer;
	
	class TextRenderer
	{
	public:
		TextRenderer() = default;
		TextRenderer(res_t<Font> font) : m_font{ font } {}
		~TextRenderer() = default;

		bool Create(const Renderer& renderer, const std::string& text, const Color& color);
		void Draw(const Renderer& renderer, float x, float y);

	private:
		res_t<Font> m_font;
		res_t<Texture> m_texture;
	};
}