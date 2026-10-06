// Engine::Instance().cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "Engine.h"

namespace nu
{
	bool Engine::Initialize()
	{
		m_renderer = std::make_unique<Renderer>();
		m_particleSystem = std::make_unique<ParticleSystem>();
		m_audio = std::make_unique<Audio>();
		m_input = std::make_unique<Input>();
		m_physics = std::make_unique<Physics>();

		m_renderer->Initialize("Game Engine", 1280, 1024);
		m_particleSystem->Initialize();
		m_audio->Initialize();
		m_input->Initialize();
		m_physics->Initialize();

		return true;
	}

	void Engine::Shutdown()
	{
		m_physics->Shutdown();
		m_input->Shutdown();
		m_audio->Shutdown();
		m_particleSystem->Shutdown();
		m_renderer->Shutdown();
	}

	void Engine::Update()
	{
		m_time.Tick();

		m_audio->Update();
		m_input->Update();
		m_physics->Update(m_time.GetDeltaTime());
		m_particleSystem->Update(m_time.GetDeltaTime());
	}
}
