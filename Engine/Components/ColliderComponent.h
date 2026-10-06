#pragma once
#include "Component.h"

namespace nu
{
	class ColliderComponent : public Component
	{
	public:
		virtual bool CheckCollision(const ColliderComponent& other) = 0;
	};
}