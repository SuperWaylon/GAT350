#pragma once
#include "ColliderComponent.h"

namespace nu
{
	class CircleColliderComponent : public ColliderComponent
	{
	public:
		CLASS_PROTOTYPE(CircleColliderComponent)

		bool CheckCollision(const ColliderComponent& other) override;

		void Read(const json::value_t& value) override;

		float GetRadius() const { return m_radius; }

	protected:
		float m_radius{ 0.0f };
	};
}