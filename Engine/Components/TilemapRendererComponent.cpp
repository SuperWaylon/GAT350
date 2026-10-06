#include "pch.h"
#include "TilemapRendererComponent.h"
#include "Renderer/Tilemap.h"
#include "Core/Factory.h"
#include "Physics/PhysicsBody.h"
#include "Framework/Actor.h"
#include "Engine.h"

namespace nu
{
	FACTORY_REGISTER(TilemapRendererComponent)

	TilemapRendererComponent::TilemapRendererComponent(const TilemapRendererComponent& other)
	{
		// copy the tilemap name
		m_tilemapName = other.m_tilemapName;
	}

	void TilemapRendererComponent::Start()
	{
		// load the tilemap resource
		m_tilemap = Resources().Get<Tilemap>(m_tilemapName, Engine::Instance().GetRenderer());
		if (!m_tilemap)
		{
			std::cerr << "Could not load tilename " << m_tilemapName << std::endl;
			return;
		}

		// set tile transform current owner (actor) transform
		Transform transform = GetOwner()->GetTransform();

		// define physics bodies
		PhysicsBody::PhysicsBodyDef bodyDef;
		bodyDef.isDynamic = false;	// doesn't move (not dynamic)
		bodyDef.actor = GetOwner();	// set actor for collision

		// iterate through layers
		for (auto layer : m_tilemap->GetLayers())
		{
			// if layer doesn't have collision, skip physics body creation
			if (!layer.hasCollision) continue;

			// get tilemap data (tile ids)
			for (int i = 0; i < layer.data.size(); i++)
			{
				int tileId = layer.data[i];
				if (tileId == 0) continue;

				// set source rect from id
				Rect sourceRect = m_tilemap->GetTileRect(layer, tileId);
				// set position from owner transform position + tile position * owner transform scale
				transform.position = GetOwner()->GetTransform().position + (m_tilemap->GetTilePosition(layer, i) * transform.scale);

				// size of physics body is tehe source rect
				Vector2 size{ sourceRect.w, sourceRect.h };

				// create physics body and add to physics bodies container
				auto physicsBody = std::make_unique<PhysicsBody>(transform, size, bodyDef, Engine::Instance().GetPhysics());
				m_physicsBodies.push_back(std::move(physicsBody));
			}
		}
	}

	void TilemapRendererComponent::Draw(const Renderer& renderer)
	{
		// iterate through layers
		for (auto layer : m_tilemap->GetLayers())
		{
			// get layer data (array of tile ids)
			for (int i = 0; i < layer.data.size(); i++)
			{
				int tileId = layer.data[i];

				if (tileId == 0) continue; // don't draw if tile ids = 0

				// get source rect of tile id
				Rect sourceRect = m_tilemap->GetTileRect(layer, tileId);

				// set position from owner (actor) transform position + tile position * transform scale
				Transform transform = GetOwner()->GetTransform();
				Vector2 position = transform.position + m_tilemap->GetTilePosition(layer, i) * transform.scale;

				// draw tile texture
				renderer.DrawTexture(*layer.texture, sourceRect, position.x, position.y, transform.rotation, transform.scale);
			}
		}
	}

	void TilemapRendererComponent::Read(const json::value_t& value)
	{
		RendererComponent::Read(value);

		JSON_READ_NAME(value, "tilemap_name", m_tilemapName);
	}
}