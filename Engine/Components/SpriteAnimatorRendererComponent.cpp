#include "pch.h"
#include "SpriteAnimatorRendererComponent.h"
#include "Renderer/TextureFrames.h"
#include "Math/MathUtils.h"
#include "Core/StringUtils.h"
#include "Core/Factory.h"
#include "Engine.h"

namespace nu
{
	FACTORY_REGISTER(SpriteAnimatorRendererComponent)

	void SpriteAnimatorRendererComponent::Start()
	{
		if (!m_defaultAnimationName.empty())
		{
			Play(m_defaultAnimationName);
		}
		else
		{
			// default to first element in map
			auto iter = m_spriteAnimations.begin();
			std::string name = iter->first;
			Play(name);
		}
	}

	void SpriteAnimatorRendererComponent::Update(float dt)
	{
		if (!m_spriteAnimation.textureFrames) return;

		m_frameTimer += dt;
		float frameTime = 1.0f / m_spriteAnimation.framesPerSecond;

		// increase frame while frame timer is greater than frame time
		while (m_frameTimer >= frameTime)
		{
			m_frame++;
			if (m_spriteAnimation.loop)
			{
				// loop frames
				m_frame = Wrap(0u, m_spriteAnimation.textureFrames->GetTotalFrames() - 1, m_frame);
			}
			else
			{
				// stop on last frame
				m_frame = Clamp(0u, m_spriteAnimation.textureFrames->GetTotalFrames() - 1, m_frame);
			}

			m_frameTimer -= frameTime;
		}

		m_sourceRect = m_spriteAnimation.textureFrames->GetFrameRect(m_frame);
	}

	void SpriteAnimatorRendererComponent::Play(const std::string& name)
	{
		if (EqualsIgnoreCase(name, m_spriteAnimation.name)) return;

		auto iter = m_spriteAnimations.find(ToLower(name));
		if (iter == m_spriteAnimations.end())
		{
			std::cerr << "Could not find animation " << name << std::endl;
			return;
		}

		m_spriteAnimation = iter->second;

		m_frame = 0;
		m_frameTimer = 0.0f;

		m_texture = m_spriteAnimation.textureFrames->GetTexture();
		m_sourceRect = m_spriteAnimation.textureFrames->GetFrameRect(m_frame);
	}

	bool SpriteAnimatorRendererComponent::IsAnimationDone() const
	{
		return (m_frame == m_spriteAnimation.textureFrames->GetTotalFrames() - 1);
	}

	void SpriteAnimatorRendererComponent::Read(const json::value_t& value)
	{
		SpriteRendererComponent::Read(value);

		JSON_READ_NAME(value, "default_animation", m_defaultAnimationName);

		if (JSON_HAS(value, animations) && JSON_GET(value, animations).IsArray())
		{
			for (auto& animatorValue : JSON_GET(value, animations).GetArray())
			{
				SpriteAnimation spriteAnimation;

				std::string textureAnimationName;
				JSON_READ_NAME(animatorValue, "name", spriteAnimation.name);

				std::string texture_frames;
				JSON_READ_REQ(animatorValue, texture_frames);

				if (!texture_frames.empty())
				{
					spriteAnimation.textureFrames = Resources().Get<TextureFrames>(texture_frames, Engine::Instance().GetRenderer());
					if (!spriteAnimation.textureFrames)
					{
						std::cerr << "Could not load texture frames: " << texture_frames << std::endl;
					}
				}

				JSON_READ_NAME_REQ(animatorValue, "frames_per_second", spriteAnimation.framesPerSecond);
				JSON_READ_NAME(animatorValue, "loop", spriteAnimation.loop);

				m_spriteAnimations[ToLower(spriteAnimation.name)] = spriteAnimation;
			}
		}

	}
}
