#pragma once
#include "Resources/Resource.h"
#include <SDL3/SDL.h>

namespace nu
{
	class VertexBuffer : public Resource
	{
	public:
		~VertexBuffer();

		bool Create(uint32_t vertexCount, uint32_t vertexSize, const uint8_t* data, SDL_GPUDevice* gpuDevice);
		
		template<typename T>
		bool Create(const std::vector<T>& vertices, SDL_GPUDevice* gpuDevice);
		
		uint32_t GetVertexCount() const { return m_vertexCount; }

		friend class Renderer;

	private:
		uint32_t m_vertexCount{ 0 };

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUBuffer* m_gpuBuffer = nullptr;
	};

	template<typename T>
	inline bool VertexBuffer::Create(const std::vector<T>& vertices, SDL_GPUDevice* gpuDevice)
	{
		return Create(
			static_cast<uint32_t>(vertices.size()),
			sizeof(T),
			reinterpret_cast<const uint8_t*>(vertices.data()),
			gpuDevice);
	}
}
