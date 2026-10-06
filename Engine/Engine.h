#pragma once

#include "Renderer/Renderer.h"
#include "Renderer/ParticleSystem.h"
#include "Core/GameTime.h"
#include "Input/Input.h"
#include "Audio/Audio.h"
#include "Physics/Physics.h"

#include "Core/Singleton.h"

namespace nu
{
	class Engine : public Singleton<Engine>
	{
	public:
		bool Initialize();
		void Shutdown();

		void Update();

		Input& GetInput() { return *m_input; }
		Renderer& GetRenderer() { return *m_renderer; }
		Audio& GetAudio() { return *m_audio; }
		Time& GetTime() { return m_time; }
		ParticleSystem& GetPS() { return *m_particleSystem; }
		Physics& GetPhysics() { return *m_physics; }

	private:
		friend class Singleton<Engine>;
		Engine() = default;

	private:
		std::unique_ptr<Input> m_input;
		std::unique_ptr<Renderer> m_renderer;
		std::unique_ptr<Audio> m_audio;
		std::unique_ptr<ParticleSystem> m_particleSystem;
		std::unique_ptr<Physics> m_physics;

		Time m_time;
	};
}
