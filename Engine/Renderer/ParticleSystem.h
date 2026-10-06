#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Texture.h"

#include <vector>

namespace nu
{
	struct Particle
	{
		bool active{ false };
		float lifespan{ 1 };

		Vector2 position{ 0, 0 };
		Vector2 velocity{ 0, 0 };
		Color color{ 0, 0, 0 };
		res_t<Texture> texture;
	};

	class ParticleSystem 
	{
	public:
		ParticleSystem() = default;

		bool Initialize(size_t poolSize = 1000);
		void Shutdown();

		void Update(float dt);
		void Draw(const class Renderer& renderer);

		void AddParticle(const Particle& particle);

	private:
		Particle* GetFreeParticle();

	private:
		// store particles in particle pool
		std::vector<Particle> m_particles;
	};
}
