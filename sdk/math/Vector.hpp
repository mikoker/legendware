#pragma once
#include <sstream>
#include <DirectXMath.h>

class quaternion_t
{
public:
	float x, y, z, w;
};

class Vector //-V690
{
public:
	float x, y, z;

	__inline Vector(void)
	{
		Invalidate();
	}

	Vector(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	Vector(const float* clr)
	{
		x = clr[0];
		y = clr[1];
		z = clr[2];
	}

	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
	{
		x = ix;
		y = iy;
		z = iz;
	}

	void Mul(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}

	Vector Clamp()
	{
		if (x < -89.0f)
			x = -89.0f;

		if (x > 89.0f)
			x = 89.0f;

		while (y < -180.0f)
			y += 360.0f;

		while (y > 180.0f)
			y -= 360.0f;

		z = 0.0f;
		return *this;
	}

	void MulAdd(const Vector& a, const Vector& b, float scalar)
	{
		x = a.x + b.x * scalar;
		y = a.y + b.y * scalar;
		z = a.z + b.z * scalar;
	}

	bool IsValid() const
	{
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
	}

	void Invalidate()
	{
		x = y = z = 0.0f;
	}

	void GetVectors(Vector& right, Vector& up)
	{
		Vector tmp;

		if (x == 0.f && y == 0.f)
		{
			right[0] = 0.f;
			right[1] = -1.f;
			right[2] = 0.f;
			up[0] = -z;
			up[1] = 0.f;
			up[2] = 0.f;
		}
		else
		{
			tmp[0] = 0.f;
			tmp[1] = 0.f;
			tmp[2] = 1.0f;
			right = Cross(tmp);
			up = right.Cross(*this);

			right.Normalize();
			up.Normalize();
		}
	}

	Vector ToEulerAngles()
	{
		auto pitch = 0.0f;
		auto yaw = 0.0f;
		auto roll = 0.0f;

		if (!x && !y)
		{
			if (z > 0.0f)
				pitch = 270.0f;
			else
				pitch = 90.0f;
		}
		else
		{
			pitch = DirectX::XMConvertToDegrees(atan2(-z, Length()));

			if (pitch < 0.0f)
				pitch += 360.0f;

			yaw = DirectX::XMConvertToDegrees(atan2(y, x));

			if (yaw < 0.0f)
				yaw += 360.0f;
		}

		return Vector(pitch, yaw, roll);
	}

	float &operator[](int i) //-V302
	{
		return ((float*)this)[i]; //-V108
	}

	float operator[](int i) const  //-V302
	{
		return ((float*)this)[i]; //-V108
	}

	bool __inline IsZero()
	{
		return x == 0.0f && y == 0.0f && z == 0.0f; //-V550
	}

	void __inline Zero()
	{
		x = y = z = 0.0f;
	}

	bool operator==(const Vector &src) const
	{
		return (src.x == x) && (src.y == y) && (src.z == z); //-V550
	}

	bool operator!=(const Vector &src) const
	{
		return (src.x != x) || (src.y != y) || (src.z != z); //-V550
	}

	Vector &operator+=(const Vector &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	Vector &operator-=(const Vector &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	Vector &operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;

		return *this;
	}

	Vector &operator*=(const Vector &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}

	Vector &operator/=(const Vector &v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;

		return *this;
	}

	Vector &operator+=(float fl)
	{
		x += fl;
		y += fl;
		z += fl;

		return *this;
	}

	Vector &operator/=(float fl)
	{
		x /= fl;
		y /= fl;
		z /= fl;

		return *this;
	}

	Vector &operator-=(float fl)
	{
		x -= fl;
		y -= fl;
		z -= fl;

		return *this;
	}

	void NormalizeInPlace()
	{
		Vector& v = *this;

		float iradius = 1.0f / (this->Length() + FLT_EPSILON);

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;
	}

	Vector Normalized()
	{
		Vector vec(*this);

		float radius = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z) + FLT_EPSILON;

		if (radius != 0.0f) {
			float iradius = 1.f / radius;

			vec.x *= iradius;
			vec.y *= iradius;
			vec.z *= iradius;
		}

		return vec;
	}

	float Normalize()
	{
		float radius = sqrtf(((this->x * this->x) + (this->y * this->y) + (this->z * this->z))) + FLT_EPSILON;

		if (radius != 0.0f) {
			x *= (1.f / radius);
			y *= (1.f / radius);
			z *= (1.f / radius);
		}

		return radius;
	}

	void NormalizeNoClamp()
	{
		this->x -= floorf(this->x / 360.0f + 0.5f) * 360.0f;
		this->y -= floorf(this->y / 360.0f + 0.5f) * 360.0f;
		this->z -= floorf(this->z / 360.0f + 0.5f) * 360.0f;
	}

	float DistTo(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}

	float DistToSqr(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}

	float Dot(const Vector &vOther) const
	{
		return (x  *vOther.x + y * vOther.y + z * vOther.z);
	}

	void VectorCrossProduct(const Vector &a, const Vector &b, Vector &result)
	{
		result.x = a.y * b.z - a.z * b.y;
		result.y = a.z * b.x - a.x * b.z;
		result.z = a.x * b.y - a.y * b.x;
	}

	Vector Cross(const Vector &vOther)
	{
		Vector res;
		VectorCrossProduct(*this, vOther, res);

		return res;
	}

	float Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	float LengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	float Length2D() const
	{
		return sqrt(x * x + y * y);
	}

	float Length2DSqr() const
	{
		return x * x + y * y;
	}

	Vector &operator=(const Vector &vOther)
	{
		x = vOther.x;
		y = vOther.y;
		z = vOther.z;

		return *this;
	}

	Vector operator-(void) const
	{
		return Vector(-x, -y, -z);
	}

	Vector operator+(const Vector &v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}

	Vector operator+(float fl) const
	{
		return Vector(x + fl, y + fl, z + fl);
	}

	Vector operator-(const Vector &v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}

	Vector operator-(float fl) const
	{
		return Vector(x - fl, y - fl, z - fl);
	}

	Vector operator*(float fl) const
	{
		return Vector(x * fl, y * fl, z * fl);
	}

	Vector operator*(const Vector &v) const
	{
		return Vector(x * v.x, y * v.y, z * v.z);
	}

	Vector operator/(float fl) const
	{
		return Vector(x / fl, y / fl, z / fl);
	}

	Vector operator/(const Vector &v) const
	{
		return Vector(x / v.x, y / v.y, z / v.z);
	}

	bool operator < (const Vector& v) {
		return { this->x < v.x&&
				 this->y < v.y&&
				 this->z < v.z };
	}

	bool operator > (const Vector& v) {
		return { this->x > v.x &&
				 this->y > v.y &&
				 this->z > v.z };
	}

	bool operator<=(const Vector& v) {
		return { this->x <= v.x &&
				 this->y <= v.y &&
				 this->z <= v.z };
	}

	bool operator>=(const Vector& v) {
		return { this->x >= v.x &&
				 this->y >= v.y &&
				 this->z >= v.z };
	}
};

inline Vector operator*(float lhs, const Vector &rhs)
{
	return rhs * lhs;
}

inline Vector operator/(float lhs, const Vector &rhs)
{
	return rhs / lhs;
}

class __declspec(align(16)) VectorAligned : public Vector //-V690
{

public:

	inline VectorAligned(void)
	{
		w = 0.0f;
	};

	inline VectorAligned(float X, float Y, float Z)
	{
		w = 0.0f;
		Init(X, Y, Z);
	}

public:

	explicit VectorAligned(const Vector &vOther)
	{
		w = 0.0f;
		Init(vOther.x, vOther.y, vOther.z);
	}

	VectorAligned &operator=(const Vector &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	VectorAligned &operator=(const VectorAligned &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	float w;
};

typedef unsigned __int32 uint32;

inline uint32 const FloatBits(const float &f)
{
	union Convertor_t
	{
		float f;
		uint32 ul;
	} tmp;

	tmp.f = f;
	return tmp.ul;
}

inline bool IsFinite(const float &f)
{
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}

inline void VectorMultiply(const Vector &a, float b, Vector &c)
{
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
}

inline void VectorMA(const Vector &start, float scale, const Vector &direction, Vector &dest)
{
	dest.x = start.x + scale * direction.x;
	dest.y = start.y + scale * direction.y;
	dest.z = start.z + scale * direction.z;
}

inline void VectorAdd(const Vector &a, const Vector &b, Vector &c)
{
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

inline void VectorSubtract(const Vector &a, const Vector &b, Vector &c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}