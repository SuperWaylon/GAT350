#pragma once
#include "Framework/Actor.h"

struct EnemyDesc : public nu::ActorDesc
{
	float speed{ 0.0f };
	float health{ 1.0f };
	int points{ 100 };
};

class Enemy : public nu::Actor
{
public:
	Enemy() = default;
	Enemy(const EnemyDesc& enemyDesc) :
		Actor(enemyDesc),
		m_speed{ enemyDesc.speed },
		m_health{ enemyDesc.health },
		m_points{ enemyDesc.points }
	{ }

	CLASS_PROTOTYPE(Enemy)

	void Update(float dt) override;
	void OnCollision(Actor* other) override;

	void Read(const nu::json::value_t& value) override;

private:
	int m_points = 100;
	float m_health = 1.0f;
	float m_speed = 800.0f;
};
