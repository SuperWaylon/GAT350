#pragma once

#include "Math/Vector2.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace nu
{
	class Renderer
	{
	public:
		bool Initialize(const char* name, int width, int height);
		void Shutdown();

		void Clear() const;
		void Present() const;

		bool BeginFrame();
		bool EndFrame();

		void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) const;
		void SetColor(float r, float g, float b, float a = 1.0f) const;

		void DrawPoint(float x, float y) const;
		void DrawLine(float x1, float y1, float x2, float y2) const;
		void DrawFillRect(float x, float y, float w, float h) const;
		void DrawRect(float x, float y, float w, float h) const;

		void DrawModel(const class Model& model, const struct Transform& transform) const;

		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

		void SetCamera(const Vector2& camera) { m_camera = camera; }
		void EnableCamera(bool enable = true) { m_cameraEnabled = enable; }

		SDL_GPUDevice* GetGPUDevice() const { return m_gpuDevice; }
		SDL_Window* GetWindow() const { return m_window; }

		void SetPipeline(const class Pipeline& pipeline);
		void SetVertexBuffer(const class VertexBuffer& vertexBuffer);
		void SetIndexBuffer(const class IndexBuffer& indexBuffer);
		void Draw(uint32_t vertexCount);
		void DrawIndex(const class IndexBuffer& indexBuffer);

		void SetVertexUniform(uint32_t slot, const void* data, uint32_t size);
		void SetFragmentUniform(uint32_t slot, const void* data, uint32_t size);
		void SetFragmentTexture(uint32_t slot, const class Texture& texture);

		void CreateDepthBuffer();

		friend class Texture;
		friend class Shader;
		friend class VertexBuffer;
		friend class IndexBuffer;

	private:
		SDL_Window* m_window = nullptr;
		SDL_Renderer* m_renderer = nullptr;

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUCommandBuffer* m_commandBuffer = nullptr;
		SDL_GPURenderPass* m_renderPass = nullptr;
		SDL_GPUTexture* m_depthTexture = nullptr;

		bool m_cameraEnabled = true;
		Vector2 m_camera;

		int m_width = 0;
		int m_height = 0;
	};
}