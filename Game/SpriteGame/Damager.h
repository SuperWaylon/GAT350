#pragma once
#include "Framework/Actor.h"

class Damager : public nu::Actor
{
public:
	CLASS_PROTOTYPE(Damager)

	void Read(const nu::json::value_t& value) override;

	void SetDamage(float damage) { m_damage = damage; }
	float GetDamage() const { return m_damage; }

protected:
	float m_damage{ 1.0f };
};