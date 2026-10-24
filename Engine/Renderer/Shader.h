#pragma once
#include "Resources/Resource.h"
#include <SDL3/SDL.h>
#include <string>

namespace nu
{
	class Shader : public Resource
	{
	public:
		virtual ~Shader();

		// uniformCount is the number of uniform buffers (cbuffers) the shader declares
		// it defaults to 1, use 0 for shaders that have no cbuffer
		// samplerCount is the number of textures (with samplers) the shader reads, it defaults to 0
		bool Load(const std::string& filename, class Renderer& renderer, uint32_t uniformCount = 1, uint32_t samplerCount = 0);

		friend class Pipeline;

	private:
		SDL_GPUShader* m_gpuShader = nullptr;
		SDL_GPUDevice* m_gpuDevice = nullptr;
	};
}