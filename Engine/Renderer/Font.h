#pragma once
#include "Resources/Resource.h"
#include <string>

struct TTF_Font;

namespace nu
{
	class Font : public Resource
	{
	public:
		Font() = default;
		~Font();

		bool Load(const std::string& name, float fontSize);

	private:
		friend class TextRenderer;

		TTF_Font* m_ttfFont{ nullptr };
	};
}