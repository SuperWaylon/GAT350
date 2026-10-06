#include "pch.h"
#include "Physics.h"
#include "Framework/Actor.h"

namespace nu 
{
	float Physics::m_pixelsPerUnit = 48.0f;

	bool Physics::Initialize() 
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = b2Vec2{ 0.0f, 10.0f };
		m_worldId = b2CreateWorld(&worldDef);

		return true;
	}

	void Physics::Shutdown() 
	{
		b2DestroyWorld(m_worldId);
	}

	void Physics::Update(float dt) 
	{
		b2World_Step(m_worldId, 1.0f / 60.0f, 4);
		ProcessCollisionEvents();
	}

	void Physics::ProcessCollisionEvents()
	{
		b2ContactEvents contactEvents = b2World_GetContactEvents(m_worldId);
		// begin contact
		for (int i = 0; i < contactEvents.beginCount; i++)
		{
			auto contactEvent = contactEvents.beginEvents + i;
			if (!b2Shape_IsValid(contactEvent->shapeIdA) || !b2Shape_IsValid(contactEvent->shapeIdB)) continue;

			b2BodyId bodyA = b2Shape_GetBody(contactEvent->shapeIdA);
			b2BodyId bodyB = b2Shape_GetBody(contactEvent->shapeIdB);

			Actor* actorA = (Actor*)b2Body_GetUserData(bodyA);
			if (actorA == nullptr || actorA->GetDestroyed() || !actorA->IsActive()) continue;

			Actor* actorB = (Actor*)b2Body_GetUserData(bodyB);
			if (actorB == nullptr || actorB->GetDestroyed() || !actorB->IsActive()) continue;

			actorA->OnCollision(actorB);
			actorB->OnCollision(actorA);
		}

		// begin sensor
		b2SensorEvents sensorEvents = b2World_GetSensorEvents(m_worldId);
		for (int i = 0; i < sensorEvents.beginCount; i++)
		{
			auto sensorEvent = sensorEvents.beginEvents + i;
			if (!b2Shape_IsValid(sensorEvent->sensorShapeId) || !b2Shape_IsValid(sensorEvent->visitorShapeId)) continue;

			b2BodyId bodyA = b2Shape_GetBody(sensorEvent->sensorShapeId);
			b2BodyId bodyB = b2Shape_GetBody(sensorEvent->visitorShapeId);

			Actor* actorA = (Actor*)b2Body_GetUserData(bodyA);
			if (actorA == nullptr || actorA->GetDestroyed() || !actorA->IsActive()) continue;

			Actor* actorB = (Actor*)b2Body_GetUserData(bodyB);
			if (actorB == nullptr || actorB->GetDestroyed() || !actorB->IsActive()) continue;

			actorA->OnCollision(actorB);
			actorB->OnCollision(actorA);
		}
	}
}