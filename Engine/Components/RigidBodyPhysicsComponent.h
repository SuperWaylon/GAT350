#pragma once
#include "PhysicsComponent.h"

namespace nu
{
	class RigidBodyPhysicsComponent : public PhysicsComponent
	{
	public:
		CLASS_PROTOTYPE(RigidBodyPhysicsComponent)

		void Update(float dt) override;

		void ApplyForce(const Vector2& force) override;
		void SetVelocity(const Vector2& velocity) override;
		Vector2 GetVelocity() override;

		void ApplyTorque(float torque) override;
		void SetAngularVelocity(float angularVelocity) override;
		float GetAngularVelocity() const override;
		
		void SetPosition(const Vector2& position) override;
		Vector2 GetPosition() const override;

		void SetRotation(float rotation) override;
		float GetRotation() const override;

		void Read(const json::value_t& value) override;

	private:
		Vector2 m_acceleration{ 0.0f, 0.0f };
		Vector2 m_velocity{ 0.0f, 0.0f };

		float m_angularAcceleration{ 0.0f };
		float m_angularVelocity{ 0.0f };
	};
}
