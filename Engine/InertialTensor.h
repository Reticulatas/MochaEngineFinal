/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
const Math::Matrix3 ShiftInertiaTensorReferencePoint(const Math::Matrix3 &inertiaTensor, const Math::Vector3 &r, float mass);
const Math::Matrix3 UnitInertiaTensorTetrahedron(const Math::Vector3 &v0, const Math::Vector3 &v1, const Math::Vector3 &v2, const Math::Vector3 &v3, const Math::Vector3 &ref);