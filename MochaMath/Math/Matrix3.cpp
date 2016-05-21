///////////////////////////////////////////////////////////////////////////////
///
/// \file Matrix3.cpp
/// Implementation of the Matrix 3 structure.
/// 
/// Authors: Joshua Davis, Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////

#include "MathUtilities.h"
#include "Matrix3.h"
#include "MathFunctions.h"
#include "Quat.h"
#include <cmath>

namespace Math
{

	const Matrix3 Matrix3::cIdentity(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f);

	Matrix3::Matrix3(float p00, float p01, float p02,
		float p10, float p11, float p12,
		float p20, float p21, float p22)
	{
		m00 = p00;  m01 = p01;  m02 = p02;
		m10 = p10;  m11 = p11;  m12 = p12;
		m20 = p20;  m21 = p21;  m22 = p22;
	}

	Matrix3::Matrix3(ConstRealPointer data_)
	{
		m00 = data_[0]; m01 = data_[1]; m02 = data_[2];
		m10 = data_[3]; m11 = data_[4]; m12 = data_[5];
		m20 = data_[6]; m21 = data_[7]; m22 = data_[8];
	}

  Matrix3::Matrix3(void)
  {
    SetIdentity();
  }

	float* Matrix3::ToFloats()
	{
		return (float*)this;
	}

	//------------------------------------------ Binary Assignment Operators (reals)

	void Matrix3::operator*=(float rhs)
	{
		Matrix3& self = *this;
		self[0] *= rhs;
		self[1] *= rhs;
		self[2] *= rhs;
	}

	void Matrix3::operator/=(float rhs)
	{
		Matrix3& self = *this;
		if (Math::IsZero(rhs))
			ErrorIf(Math::IsZero(rhs), "Matrix3 - Division by zero.");
		self[0] /= rhs;
		self[1] /= rhs;
		self[2] /= rhs;
	}

	//----------------------------------------------------- Binary Operators (reals)

	Matrix3 Matrix3::operator*(float rhs) const
	{
		Matrix3 ret = *this;
		ret *= rhs;
		return ret;
	}

	Matrix3 Matrix3::operator/(float rhs) const
	{
		if (Math::IsZero(rhs))
			ErrorIf(Math::IsZero(rhs), "Matrix3 - Division by zero.");
		Matrix3 ret = *this;
		ret /= rhs;
		return ret;
	}

	//---------------------------------------- Binary Assignment Operator (Matrices)

	void Matrix3::operator+=(Mat3Param rhs)
	{
		Matrix3& self = *this;
		self[0] += rhs[0];
		self[1] += rhs[1];
		self[2] += rhs[2];
	}

	void Matrix3::operator-=(Mat3Param rhs)
	{
		Matrix3& self = *this;
		self[0] -= rhs[0];
		self[1] -= rhs[1];
		self[2] -= rhs[2];
	}

	//-------------------------------------------------- Binary Operators (Matrices)

	Matrix3 Matrix3::operator+(Mat3Param rhs) const
	{
		Matrix3 ret = *this;
		ret += rhs;
		return ret;
	}

	Matrix3 Matrix3::operator-(Mat3Param rhs) const
	{
		Matrix3 ret = *this;
		ret -= rhs;
		return ret;
	}

	Matrix3 Matrix3::operator*(Mat3Param rhs) const
	{
		return Concat(rhs);
	}

	Math::Vector3 Matrix3::operator*(Vector3 rhs) const
	{
		return Transform(*this, rhs);
	}

	//----------------------------------------------------------- Binary Comparisons

	bool Matrix3::operator==(Mat3Param rhs) const
	{
		const Matrix3& self = *this;
		return self[0] == rhs[0] &&
			self[1] == rhs[1] &&
			self[2] == rhs[2];
	}

	bool Matrix3::operator!=(Mat3Param rhs) const
	{
		return !(*this == rhs);
	}

