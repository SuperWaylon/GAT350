#pragma once
#include "Resources/Resource.h"

#include <SDL3/SDL.h>
#include <vector>

namespace nu
{
    class Shader;


    class Pipeline : public Resource
    {
    public:
        ~Pipeline();

        bool Create(
            const Shader& vertexShader,
            const Shader& fragmentShader,
            SDL_GPUDevice* gpuDevice,
            SDL_Window* window);

        void AddVertexBuffer(uint32_t pitch);
        void AddVertexAttribute(uint32_t location, SDL_GPUVertexElementFormat format, uint32_t offset);

        friend class Renderer;

    private:
        SDL_GPUDevice* m_gpuDevice = nullptr;
        SDL_GPUGraphicsPipeline* m_gpuPipeline = nullptr;

        std::vector<SDL_GPUVertexBufferDescription> m_vertexBufferDescriptions;
        std::vector<SDL_GPUVertexAttribute> m_vertexAttributes;

        SDL_GPUPrimitiveType m_primitiveType = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    };
}