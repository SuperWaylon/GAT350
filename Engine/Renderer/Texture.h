#pragma once
#include "Resources/Resource.h"

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <string>

namespace nu
{
	class Texture : public Resource
	{
	public:
		~Texture();

		bool Load(const std::string& filename, const class Renderer& renderer);

		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }

		glm::vec2 GetSize() const { return { m_width, m_height }; }

		friend class Renderer;

	private:
		uint32_t m_width{ 0 };
		uint32_t m_height{ 0 };

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUTexture* m_gpuTexture = nullptr;
		SDL_GPUSampler* m_gpuSampler = nullptr;
	};
}
