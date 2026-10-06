#pragma once

namespace nu
{
	struct Rect
	{
		float x, y, w, h;

		Rect() : x{ 0 }, y{ 0 }, w{ 0 }, h{ 0 } {}
		Rect(float x, float y, float w, float h) :
			x{ x },
			y{ y },
			w{ w },
			h{ h }
		{}
	};
}
