#include "pch.h"
#include "Tilemap.h"
#include "Texture.h"
#include "Renderer.h"
#include "Serialization/Json.h"
#include "Core/StringUtils.h"

namespace nu
{
	bool Tilemap::Load(const std::string& filename, class Renderer& renderer)
	{
		// load json
		json::document_t document;
		if (!json::Load(filename, document))
		{
			std::cerr << "Could not load Tilemap " << filename << std::endl;
			return false;
		}

		// read tilemap data
		JSON_READ_NAME_REQ(document, "tilewidth", m_tileWidth);
		JSON_READ_NAME_REQ(document, "tileheight", m_tileHeight);

		// read tilemap layers
		if (JSON_HAS_NAME(document, "layers"))
		{
			for (auto& layerValue : JSON_GET_NAME(document, "layers").GetArray())
			{
				Layer layer;

				JSON_READ_NAME_REQ(layerValue, "height", layer.height);
				JSON_READ_NAME_REQ(layerValue, "width", layer.width);
				JSON_READ_NAME_REQ(layerValue, "data", layer.data);

				// load texture from properties
				if (JSON_HAS_NAME(layerValue, "properties"))
				{
					for (auto& propertyValue : JSON_GET(layerValue, properties).GetArray())
					{
						std::string name;
						JSON_READ(propertyValue, name);

						// check if property name is TextureName
						if (EqualsIgnoreCase(name, "texture_name"))
						{
							// read texture name value (texture name)
							std::string value;
							JSON_READ(propertyValue, value);

							layer.texture = Resources().Get<Texture>(value, renderer);
							if (!layer.texture)
							{
								std::cerr << "Could not read tilemap layer texure " << value << std::endl;
							}
						}
						else if (EqualsIgnoreCase(name, "has_collision"))
						{
							JSON_READ_NAME(propertyValue, "value", layer.hasCollision);
						}
					}
				}

				m_layers.push_back(layer);
			}
		}

		return true;
	}

	// get texture source rectangle from the tile id
	Rect Tilemap::GetTileRect(const Layer& layer, int tileId)
	{
		// tile id 0 is blank tile, return empty rect
		if (tileId == 0) return Rect();

		Vector2 textureSize = layer.texture->GetSize();
		// get the number of tiles in a row (texture size / tile width size)
		int tilesPerRow = (int)(textureSize.x / m_tileWidth);

		// get the column / row of the tile
		int column = (tileId - 1) % tilesPerRow;
		int row = (tileId - 1) / tilesPerRow;

		// return source rectangle of tile in texture
		return Rect
		{
			(float)(column * m_tileWidth),
			(float)(row * m_tileHeight),
			(float)(m_tileWidth),
			(float)(m_tileHeight)
		};
	}

	// get the tile position from the tile index (index into tile data)
	Vector2 Tilemap::GetTilePosition(const Layer& layer, int tileIndex)
	{
		int column = tileIndex % layer.width;
		int row = tileIndex / layer.width;

		return Vector2{ (float)(column * m_tileWidth), (float)(row * m_tileHeight) };
	}
}