#pragma once

#include "Math.h"
#include <cmath>

class Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;
public:
	Quaternion() :
		x(0), y(0), z(0), w(0)
	{}

	Quaternion(float x, float y, float z, float w)
	{
		float rads = Math::toRadians(w);

		w = cosf(rads/2);

		float sin = sinf(rads / 2);
		x = x * sin;
		y = y * sin;
		z = z * sin;
	}

	Quaternion(const Quaternion& other) :
		x(other.x), y(other.y), z(other.z), w(other.w)
	{}

	Quaternion(Quaternion&& other) noexcept :
		x(other.x), y(other.y), z(other.z), w(other.w)
	{}

	Quaternion& operator=(const Quaternion& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;

		return *this;
	}

	Quaternion& operator=(Quaternion&& other) noexcept
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;

		return *this;
	}

	static inline void Interpolate(Quaternion& pOut, const Quaternion& pStart, const Quaternion& pEnd, float pFactor)
	{
		// calc cosine theta
		float cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

		// adjust signs (if necessary)
		Quaternion end = pEnd;
		if (cosom < static_cast<float>(0.0))
		{
			cosom = -cosom;
			end.x = -end.x;   // Reverse all signs
			end.y = -end.y;
			end.z = -end.z;
			end.w = -end.w;
		}

		// Calculate coefficients
		float sclp, sclq;
		if ((static_cast<float>(1.0) - cosom) > static_cast<float>(0.0001)) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			float omega, sinom;
			omega = std::acos(cosom); // extract theta from dot product's cos theta
			sinom = std::sin(omega);
			sclp = std::sin((static_cast<float>(1.0) - pFactor) * omega) / sinom;
			sclq = std::sin(pFactor * omega) / sinom;
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = static_cast<float>(1.0) - pFactor;
			sclq = pFactor;
		}

		pOut.x = sclp * pStart.x + sclq * end.x;
		pOut.y = sclp * pStart.y + sclq * end.y;
		pOut.z = sclp * pStart.z + sclq * end.z;
		pOut.w = sclp * pStart.w + sclq * end.w;
	}
};