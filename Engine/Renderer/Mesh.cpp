#include "pch.h"
#include "Mesh.h"

namespace nu
{
    float Mesh::GetRadius() const
    {
        float radius = 0.0f;

        for (const auto& point : m_points)
        {
            float length = point.Length();
            if (length > radius)
                radius = length;
        }

        return radius;
    }

}