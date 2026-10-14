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

		bool Load(const std::string& filename, class Renderer& renderer);

		friend class Pipeline;

	private:
		SDL_GPUShader* m_gpuShader = nullptr;
		SDL_GPUDevice* m_gpuDevice = nullptr;
	};
}
