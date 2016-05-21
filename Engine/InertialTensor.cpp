#include "stdafx.h"
#include "InertialTensor.h"
#include <cmath>
const Math::Matrix3 ShiftInertiaTensorReferencePoint(const Math::Matrix3 &inertiaTensor, const Math::Vector3 &r, float mass)
{
	// Parallel Axis Theorem: 
	// http://en.wikipedia.org/wiki/Parallel_axis_theorem
	return inertiaTensor + mass * (r.LengthSq() * Math::Matrix3().SetIdentity() - Math::Matrix3::Outer(r, r));
}

const Math::Matrix3 UnitInertiaTensorTetrahedron(const Math::Vector3 &v0, const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3, const Math::Vector3 &ref)
{
	// How to Find The Inertia Tensor (or Other Mass Properties) of A 3D Solid Body Represented by A Triangle Mesh
	// by Jonathan Blow
	// http://number-none.com/blow/inertia/bb_inertia.doc

	// C_canonical
	static const Math::Matrix3 canonicalTensor(1.0f / 60.0f, 1.0f / 120.0f, 1.0f / 120.0f,
		1.0f / 120.0f, 1.0f / 60.0f, 1.0f / 120.0f,
		1.0f / 120.0f, 1.0f / 120.0f, 1.0f / 60.0f);

	// X_avg
	static const Math::Vector3 caninicalCentroid(0.25f, 0.25f, 0.25f);

	// A
	const Math::Vector3 v01 = v1 - v0;
	const Math::Vector3 v02 = v2 - v0;
	const Math::Vector3 v03 = v3 - v0;
	const Math::Matrix3 a(v01.x, v02.x, v03.x,
		v01.y, v02.y, v03.y,
		v01.z, v02.z, v03.z);

	// C_linearlyTransformed = det(A) * A * C_canonical * A'
	const float det = std::fabs(a.Determinant());
	Math::Matrix3 c = det * a * canonicalTensor * a.Transposed();

	// C_translated = C_linearlyTransformed + mass * (translation * X_avg' + x_avg * translation' + x_avg * x_avg')
	static const float k_oneSixth = 1.0f / 6.0f;
	const float volume = det * k_oneSixth;
	const Math::Vector3 translation = v0 - ref;
	c += volume * (Math::Matrix3::Outer(translation, caninicalCentroid) + Math::Matrix3::Outer(caninicalCentroid, translation) + Math::Matrix3::Outer(translation, translation));

	// I = trace(C) * eye(3) - C
	return (c.m00 + c.m11 + c.m22) * Math::Matrix3().SetIdentity() - c;
}