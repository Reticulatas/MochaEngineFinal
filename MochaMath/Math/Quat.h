/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */


#pragma once
#include "Vector4.h"
#include <cmath>
#define EPSILON  (0.00001f)
using namespace Math;

class Quat
{
	friend Quat operator*(float lhs, const Quat &rhs); // this was causing linker errors
public:

	union
	{
		struct { float x, y, z, w; };
		float data[4];
	};

	__forceinline Quat &SetFromRotation(float radians, float axisX, float axisY, float axisZ)
	{
		if (axisX * axisX + axisY * axisY + axisZ * axisZ > EPSILON)
		{
			float s = std::sinf(0.5f * radians);
			float len_inv = 1.0f / std::sqrtf(axisX * axisX + axisY * axisY + axisZ * axisZ);
			x = s * axisX * len_inv;
			y = s * axisY * len_inv;
			z = s * axisZ * len_inv;
			w = std::cosf(0.5f * radians);
		}
		else
		{
			x = y = z = 0.0f;
			w = 1.0f;
		}
		return *this;
	}

	__forceinline Quat &SetFromRotation(float radians, Vector4 axis)
	{
		axis.Normalize();
		return SetFromRotation(radians, axis.x, axis.y, axis.z);
	}

	__forceinline Quat &SetFromRotation(float radians, Math::Vector3 axis)
	{
		return SetFromRotation(radians, axis.x, axis.y, axis.z);
	}

	__forceinline Quat &SetFromRotation(const Vector4 &from, const Vector4 &to)
	{
		if (Equal(from.Dot(to), 1.0f)) return *this;
		return SetFromRotation(std::acosf(from.Normalized().Dot(to.Normalized())), from.Cross(to));
	}

	__forceinline Quat &SetFromVector(const Math::Vector3 &v)
	{
		return SetFromRotation(v.Length(), v);
	}

	__forceinline Quat &SetFromVector(const Vector4 &v)
	{
		return SetFromRotation(v.Length(), v);
	}

	Quat &SetFromMatrix(const Math::Matrix3 &m)
	{
		float trace = m.m00 + m.m11 + m.m22;
		if (trace > 0.0f)
		{
			float s = 0.5f / std::sqrtf(trace + 1.0f);
			x = (m.m21 - m.m12) * s;
			y = (m.m02 - m.m20) * s;
			z = (m.m10 - m.m01) * s;
			w = 0.25f / s;
		}
		else if (m.m00 > m.m11 && m.m00 > m.m22)
		{
			float s = 2.0f * std::sqrtf(1.0f + m.m00 - m.m11 - m.m22);
			x = 0.25f * s;
			y = (m.m01 + m.m10) / s;
			z = (m.m02 + m.m20) / s;
			w = (m.m21 - m.m12) / s;
		}
		else if (m.m11 > m.m22)
		{
			float s = 2.0f * std::sqrtf(1.0f + m.m11 - m.m00 - m.m22);
			x = (m.m01 + m.m10) / s;
			y = 0.25f * s;
			z = (m.m12 + m.m21) / s;
			w = (m.m02 - m.m20) / s;
		}
		else
		{
			float s = 2.0f * sqrt(1.0f + m.m22 - m.m00 - m.m11);
			x = (m.m02 + m.m20) / s;
			y = (m.m12 + m.m21) / s;
			z = 0.25f * s;
			w = (m.m10 - m.m01) / s;
		}

		return *this;
	}

	Quat &SetFromMatrix(const Matrix4 &m)
	{
		float trace = m.m00 + m.m11 + m.m22;
		if (trace > 0.0f)
		{
			float s = 0.5f / std::sqrtf(trace + 1.0f);
			x = (m.m21 - m.m12) * s;
			y = (m.m02 - m.m20) * s;
			z = (m.m10 - m.m01) * s;
			w = 0.25f / s;
		}
		else if (m.m00 > m.m11 && m.m00 > m.m22)
		{
			float s = 2.0f * std::sqrtf(1.0f + m.m00 - m.m11 - m.m22);
			x = 0.25f * s;
			y = (m.m01 + m.m10) / s;
			z = (m.m02 + m.m20) / s;
			w = (m.m21 - m.m12) / s;
		}
		else if (m.m11 > m.m22)
		{
			float s = 2.0f * std::sqrtf(1.0f + m.m11 - m.m00 - m.m22);
			x = (m.m01 + m.m10) / s;
			y = 0.25f * s;
			z = (m.m12 + m.m21) / s;
			w = (m.m02 - m.m20) / s;
		}
		else
		{
			float s = 2.0f * sqrt(1.0f + m.m22 - m.m00 - m.m11);
			x = (m.m02 + m.m20) / s;
			y = (m.m12 + m.m21) / s;
			z = 0.25f * s;
			w = (m.m10 - m.m01) / s;
		}

		return *this;
	}

	__forceinline Quat(float xComp = 0.0f, float yComp = 0.0f, float zComp = 0.0f, float wComp = 1.0f)
		: w(wComp), x(xComp), y(yComp), z(zComp)
	{ }

	__forceinline Quat(float radians, const Vector4 &axis)
	{
		SetFromRotation(radians, axis);
	}

	__forceinline Quat(float radians, const Math::Vector3 &axis)
	{
		SetFromRotation(radians, axis);
	}

	__forceinline Quat(const Vector4 &from, const Vector4 &to)
	{
		SetFromRotation(from, to);
	}

