#include "pch.h"
#include "PhysicsComponent.h"

namespace nu
{
	void PhysicsComponent::Read(const json::value_t& value)
	{
		Component::Read(value);

		JSON_READ_NAME(value, "mass", m_mass);
		JSON_READ_NAME(value, "damping", m_damping);
		JSON_READ_NAME(value, "angular_damping", m_angularDamping);
		JSON_READ_NAME(value, "gravity_scale", m_gravityScale);
	}
}

