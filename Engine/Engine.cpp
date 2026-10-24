#include "pch.h"
#include "Engine.h"

#include <iostream>

namespace nu
{
	bool Engine::Initialize()
	{
		// the renderer goes first: Renderer::Initialize calls SDL_Init, and Input needs SDL to be initialized
		m_renderer = std::make_unique<Renderer>();
		if (!m_renderer->Initialize("GAT350", 1280, 720))	// change the title and size to whatever you want
		{
			std::cerr << "Engine: could not initialize renderer" << std::endl;
			return false;
		}

		m_input = std::make_unique<Input>();
		if (!m_input->Initialize())
		{
			std::cerr << "Engine: could not initialize input" << std::endl;
			return false;
		}

		m_audio = std::make_unique<Audio>();
		if (!m_audio->Initialize())
		{
			std::cerr << "Engine: could not initialize audio" << std::endl;
			return false;
		}

		m_particleSystem = std::make_unique<ParticleSystem>();
		if (!m_particleSystem->Initialize())
		{
			std::cerr << "Engine: could not initialize particle system" << std::endl;
			return false;
		}

		return true;
	}

	void Engine::Shutdown()
	{
		// shut down in reverse order of initialization
		// every step checks for null so this is safe even if Initialize failed partway
		if (m_particleSystem)
		{
			m_particleSystem->Shutdown();
			m_particleSystem.reset();
		}

		if (m_audio)
		{
			m_audio->Shutdown();
			m_audio.reset();
		}

		if (m_input)
		{
			m_input->Shutdown();
			m_input.reset();
		}

		if (m_renderer)
		{
			m_renderer->Shutdown();
			m_renderer.reset();
		}
	}

	void Engine::Update()
	{
		m_time.Tick();

		m_input->Update();
		m_audio->Update();
		m_particleSystem->Update(m_time.GetDeltaTime());
	}
}