	__forceinline explicit Quat(const Vector4 &v)
	{
		SetFromVector(v);
	}

	__forceinline explicit Quat(const Math::Vector3 &v)
	{
		SetFromVector(v);
	}

	__forceinline explicit Quat(const Matrix4 &m)
	{
		SetFromMatrix(m);
	}

	bool IsUnit(void) const
	{
		return Equal(1.0f, w * w + x * x + y * y + z * z) || Equal(0.0f, w * w + x * x + y * y + z * z);
	}

	Math::Matrix3 &ToMatrix(Math::Matrix3 &target) const
	{
		//GV_ASSERT_MESSAGE(IsUnit(), "Quaternion::ToMatrix requires unit quaternions.");

		float s, sx, sy, sz, wx, wy, wz, xx, xy, xz, yy, yz, zz;
		s = 2.0f; //would be (2.0f / length) if not normalized
		sx = s *  x; sy = s *  y; sz = s *  z;
		wx = w * sx; wy = w * sy; wz = w * sz;
		xx = x * sx; xy = x * sy; xz = x * sz;
		yy = y * sy; yz = y * sz; zz = z * sz;

		target = target.SetIdentity();

		target.m00 = 1.0f - (yy + zz);
		target.m01 = xy - wz;
		target.m02 = xz + wy;

		target.m10 = xy + wz;
		target.m11 = 1.0f - (xx + zz);
		target.m12 = yz - wx;

		target.m20 = xz - wy;
		target.m21 = yz + wx;
		target.m22 = 1.0f - (xx + yy);

		return target;
	}

	Matrix4 &ToMatrix(Matrix4 &target) const
	{
		//GV_ASSERT_MESSAGE(IsUnit(), "Quaternion::ToMatrix requires unit quaternions.");

		float s, sx, sy, sz, wx, wy, wz, xx, xy, xz, yy, yz, zz;
		s = 2.0f; //would be (2.0f / length) if not normalized
		sx = s *  x; sy = s *  y; sz = s *  z;
		wx = w * sx; wy = w * sy; wz = w * sz;
		xx = x * sx; xy = x * sy; xz = x * sz;
		yy = y * sy; yz = y * sz; zz = z * sz;

		target.SetIdentity();

		target.m00 = 1.0f - (yy + zz);
		target.m01 = xy - wz;
		target.m02 = xz + wy;

		target.m10 = xy + wz;
		target.m11 = 1.0f - (xx + zz);
		target.m12 = yz - wx;

		target.m20 = xz - wy;
		target.m21 = yz + wx;
		target.m22 = 1.0f - (xx + yy);

		return target;
	}

	__forceinline const Matrix4 ToMatrix(void)
	{
		return ToMatrix(Matrix4());
	}

	__forceinline float Angle(void) const
	{
		return 2.0f * std::acosf(Clamp(w, -1.0f, 1.0f));
	}

	__forceinline Quat &Pow(float power)
	{
		return SetFromRotation(Angle() * power, x, y, z);
	}
	__forceinline const Quat Powed(float power)
	{
		return Quat(*this).Pow(power);
	}

	const Vector4 Rotate(const Vector4 &p) const
	{
		//GV_ASSERT_MESSAGE(IsUnit(), "Quaternion::Rotate(const Vector4 v) requires unit quaternions.");
		Vector4 v(x, y, z);
		return
			p * (2.0f * w * w - 1.0f)
			+ v * 2.0f * (v.Dot(p))
			+ (v.Cross(p) * 2.0f * w);
	}

	const Math::Vector3 Rotate(const Math::Vector3 &p) const
	{
		//GV_ASSERT_MESSAGE(IsUnit(), "Quaternion::Rotate(const Vector4 v) requires unit quaternions.");
		Math::Vector3 v(x, y, z);
		return
			p * (2.0f * w * w - 1.0f)
			+ 2.0f * (v.Dot(p)) * v
			+ 2.0f * w * (v.Cross(p));
	}

	__forceinline Quat operator-(void) const
	{
		return Quat(-x, -y, -z, w);
	}

	Quat &operator*=(const Quat &rhs)
	{
		Vector4 vThis(x, y, z);
		Vector4 vRhs(rhs.x, rhs.y, rhs.z);
		Vector4 newV = vThis * rhs.w + vRhs * w + vThis.Cross(vRhs);

		x = newV.x;
		y = newV.y;
		z = newV.z;
		w = w * rhs.w - vThis.Dot(vRhs);

		return *this;
	}

	__forceinline Quat &operator*(const Quat &rhs) const
	{
		return Quat(*this) *= rhs;
	}

	__forceinline Quat &operator*=(float rhs)
	{
		x *= rhs; y *= rhs; z *= rhs; w *= rhs;
		return *this;
	}

	__forceinline Quat operator*(float rhs) const
	{
		return Quat(*this) *= rhs;
	}

	__forceinline Quat &Normalize(void)
	{
		float len = std::sqrtf(x * x + y * y + z * z + w * w);
		if (len == 0.0f)
			return *this;

		len = 1.0f / len;

		x *= len;
		y *= len;
		z *= len;
		w *= len;

		return *this;
	}
	__forceinline const Quat Normalized(void) const
	{
		return Quat(*this).Normalize();
	}

	};

	__forceinline Quat operator*(float lhs, const Quat &rhs) // this was causing linker errors
	{
		return rhs * lhs;
	}
  