	float Matrix3::operator()(unsigned r, unsigned c) const
	{
		if (r > 2)
			ErrorIf(r > 2, "Matrix3 - Index out of range.");
		if (c > 2)
			ErrorIf(c > 2, "Matrix3 - Index out of range.");

#ifdef ColumnBasis
		return array[c + r * 3];
#else
		return array[r + c * 3];
#endif
	}

	float& Matrix3::operator()(unsigned r, unsigned c)
	{
		if (r > 2)
			ErrorIf(r > 2, "Matrix3 - Index out of range.");
		if (c > 2)
			ErrorIf(c > 2, "Matrix3 - Index out of range.");


#ifdef ColumnBasis
		return array[c + r * 3];
#else
		return array[r + c * 3];
#endif
	}

	Vector3& Matrix3::operator[](unsigned index)
	{
		return ((Vector3*)this)[index];
	}

	const Vector3& Matrix3::operator[](unsigned index) const
	{
		return ((Vector3*)this)[index];
	}

	Matrix3 Matrix3::Transposed(void) const
	{
		Matrix3 ret;

		ret.m00 = m00;
		ret.m01 = m10;
		ret.m02 = m20;
		ret.m10 = m01;
		ret.m11 = m11;
		ret.m12 = m21;
		ret.m20 = m02;
		ret.m21 = m12;
		ret.m22 = m22;

		return ret;
	}

	Mat3Ref Matrix3::Transpose(void)
	{
		Math::Swap(m01, m10);
		Math::Swap(m02, m20);
		Math::Swap(m12, m21);
		return *this;
	}

	Matrix3 Matrix3::Inverted(void) const
	{
		Matrix3 foo(*this);
		foo.Invert();
		return foo;
	}

	Mat3Ref Matrix3::Invert(void)
	{
		float determinant = Determinant();
		if (determinant == 0.0f)
			ErrorIf(determinant == 0.0f, "Matrix3 - Uninvertible matrix.");
		determinant = 1.0f / determinant;
		float t00 = (m11 * m22 - m12 * m21) * determinant;
		float t01 = (m02 * m21 - m01 * m22) * determinant;
		float t02 = (m01 * m12 - m02 * m11) * determinant;
		float t10 = (m12 * m20 - m10 * m22) * determinant;
		float t11 = (m00 * m22 - m02 * m20) * determinant;
		float t12 = (m02 * m10 - m00 * m12) * determinant;
		float t20 = (m10 * m21 - m11 * m20) * determinant;
		float t21 = (m01 * m20 - m00 * m21) * determinant;
		float t22 = (m00 * m11 - m01 * m10) * determinant;

		m00 = t00;  m01 = t01;  m02 = t02;
		m10 = t10;  m11 = t11;  m12 = t12;
		m20 = t20;  m21 = t21;  m22 = t22;
		return *this;
	}

	Matrix3 Matrix3::Concat(Mat3Param rhs) const
	{
		Matrix3 ret;

		ret.m00 = Dot(Cross(0), rhs.Basis(0));
		ret.m01 = Dot(Cross(0), rhs.Basis(1));
		ret.m02 = Dot(Cross(0), rhs.Basis(2));

		ret.m10 = Dot(Cross(1), rhs.Basis(0));
		ret.m11 = Dot(Cross(1), rhs.Basis(1));
		ret.m12 = Dot(Cross(1), rhs.Basis(2));

		ret.m20 = Dot(Cross(2), rhs.Basis(0));
		ret.m21 = Dot(Cross(2), rhs.Basis(1));
		ret.m22 = Dot(Cross(2), rhs.Basis(2));

		return ret;
	}

	Mat3Ref Matrix3::SetIdentity(void)
	{
		Matrix3& self = *this;
		self[0].Set(1.0f, 0.0f, 0.0f);
		self[1].Set(0.0f, 1.0f, 0.0f);
		self[2].Set(0.0f, 0.0f, 1.0f);
		return *this;
	}

	Mat3Ref Matrix3::ZeroOut(void)
	{
		Matrix3& self = *this;
		self[0].ZeroOut();
		self[1].ZeroOut();
		self[2].ZeroOut();
		return *this;
	}

