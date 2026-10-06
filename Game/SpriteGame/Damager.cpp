#include "Damager.h"
#include "Core/Factory.h"

FACTORY_REGISTER(Damager)

void Damager::Read(const nu::json::value_t& value)
{
	nu::Actor::Read(value);

	JSON_READ_NAME(value, "damage", m_damage);
}
