#include "pch.h"
#include "SpriteRendererComponent.h"
#include "Renderer/Renderer.h"
#include "Framework/Actor.h"
#include "Resources/ResourceManager.h"
#include "Core/Factory.h"
#include "Engine.h"

namespace nu
{
	FACTORY_REGISTER(SpriteRendererComponent)

	void SpriteRendererComponent::Start()
	{
		if (!m_textureName.empty())
		{
			m_texture = Resources().Get<Texture>(m_textureName, Engine::Instance().GetRenderer());
			if (m_texture)
			{
				m_size = m_texture->GetSize();
			}
		}
	}

	void SpriteRendererComponent::Draw(const Renderer& renderer)
	{
		if (m_texture)
		{
			if (m_sourceRect.w > 0 && m_sourceRect.h > 0)
			{
				renderer.DrawTexture(*m_texture,
					m_sourceRect,
					GetOwner()->GetTransform().position.x,
					GetOwner()->GetTransform().position.y,
					GetOwner()->GetTransform().rotation,
					GetOwner()->GetTransform().scale,
					m_flipH,
					m_origin);
			}
			else
			{
				renderer.DrawTexture(*m_texture,
					GetOwner()->GetTransform().position.x,
					GetOwner()->GetTransform().position.y,
					GetOwner()->GetTransform().rotation,
					GetOwner()->GetTransform().scale,
					m_flipH,
					m_origin);
			}
		}
	}

	void SpriteRendererComponent::Read(const json::value_t& value)
	{
		RendererComponent::Read(value);

		JSON_READ_NAME(value, "texture", m_textureName);
		JSON_READ_NAME(value, "flipH", m_flipH);
		JSON_READ_NAME(value, "origin", m_origin);

	}
}
