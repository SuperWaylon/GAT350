#include "pch.h"
#include "Renderer.h"
#include "Model.h"
#include "Texture.h"

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

        m_renderer = SDL_CreateRenderer(m_window, NULL);
        if (m_renderer == nullptr) 
        {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            return false;
        }

        SDL_SetDefaultTextureScaleMode(m_renderer, SDL_SCALEMODE_PIXELART);
        SDL_SetRenderVSync(m_renderer, 1);

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
        for (auto mesh : model.GetMeshes())
        {
            SetColor(mesh.GetColor().r, mesh.GetColor().g, mesh.GetColor().b, 1.0f);
            auto& points = mesh.GetPoints();
            for (int i = 0; i + 1 < points.size(); i++)
            {
                // local space
                Vector2 v1 = points[i];
                Vector2 v2 = points[i + 1];

                // convert to world space
                v1 *= transform.scale;
                v2 *= transform.scale;

                v1 = v1.Rotate(transform.rotation * DegToRad);
                v2 = v2.Rotate(transform.rotation * DegToRad);

                v1 += transform.position;
                v2 += transform.position;

                DrawLine(v1.x, v1.y, v2.x, v2.y);
            }
        }
    }

    void Renderer::DrawTexture(const Texture& texture, float x, float y, float angle, float scale, bool flipH, const Vector2& origin) const
    {
        Vector2 size = texture.GetSize();

        float cameraX = (m_cameraEnabled) ? (m_camera.x - m_width * 0.5f) : 0.0f;
        float cameraY = (m_cameraEnabled) ? (m_camera.y - m_height * 0.5f) : 0.0f;

        SDL_FRect destRect;
        destRect.w = size.x * scale;
        destRect.h = size.y * scale;

        destRect.x = (x - cameraX) - (destRect.w * origin.x);
        destRect.y = (y - cameraY) - (destRect.h * origin.y);

        // https://wiki.libsdl.org/SDL3/SDL_RenderTexture
        SDL_RenderTextureRotated(m_renderer, texture.m_texture, NULL, &destRect, angle, NULL, (flipH) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }

    void Renderer::DrawTexture(const Texture& texture, const Rect& source, float x, float y, float angle, float scale, bool flipH, const Vector2& origin) const
    {
        float cameraX = (m_cameraEnabled) ? (m_camera.x - m_width * 0.5f) : 0.0f;
        float cameraY = (m_cameraEnabled) ? (m_camera.y - m_height * 0.5f) : 0.0f;

        SDL_FRect sourceRect;
        sourceRect.x = source.x;
        sourceRect.y = source.y;
        sourceRect.w = source.w;
        sourceRect.h = source.h;

        SDL_FRect destRect;
        destRect.w = source.w * scale;
        destRect.h = source.h * scale;

        destRect.x = (x - cameraX) - (destRect.w * origin.x);
        destRect.y = (y - cameraY) - (destRect.h * origin.y);

        // https://wiki.libsdl.org/SDL3/SDL_RenderTexture
        SDL_RenderTextureRotated(m_renderer, texture.m_texture, &sourceRect, &destRect, angle, NULL, (flipH) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }
}