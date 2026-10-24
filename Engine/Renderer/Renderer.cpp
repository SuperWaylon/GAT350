#include "pch.h"
#include "Renderer.h"
#include "Model.h"
#include "Texture.h"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "Math/Transform.h"
#include "Math/MathUtils.h"
#include "Math/Rect.h"

namespace nu
{
    bool Renderer::Initialize(const char* name, int width, int height)
    {
        m_width = width;
        m_height = height;

        SDL_Init(SDL_INIT_VIDEO);

        if (!TTF_Init())
        {
            std::cerr << "TTF_Init Error: " << SDL_GetError() << std::endl;
            return false;
        }

        m_window = SDL_CreateWindow(name, width, height, 0);
        if (m_window == nullptr)
        {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        SDL_GPUShaderFormat formats = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL;
        m_gpuDevice = SDL_CreateGPUDevice(formats, true, nullptr);
        if (!m_gpuDevice)
        {
            std::cerr << "Failed to create GPU Device: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            return false;
        }

        // claim the window for our modern GPU context
        SDL_ClaimWindowForGPUDevice(m_gpuDevice, m_window);

        // print out the driver being utilized (e.g., "vulkan" or "d3d12")
        std::cout << "GPU Driver Initialized: " << SDL_GetGPUDeviceDriver(m_gpuDevice) << std::endl;

        CreateDepthBuffer();

        return true;
    }

    bool Renderer::BeginFrame()
    {
        m_renderPass = nullptr;   // reset so a skipped frame never uses a stale pass

        m_commandBuffer = SDL_AcquireGPUCommandBuffer(m_gpuDevice);
        if (!m_commandBuffer)
        {
            std::cerr << "Could not acquire command buffer: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_GPUTexture* swapchainTexture = nullptr;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(m_commandBuffer, m_window, &swapchainTexture, nullptr, nullptr))
        {
            std::cerr << "Could not acquire swapchain texture: " << SDL_GetError() << std::endl;
            return false;
        }

        if (swapchainTexture != nullptr)
        {
            SDL_GPUColorTargetInfo colorTargetInfo{};
            colorTargetInfo.texture = swapchainTexture;
            colorTargetInfo.clear_color = SDL_FColor{ 0.0f, 0.0f, 0.0f, 1.0f };
            colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

            // the depth target handles depth sorting
            SDL_GPUDepthStencilTargetInfo depthTargetInfo{};
            depthTargetInfo.texture = m_depthTexture;
            depthTargetInfo.clear_depth = 1.0f;
            depthTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            depthTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
            depthTargetInfo.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
            depthTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

            // begin ONE render pass, with both the color and depth targets
            m_renderPass = SDL_BeginGPURenderPass(m_commandBuffer, &colorTargetInfo, 1, &depthTargetInfo);
        }

        return true;
    }

    bool Renderer::EndFrame()
    {
        if (m_renderPass)
        {
            SDL_EndGPURenderPass(m_renderPass);
            m_renderPass = nullptr;
        }

        if (!SDL_SubmitGPUCommandBuffer(m_commandBuffer))
        {
            std::cerr << "Could not submit command buffer: " << SDL_GetError() << std::endl;
            return false;
        }

        return true;
    }

    void Renderer::Shutdown()
    {
        TTF_Quit();
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    void Renderer::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
    {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    }

    void Renderer::SetColor(float r, float g, float b, float a) const
    {
        SDL_SetRenderDrawColorFloat(m_renderer, r, g, b, a);
    }

    void Renderer::Clear() const
    {
        SDL_RenderClear(m_renderer);
    }

    void Renderer::Present() const
    {
        SDL_RenderPresent(m_renderer);
    }

    void Renderer::DrawPoint(float x, float y) const
    {
        SDL_RenderPoint(m_renderer, x, y);
    }

    void Renderer::DrawLine(float x1, float y1, float x2, float y2) const
    {
        SDL_RenderLine(m_renderer, x1, y1, x2, y2);
    }

    void Renderer::DrawFillRect(float x, float y, float w, float h) const
    {
        SDL_FRect rect{ x, y, w, h };
        SDL_RenderFillRect(m_renderer, &rect);
    }

    void Renderer::DrawRect(float x, float y, float w, float h) const
    {
        SDL_FRect rect{ x, y, w, h };
        SDL_RenderRect(m_renderer, &rect);
    }

    void Renderer::DrawModel(const Model& model, const Transform& transform) const
    {
        //
    }

    void Renderer::SetPipeline(const Pipeline& pipeline)
    {
        if (!m_renderPass) return;
        // bind the pipeline to the current render pass with SDL_BindGPUGraphicsPipeline(renderPass, gpuPipeline)
        // the pipeline sets the shaders and render settings used by the draw calls that follow
        SDL_BindGPUGraphicsPipeline(m_renderPass, pipeline.m_gpuPipeline);
    }

    void Renderer::SetVertexBuffer(const VertexBuffer& vertexBuffer)
    {
        if (!m_renderPass) return;
        // describe which buffer to bind and where to start reading from
        SDL_GPUBufferBinding binding{
            .buffer = vertexBuffer.m_gpuBuffer, // the vertex buffer's gpu buffer
            .offset = 0   // start reading at the beginning of the buffer
        };

        // bind the vertex buffer to the current render pass
        // the draw calls that follow read their vertices from this buffer
        // slot 0 matches the buffer slot set in Pipeline::AddVertexBuffer()
        SDL_BindGPUVertexBuffers(
            m_renderPass,
            0,
            &binding,
            1
        );
    }

    void Renderer::SetIndexBuffer(const IndexBuffer& indexBuffer)
    {
        if (!m_renderPass) return;
        // describe which buffer to bind and where to start reading from
        SDL_GPUBufferBinding binding{
            .buffer = indexBuffer.m_gpuBuffer, // the index buffer's gpu buffer
            .offset = 0   // start reading at the beginning of the buffer
        };

        // bind the index buffer to the current render pass
        // the indexed draw calls that follow read their indices from this buffer
        SDL_BindGPUIndexBuffer(
            m_renderPass,
            &binding,
            SDL_GPU_INDEXELEMENTSIZE_32BIT
        );
    }

    void Renderer::Draw(uint32_t vertexCount)
    {
        if (!m_renderPass) return;
        // draw using the currently bound pipeline and vertex buffer
        // parameters: render pass, vertex count, instance count (1), first vertex (0), first instance (0)
        SDL_DrawGPUPrimitives(
            m_renderPass,
            vertexCount,
            1,
            0,
            0
        );
    }

    void Renderer::DrawIndex(const IndexBuffer& indexBuffer)
    {
        if (!m_renderPass) return;

        SDL_DrawGPUIndexedPrimitives(
            m_renderPass,
            indexBuffer.GetIndexCount(),
            1,
            0,
            0,
            0
        );
    }

    void Renderer::SetVertexUniform(uint32_t slot, const void* data, uint32_t size)
    {
        SDL_PushGPUVertexUniformData(m_commandBuffer, slot, data, size);
    }

    void Renderer::SetFragmentUniform(uint32_t slot, const void* data, uint32_t size)
    {
        SDL_PushGPUFragmentUniformData(m_commandBuffer, slot, data, size);
    }

    void Renderer::SetFragmentTexture(uint32_t slot, const Texture& texture)
    {
        if (!m_renderPass) return;

        // bind the texture and its sampler to the fragment shader slot
        SDL_GPUTextureSamplerBinding binding
        {
            .texture = texture.m_gpuTexture,
            .sampler = texture.m_gpuSampler
        };

        SDL_BindGPUFragmentSamplers(m_renderPass, slot, &binding, 1);
    }

    void Renderer::CreateDepthBuffer()
    {
        SDL_GPUTextureCreateInfo depthInfo{
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
            .width = (uint32_t)m_width,
            .height = (uint32_t)m_height,
            .layer_count_or_depth = 1,
            .num_levels = 1,
            .sample_count = SDL_GPU_SAMPLECOUNT_1,
        };

        m_depthTexture = SDL_CreateGPUTexture(m_gpuDevice, &depthInfo);
    }
}