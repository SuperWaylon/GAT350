#include "pch.h"
#include "PhysicsBody.h"
#include "Math/MathUtils.h"
#include "Physics.h"

namespace nu
{
	PhysicsBody::PhysicsBody(const Transform& transform, const Vector2& size, const PhysicsBodyDef& def, const Physics& physics) 
	{
		b2BodyDef bodyDef = b2DefaultBodyDef();

		// set body definition
		bodyDef.type = (def.isDynamic) ? b2_dynamicBody : b2_staticBody;
		bodyDef.position = Physics::ToB2(Physics::ScreenToWorld(transform.position));
		bodyDef.rotation = b2MakeRot(transform.rotation * DegToRad);
		bodyDef.motionLocks.angularZ = def.constrainAngle;
		bodyDef.gravityScale = def.gravityScale;
		bodyDef.linearDamping = def.linearDamping;
		bodyDef.angularDamping = def.angularDamping;
		bodyDef.enableSleep = false;
		bodyDef.userData = def.actor;

		// create body
		m_bodyId = b2CreateBody(physics.m_worldId, &bodyDef);

		// set shape definition
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.material.friction = def.friction;
		shapeDef.material.restitution = def.restitution;
		shapeDef.density = def.density;
		shapeDef.isSensor = def.isSensor;

		shapeDef.enableSensorEvents = true;
		shapeDef.enableContactEvents = true;


		// create shape
		b2Vec2 hsize = Physics::ToB2(Physics::ScreenToWorld(size * transform.scale * 0.5f));
		switch (def.shape)
		{
		case Shape::Box:
		{
			b2Polygon box = b2MakeBox(hsize.x, hsize.y);
			b2CreatePolygonShape(m_bodyId, &shapeDef, &box);
		}
		break;
		case Shape::Capsule:
		{
			b2Capsule capsule{ b2Vec2{ 0, hsize.y - hsize.x }, b2Vec2{ 0, -(hsize.y - hsize.x) }, hsize.x };
			b2CreateCapsuleShape(m_bodyId, &shapeDef, &capsule);
		}
		break;
		case Shape::Circle:
		{
			b2Circle circle{ b2Vec2{ 0, 0 }, hsize.x };
			b2CreateCircleShape(m_bodyId, &shapeDef, &circle);
		}
		break;
		default:
			break;
		}
	}

	PhysicsBody::~PhysicsBody() 
	{
		b2DestroyBody(m_bodyId);
	}

	Vector2 PhysicsBody::GetPosition() const
	{
		return Physics::WorldToScreen(Physics::ToVector2(b2Body_GetPosition(m_bodyId)));
	}

	void PhysicsBody::SetPosition(const Vector2& position) const
	{
		b2Vec2 b2Position = Physics::ToB2(Physics::ScreenToWorld(position));
		b2Rot b2Rotation = b2Body_GetRotation(m_bodyId);

		b2Body_SetTransform(
			m_bodyId,
			b2Position,
			b2Rotation
		);
	}

	float PhysicsBody::GetRotation() const
	{
		return b2Rot_GetAngle(b2Body_GetRotation(m_bodyId));
	}

	void PhysicsBody::SetRotation(float radians) const
	{
		b2Vec2 position = b2Body_GetPosition(m_bodyId);

		b2Body_SetTransform(
			m_bodyId,
			position,
			b2MakeRot(radians)
		);
	}

	void PhysicsBody::ApplyForce(const Vector2& force) 
	{
		b2Body_ApplyForceToCenter(m_bodyId, Physics::ToB2(Physics::ScreenToWorld(force)), true);
	}

	void PhysicsBody::ApplyTorque(float radians) 
	{
		b2Body_ApplyTorque(m_bodyId, radians, true);
	}

	void PhysicsBody::SetAngularVelocity(float angularVelocity)
	{
		b2Body_SetAngularVelocity(m_bodyId, angularVelocity);
	}

	float PhysicsBody::GetAngularVelocity()
	{
		return b2Body_GetAngularVelocity(m_bodyId);;
	}

	void PhysicsBody::SetVelocity(const Vector2& velocity) 
	{
		b2Body_SetLinearVelocity(m_bodyId, Physics::ToB2(Physics::ScreenToWorld(velocity)));
	}

	Vector2 PhysicsBody::GetVelocity() 
	{
		return Physics::WorldToScreen(Physics::ToVector2(b2Body_GetLinearVelocity(m_bodyId)));
	}
}
