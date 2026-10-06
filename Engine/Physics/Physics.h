#pragma once
#include "Math/Vector2.h"

#include <box2d/box2d.h>
#include <memory>

namespace nu
{
	class Physics 
	{
	public:
		Physics() = default;

		bool Initialize();
		void Shutdown();

		void Update(float dt);

		static b2Vec2 ToB2(const Vector2& v)		{ return b2Vec2{ v.x, v.y }; }
		static Vector2 ToVector2(const b2Vec2& v)	{ return Vector2{ v.x, v.y }; }

		static Vector2 ScreenToWorld(const Vector2& screen) { return screen / m_pixelsPerUnit; }
		static Vector2 WorldToScreen(const Vector2& world)  { return world * m_pixelsPerUnit; }

		static float m_pixelsPerUnit;

		friend class PhysicsBody;

	private:
		void ProcessCollisionEvents();

	private:
		b2WorldId m_worldId;
	};
}
