#pragma once
#include "Framework/Actor.h"

struct BulletDesc : public nu::ActorDesc
{
	float speed = 800.0f;
};

class Bullet : public nu::Actor
{
public:
	Bullet() = default;
	Bullet(const BulletDesc& BulletDesc) :
		Actor(BulletDesc),
		m_speed{ BulletDesc.speed }
	{
	}

	CLASS_PROTOTYPE(Bullet)

	void Update(float dt) override;

	void Read(const nu::json::value_t& value) override;

private:
	float m_speed = 800.0f;
};
