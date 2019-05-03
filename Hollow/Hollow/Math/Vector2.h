#pragma once

#ifndef HW_VECTOR_2_H
#define HW_VECTOR_2_H

class Vector2
{
public:
	float x;
	float y;
public:
	Vector2() :
		x(0.0f), y(0.0f)
	{}

	Vector2(float x, float y) :
		x(x), y(y)
	{}

	Vector2(const Vector2&) = default;
	Vector2(Vector2&&) = default;

	Vector2& operator=(const Vector2&) = default;
	Vector2& operator=(Vector2&&) = default;
};

#endif