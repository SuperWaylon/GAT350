#pragma once
#include "Resources/Resource.h"
#include <SDL3/SDL.h>
#include <vector>

namespace nu
{
	class IndexBuffer : public Resource
	{
	public:
		~IndexBuffer();

		bool Create(uint32_t indexCount, const uint32_t* data, SDL_GPUDevice* gpuDevice);

		// convenience overload so a std::vector<uint32_t> can be passed directly
		bool Create(const std::vector<uint32_t>& indices, SDL_GPUDevice* gpuDevice)
		{
			return Create(static_cast<uint32_t>(indices.size()), indices.data(), gpuDevice);
		}

		uint32_t GetIndexCount() const { return m_indexCount; }

		friend class Renderer;

	private:
		uint32_t m_indexCount{ 0 };

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUBuffer* m_gpuBuffer = nullptr;
	};
}