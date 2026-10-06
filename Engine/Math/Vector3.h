#pragma once
#include <cmath>
#include <cassert>

namespace nu
{
	struct Vector3
	{
		union
		{
			struct { float x, y, z; };
			struct { float r, g, b; };
		};

		//float x, y, z;

		Vector3() = default;
		Vector3(float x, float y, float z) : x{ x }, y{ y }, z{ z } {}
		explicit Vector3(float v) : x{ v }, y{ v }, z{ v } {}

		float  operator [] (unsigned int i) const { assert(i < 3); return (&x)[i]; }
		float& operator [] (unsigned int i) { assert(i < 3); return (&x)[i]; }

		bool operator == (const Vector3& v) const { return (this->x == v.x && this->y == v.y && this->z == v.z); }
		bool operator != (const Vector3& v) const { return (this->x != v.x || this->y != v.y || this->z != v.z); }

		Vector3 operator + (const Vector3& v) const { return Vector3{ this->x + v.x, this->y + v.y, this->z + v.z }; }
		Vector3 operator - (const Vector3& v) const { return Vector3{ this->x - v.x, this->y - v.y, this->z - v.z }; }
		Vector3 operator * (const Vector3& v) const { return Vector3{ this->x * v.x, this->y * v.y, this->z * v.z }; }
		Vector3 operator / (const Vector3& v) const { return Vector3{ this->x / v.x, this->y / v.y, this->z / v.z }; }

		Vector3 operator + (float v) const { return Vector3{ this->x + v, this->y + v, this->z + v }; }
		Vector3 operator - (float v) const { return Vector3{ this->x - v, this->y - v, this->z - v }; }
		Vector3 operator * (float v) const { return Vector3{ this->x * v, this->y * v, this->z * v }; }
		Vector3 operator / (float v) const { return Vector3{ this->x / v, this->y / v, this->z / v }; }

		Vector3& operator += (const Vector3& v) { this->x += v.x; this->y += v.y;  this->z += v.z; return *this; }
		Vector3& operator -= (const Vector3& v) { this->x -= v.x; this->y -= v.y;  this->z -= v.z; return *this; }
		Vector3& operator *= (const Vector3& v) { this->x *= v.x; this->y *= v.y;  this->z *= v.z; return *this; }
		Vector3& operator /= (const Vector3& v) { this->x /= v.x; this->y /= v.y;  this->z /= v.z; return *this; }

		Vector3& operator += (float v) { this->x += v; this->y += v;  this->z += v; return *this; }
		Vector3& operator -= (float v) { this->x -= v; this->y -= v;  this->z -= v; return *this; }
		Vector3& operator *= (float v) { this->x *= v; this->y *= v;  this->z *= v; return *this; }
		Vector3& operator /= (float v) { this->x /= v; this->y /= v;  this->z /= v; return *this; }

		float LengthSqr() const { return (x * x) + (y * y) + (z * z); }
		float Length() const { return  std::sqrt(LengthSqr()); }
		Vector3 Normalized() const { return (*this) / Length(); }
		float Dot(const Vector3& v) const { return (this->x * v.x) + (this->y * v.y) + (this->z * v.z); }

		static float Dot(const Vector3& v1, const Vector3& v2) { return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z); }
	};

	using Color = Vector3;
}

