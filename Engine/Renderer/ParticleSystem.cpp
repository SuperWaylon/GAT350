#include "pch.h"
#include "ParticleSystem.h"
#include "Renderer.h"

namespace nu
{
	bool ParticleSystem::Initialize(size_t size)
	{
		// reserve # particles in container
		m_particles.resize(size);

		return true;
	}

	void ParticleSystem::Shutdown() 
	{
		// clear all particles
		m_particles.clear();
	}

	void ParticleSystem::Update(float dt) 
	{
		// update active particles
		for (auto& particle : m_particles) 
		{
			if (!particle.active) continue;

			// reduce lifespan by delta time, active if lifespan > 0
			particle.lifespan -= dt;
			particle.active = (particle.lifespan > 0);
			// update position with velocity
			particle.position += particle.velocity * dt;
		}
	}

	void ParticleSystem::Draw(const Renderer& renderer) 
	{
		// draw all active particlee
		for (auto& particle : m_particles) 
		{
			if (particle.active) 
			{
				// set particle color and draw point at current position
				if (particle.texture)
				{
					renderer.DrawTexture(*particle.texture, particle.position.x, particle.position.y, 0, 0.25f);
				}
			}
		}
	}

	void ParticleSystem::AddParticle(const Particle& particle)
	{
		// get free particle
		Particle* freeParticle = GetFreeParticle();
		// check if free particle is not nullptr
		if (freeParticle)
		{
			// set free particle object with particle
			*freeParticle = particle;
			// set particle active
			freeParticle->active = true;
		}
	}

	Particle* ParticleSystem::GetFreeParticle() 
	{
		// find and return inactive particle 
		for (auto& particle : m_particles) 
		{
			// return pointer to particle
			if (!particle.active) 
				return &particle;
		}

		// no free particles, return nullptr
		return nullptr;
	}
}

