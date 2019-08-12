#pragma once

#ifndef HW_VECTOR_4_H
#define HW_VECTOR_4_H

#include <utility>
#include "Vector3.h"

namespace Hollow {
	class Vector4
	{
	public:
		float x;
		float y;
		float z;
		float w;
	public:
		Vector4();
		Vector4(float x, float y, float z, float w);
		Vector4(const Vector3& other);

		Vector4(const Vector4&) = default;
		Vector4(Vector4&&) = default;

		Vector4& operator=(const Vector4& other);
		Vector4& operator=(Vector4&& other);

		Vector4 operator*(const float val);
		Vector4 operator*(const double val);
		float operator*(const Vector4& other);

		Vector4 cross(const Vector4& other);
		static Vector4 cross(const Vector4& left, const Vector4& right);

		static float dot(const Vector4& left, const Vector4& right);

		Vector4 operator+(const Vector4& other);
		void operator+=(const Vector4& other);

		Vector4 operator-(const Vector4& other) const;
		void operator-=(const Vector4& other);

		static Vector4 normalize(const Vector4& vector);
		static Vector4 negate(const Vector4& vector);
	};
}

#endif