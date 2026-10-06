#include "Player.h"
#include "Bullet.h"
#include "Engine.h"
#include "Renderer/Renderer.h"
#include "Core/Factory.h"
#include "Core/Random.h"
#include "Math/MathUtils.h"
#include "Components/PhysicsComponent.h"
#include "Framework/Scene.h"

#include "SpaceGame.h"

FACTORY_REGISTER(Player)

void Player::Update(float dt)
{
	// movement
	float thrust = 0.0f;
	if (nu::Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_W)) thrust =  m_speed;
	if (nu::Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_S)) thrust = -m_speed;


	float rotate = 0.0f;
	if (nu::Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_A)) rotate = -40.0f;
	if (nu::Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_D)) rotate = +40.0f;

	nu::PhysicsComponent* physicsComponent = GetComponent<nu::PhysicsComponent>();
	if (physicsComponent)
	{
		nu::Vector2 forward{ 1, 0 }; // ->
		nu::Vector2 force = forward.Rotate(m_transform.rotation * nu::DegToRad) * thrust;

		physicsComponent->ApplyForce(force);
		physicsComponent->ApplyTorque(rotate);

		nu::Vector2 position = physicsComponent->GetPosition();
		//position.x = nu::Wrap(0.0f, 1280.0f, position.x);
		//position.y = nu::Wrap(0.0f, 1024.0f, position.y);
		//physicsComponent->SetPosition(position);

		nu::Engine::Instance().GetRenderer().SetCamera(position);
	}

	// particle system
	if (thrust)
	{
		nu::Particle particle;
		nu::Vector2 offset{ -20.0f, 0.0f };
		offset = offset.Rotate(m_transform.rotation * nu::DegToRad);
		particle.position = m_transform.position + offset;

		particle.texture = nu::Resources().Get<nu::Texture>("textures/particle.png", nu::Engine::Instance().GetRenderer());
		particle.lifespan = nu::RandomFloat(0.5f, 1.5f);
		particle.velocity = nu::Vector2{ nu::RandomFloat(-100.0f, -30.0f), 0.0f}.Rotate((m_transform.rotation + nu::RandomInt(-30, 30)) * nu::DegToRad);

		nu::Engine::Instance().GetPS().AddParticle(particle);
	}

	// fire
	m_fireTimer -= dt;
	if (m_fireTimer <= 0.0f && nu::Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_SPACE))
	{
		bool burst = (m_fireTimer <= -1.0f);

		m_fireTimer = 0.25f;
		nu::Engine::Instance().GetAudio().PlaySound("laser", true);

		auto bullet = nu::Factory::Instance().Create<Bullet>("BulletPrototype");
		bullet->SetTransform(m_transform);
		bullet->SetScale(2.0f);
		bullet->SetTag("PlayerBullet");

		m_scene->AddActor(std::move(bullet));
	}

	// bullet time
	if (nu::Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_X))
	{
		nu::Engine::Instance().GetTime().SetTimeScale(0.5f);
	}
	else
	{
		nu::Engine::Instance().GetTime().SetTimeScale(1.0f);
	}


	Actor::Update(dt);
}

void Player::OnCollision(Actor* other)
{
	return; // DON'T DIE!!!

	if (other->GetTag() == "Enemy")
	{
		SetDestroyed();
		other->SetDestroyed();

		nu::Engine::Instance().GetAudio().PlaySound("explosion");
		((SpaceGame*)m_scene->GetGame())->OnPlayerDead();
	}
}

void Player::Read(const nu::json::value_t& value)
{
	Actor::Read(value);

	JSON_READ_NAME(value, "speed", m_speed);
}