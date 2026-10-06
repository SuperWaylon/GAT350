#pragma once
#include "Resources/Resource.h"
#include "Math/Vector2.h"
#include <string>

struct SDL_Texture;

namespace nu
{
	class Texture : public Resource
	{
	public:
		Texture() = default;
		Texture(SDL_Texture* texture);
		~Texture();

		bool Load(const std::string& filename, class Renderer& renderer);

		const Vector2& GetSize() const { return m_size; }

		friend class Renderer;

	private:
		SDL_Texture* m_texture{ nullptr };
		Vector2 m_size{ 0.0f, 0.0f };
	};
}