	float Matrix3::Determinant(void) const
	{
		return (m00 * m11 * m22 + m10 * m21 * m02 + m01 * m12 * m20)
			- (m02 * m11 * m20 + m10 * m01 * m22 + m00 * m21 * m12);
	}

	bool Matrix3::Valid(void) const
	{
		const Matrix3& self = *this;
		return self[0].Valid() && self[1].Valid() && self[2].Valid();
	}

	void Matrix3::Scale(float x, float y, float z)
	{
		SetIdentity();
		m00 = x;
		m11 = y;
		m22 = z;
	}

	void Matrix3::Scale(Vec3Param rhs)
	{
		Scale(rhs[0], rhs[1], rhs[2]);
	}

	void Matrix3::Rotate(float x, float y, float z, float radian)
	{
		float c0 = Math::Cos(radian);
		float n1C0 = 1.0f - c0;
		float s0 = Math::Sin(radian);

		//| x^2(1-c0)+c0  xy(1-c0)-zs0  xz(1-c0)+ys0 |
		//| xy(1-c0)+zs0  y^2(1-c0)+c0  yz(1-c0)-xs0 |
		//| xz(1-c0)-ys0  yz(1-c0)+xs0  z^2(1-c0)+c0 |
		m00 = x * x * n1C0 + c0;
		m01 = x * y * n1C0 - z * s0;
		m02 = x * z * n1C0 + y * s0;

		m10 = x * y * n1C0 + z * s0;
		m11 = y * y * n1C0 + c0;
		m12 = y * z * n1C0 - x * s0;

		m20 = x * z * n1C0 - y * s0;
		m21 = y * z * n1C0 + x * s0;
		m22 = z * z * n1C0 + c0;
	}

	void Matrix3::Rotate(Vec3Param rhs, float radian)
	{
		Rotate(rhs.x, rhs.y, rhs.z, radian);
	}

	void Matrix3::Translate(float x, float y)
	{
		m00 = 1.0f;  m01 = 0.0f;  m02 = x;
		m10 = 0.0f;  m11 = 1.0f;  m12 = y;
		m20 = 0.0f;  m21 = 0.0f;  m22 = 1.0f;
	}

	void Matrix3::Translate(Vec2Param rhs)
	{
		Translate(rhs.x, rhs.y);
	}

	void Matrix3::BuildTransform(Vec2Param translate, float radians, Vec2Param scale)
	{
		//Translation
		m02 = translate.x;
		m12 = translate.y;
		m22 = 1.0f;

		//Rotation
		m00 = Math::Cos(radians);
		m01 = -Math::Sin(radians);
		m10 = -m01;
		m11 = m00;

		//Scale
		m00 *= scale.x;
		m10 *= scale.x;
		m01 *= scale.y;
		m11 *= scale.y;

		m20 = m21 = 0.0f;
	}

	void Matrix3::BuildTransform(QuatParam rotate, Vec3Param scale)
	{
		//Rotational component
		float xx = rotate.x * rotate.x;
		float xy = rotate.x * rotate.y;
		float xz = rotate.x * rotate.z;
		float yy = rotate.y * rotate.y;
		float yz = rotate.y * rotate.z;
		float zz = rotate.z * rotate.z;
		float zw = rotate.z * rotate.w;
		float yw = rotate.y * rotate.w;
		float xw = rotate.x * rotate.w;

		m00 = 1.0f - 2.0f * (yy + zz);
		m01 = 2.0f * (xy - zw);
		m02 = 2.0f * (xz + yw);

		m10 = 2.0f * (xy + zw);
		m11 = 1.0f - 2.0f * (xx + zz);
		m12 = 2.0f * (yz - xw);

		m20 = 2.0f * (xz - yw);
		m21 = 2.0f * (yz + xw);
		m22 = 1.0f - 2.0f * (xx + yy);

		//Scale component
		m00 *= scale.x;
		m10 *= scale.x;
		m20 *= scale.x;

		m01 *= scale.y;
		m11 *= scale.y;
		m21 *= scale.y;

		m02 *= scale.z;
		m12 *= scale.z;
		m22 *= scale.z;
	}

