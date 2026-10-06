#include "pch.h"
#include "Model.h"

namespace nu
{
	void Model::CalculateRadius()
	{
		m_radius = 0.0f;

		// check for mesh radius
		for (const auto& mesh : m_meshes)
		{
			// get mesh radius
			float radius = mesh.GetRadius();
			// if radius is larger than current radius, radius is new radius
			if (radius > m_radius)
				m_radius = radius;
		}
	}
}