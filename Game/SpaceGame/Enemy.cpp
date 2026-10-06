#include "Enemy.h"
#include "Player.h"
#include "Engine.h"
#include "SpaceGame.h"
#include "Core/Factory.h"
#include "Framework/Scene.h"
#include "Math/MathUtils.h"
#include "Core/Random.h"
#include "Components/PhysicsComponent.h"

#include <iostream>

FACTORY_REGISTER(Enemy)

void Enemy::Update(float dt)
{
	Player* player = m_scene->GetActorByName<Player>("PlayerPrototype");
	if (player)
	{

		nu::PhysicsComponent* physicsComponent = GetComponent<nu::PhysicsComponent>();
		if (physicsComponent)
		{
			nu::Vector2 forward{ 1, 0 }; // ->
			nu::Vector2 force = forward.Rotate(m_transform.rotation * nu::DegToRad) * m_speed;
			physicsComponent->ApplyForce(force);

			nu::Vector2 direction = player->GetTransform().position - m_transform.position;
			float rotation = direction.Angle();
			physicsComponent->SetRotation(rotation * nu::RadToDeg);
		}
	}

	nu::Particle particle;
	nu::Vector2 offset{ -20.0f, 0.0f };
	offset = offset.Rotate(m_transform.rotation * nu::DegToRad);
	particle.position = m_transform.position + offset;

	particle.texture = nu::Resources().Get<nu::Texture>("textures/particle.png", nu::Engine::Instance().GetRenderer());
	particle.lifespan = nu::RandomFloat(0.15f, 0.5f);
	particle.velocity = nu::Vector2{ nu::RandomFloat(-100.0f, -30.0f), 0.0f }.Rotate((m_transform.rotation + nu::RandomInt(-30, 30)) * nu::DegToRad);

	nu::Engine::Instance().GetPS().AddParticle(particle);


	Actor::Update(dt);
}

void Enemy::OnCollision(Actor* other)
{
	if (other->GetTag() == "PlayerBullet")
	{
		other->SetDestroyed();

		m_health -= 1.0f;
		if (m_health <= 0.0f)
		{
			SetDestroyed();

			((SpaceGame*)m_scene->GetGame())->AddPoints(m_points);

			nu::Engine::Instance().GetAudio().PlaySound("explosion");
			// create particle explosion
			for (int i = 0; i < 100; i++)
			{
				nu::Particle particle;
				particle.position = m_transform.position;
				particle.texture = nu::Resources().Get<nu::Texture>("textures/particle.png", nu::Engine::Instance().GetRenderer());
				particle.lifespan = nu::RandomFloat(0.15f, 0.75f);
				particle.velocity = { nu::RandomFloat(-600.0f, 600.0f), nu::RandomFloat(-600.0f, 600.0f) };

				nu::Engine::Instance().GetPS().AddParticle(particle);
			}
		}
	}
}

void Enemy::Read(const nu::json::value_t& value)
{
	Actor::Read(value);

	JSON_READ_NAME(value, "points", m_points);
	JSON_READ_NAME(value, "health", m_health);
	JSON_READ_NAME(value, "speed", m_speed);
}