	Mat3Ref Matrix3::Orthonormalize(void)
	{
    //return Quat().SetFromMatrix(*this).Normalize().ToMatrix(*this);
    return ToMatrix3(ToQuaternion(*this).Normalized());
	}

	Matrix3::BasisVector Matrix3::Basis(unsigned index) const
	{
		const Matrix3& self = *this;
		if (index > 2)
			ErrorIf(index > 2, "Matrix3 - Index out of range.");
#ifdef ColumnBasis
		return Vector3(array[index], array[3 + index], array[6 + index]);
#else
		return self[index];
#endif
	}

	Matrix3::BasisVector Matrix3::BasisX(void) const
	{
		const Matrix3& self = *this;
#ifdef ColumnBasis
		return Vector3(array[0], array[3], array[6]);
#else
		return self[0];
#endif
	}

	Matrix3::BasisVector Matrix3::BasisY(void) const
	{
		const Matrix3& self = *this;
#ifdef ColumnBasis
		return Vector3(array[1], array[4], array[7]);
#else
		return self[1];
#endif
	}

	Matrix3::BasisVector Matrix3::BasisZ(void) const
	{
		const Matrix3& self = *this;
#ifdef ColumnBasis
		return Vector3(array[2], array[5], array[8]);
#else
		return self[2];
#endif
	}

	Matrix3::CrossVector Matrix3::Cross(unsigned index) const
	{
		const Matrix3& self = *this;
		if (index > 2)
			ErrorIf(index > 2, "Matrix3 - Index out of range.");
#ifdef ColumnBasis
		return self[index];
#else
		return Vector3(array[index], array[3 + index], array[6 + index]);
#endif
	}

	void Matrix3::SetBasis(unsigned index, Vec3Param basisVector)
	{
		SetBasis(index, basisVector[0], basisVector[1], basisVector[2]);
	}

	void Matrix3::SetBasis(unsigned index, float x, float y, float z)
	{
		Matrix3& self = *this;
		if (index > 2)
			ErrorIf(index > 2, "Matrix3 - Index out of range.");
#ifdef ColumnBasis
		array[index] = x;
		array[3 + index] = y;
		array[6 + index] = z;
#else
		self[index].Set(x, y, z);
#endif
	}

	void Matrix3::SetCross(unsigned index, Vec3Param crossVector)
	{
		SetCross(index, crossVector[0], crossVector[1], crossVector[2]);
	}

	void Matrix3::SetCross(unsigned index, float x, float y, float z)
	{
		Matrix3& self = *this;
		if (index > 2)
			ErrorIf(index > 2, "Matrix3 - Index out of range.");
#ifdef ColumnBasis
		self[index].Set(x, y, z);
#else
		array[index] = x;
		array[3 + index] = y;
		array[6 + index] = z;
#endif
	}

	Matrix3 operator*(float lhs, Mat3Param rhs)
	{
		return rhs * lhs;
	}

	Matrix3 Concat(Mat3Param lhs, Mat3Param rhs)
	{
		return lhs.Concat(rhs);
	}

	Matrix3 BuildTransform(Vec2Param translate, float radians, Vec2Param scale)
	{
		Matrix3 matrix;
		matrix.BuildTransform(translate, radians, scale);
		return matrix;
	}

	Matrix3 BuildTransform(QuatParam rotate, Vec3Param scale)
	{
		Matrix3 matrix;
		matrix.BuildTransform(rotate, scale);
		return matrix;
	}

	Vector3 Transform(Mat3Param matrix, Vec3Param vector)
	{
		float x = Dot(matrix.Cross(0), vector);
		float y = Dot(matrix.Cross(1), vector);
		float z = Dot(matrix.Cross(2), vector);
		return Vector3(x, y, z);
	}

	void Transform(Mat3Param matrix, Vec3Ptr vector)
	{
		if (vector == NULL)
			ErrorIf(vector == NULL, "Matrix3 - Null pointer passed for vector.");
		float x = Dot(matrix.Cross(0), *vector);
		float y = Dot(matrix.Cross(1), *vector);
		float z = Dot(matrix.Cross(2), *vector);
		vector->Set(x, y, z);
	}

