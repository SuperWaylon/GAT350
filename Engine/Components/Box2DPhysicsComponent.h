#pragma once
#include "PhysicsComponent.h"
#include "Physics/PhysicsBody.h"

namespace nu
{
	class Box2DPhysicsComponent : public PhysicsComponent
	{
	public:
		Box2DPhysicsComponent() = default;
		Box2DPhysicsComponent(const Box2DPhysicsComponent& other);

		CLASS_PROTOTYPE(Box2DPhysicsComponent)

		void Start() override;
		void Update(float dt) override;
		void Read(const json::value_t& value) override;

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

	private:
		Vector2 m_size{ 0, 0 };
		Vector2 m_scale{ 1, 1 };

		PhysicsBody::PhysicsBodyDef m_bodyDef;
		std::unique_ptr<PhysicsBody> m_physicsBody;
	};
}