	Vector2 TransformPoint(Mat3Param matrix, Vec2Param vector)
	{
		float x = Dot(*(Vector2*)&matrix[0], vector) + matrix[0][2];
		float y = Dot(*(Vector2*)&matrix[1], vector) + matrix[1][2];
		return Vector2(x, y);
	}

	Vector2 TransformNormal(Mat3Param matrix, Vec2Param normal)
	{
		float x = Dot(*(Vector2*)&matrix[0], normal);
		float y = Dot(*(Vector2*)&matrix[1], normal);
		return Vector2(x, y);
	}

	Vector3 TransposedTransform(Mat3Param matrix, Vec3Param vector)
	{
		float x = Dot(matrix.Basis(0), vector);
		float y = Dot(matrix.Basis(1), vector);
		float z = Dot(matrix.Basis(2), vector);
		return Vector3(x, y, z);
	}

	void TransposedTransform(Mat3Param matrix, Vec3Ptr vector)
	{
		if (vector == NULL)
			ErrorIf(vector == NULL, "Matrix3 - Null pointer passed for vector.");
		float x = Dot(matrix.Basis(0), *vector);
		float y = Dot(matrix.Basis(1), *vector);
		float z = Dot(matrix.Basis(2), *vector);
		vector->Set(x, y, z);
	}

	float Trace(Mat3Param matrix)
	{
		return matrix.m00 + matrix.m11 + matrix.m22;
	}

	float Cofactor(Mat3Param matrix, unsigned row, unsigned column)
	{
		if (row > 2)
			ErrorIf(row > 2, "Matrix3 - Row index out of range.");
		if (column > 2)
			ErrorIf(column > 2, "Matrix3 - Column index out of range.");

		//Negative if r+c is odd, positive if even
		float sign = ((row + column) % 2) == 1 ? -1.0f : 1.0f;

		float matrix2[4];
		unsigned i = 0;
		for (unsigned r = 0; r < 3; ++r)
		{
			if (r != row)
			{
				for (unsigned c = 0; c < 3; ++c)
				{
					if (c != column)
					{
						matrix2[i] = matrix(r, c);
						++i;
					}
				}
			}
		}

		return sign * ((matrix2[0] * matrix2[3]) - (matrix2[1] * matrix2[2]));
	}

	///Diagonalizes a symmetric matrix (M = M^T)
	void Diagonalize(Mat3Ptr matrix)
	{
		if (matrix == NULL)
			ErrorIf(matrix == NULL, "Matrix3 - Null pointer passed for matrix.");

		Matrix3 quatMatrix = ToMatrix3(CreateDiagonalizer(*matrix));
		*matrix = Concat(Concat(quatMatrix, *matrix), quatMatrix.Transposed());
	}

	Matrix3 Diagonalized(Mat3Param matrix)
	{
		Matrix3 newMatrix = matrix;
		Diagonalize(&newMatrix);
		return newMatrix;
	}

	void Invert(Mat3Ptr matrix)
	{
		matrix->Invert();
	}

	Matrix3 Inverted(Mat3Param matrix)
	{
		return matrix.Inverted();
	}

	const Matrix3 Matrix3::Outer(const Vector3 &v1, const Vector3 &v2)
	{
		return Matrix3(v1.x * v2.x, v1.x * v2.y, v1.x * v2.z,
			v1.y * v2.x, v1.y * v2.y, v1.y * v2.z,
			v1.z * v2.x, v1.z * v2.y, v1.z * v2.z);
	}

	Matrix3& Matrix3::One(void)
	{
		m00 = m01 = m02 = 1.0f;
		m10 = m11 = m12 = 1.0f;
		m20 = m21 = m22 = 1.0f;
		return *this;
	}

	Math::Mat3Ref Matrix3::Prepend(const Matrix3 &lhs)
	{
		return *this = lhs * *this;
	}

}