/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
class PhysicsWorld;
struct Proxy;

class PhysicsDebugDraw 
{
	friend class PhysicsWorld;
public:
	static const unsigned k_dynamicRigidBodyLineColor = 0xFFFFFFFF;
	static const unsigned k_kinematicRigidBodyLineColor = 0xFF00FFFF;
	static const unsigned k_staticRigidBodyLineColor = 0xFF00A000;
	static const unsigned k_sleepingRigidBodyLineColor = 0xFF808080;
	static const unsigned k_sensorLineColor = 0xFFC00000;
	static const unsigned k_contactLineColor = 0xFFFF0000;
	static const unsigned k_xAxisLineColor = 0xFFFF0000;
	static const unsigned k_yAxisLineColor = 0xFF00FF00;
	static const unsigned k_zAxisLineColor = 0xFF0000FF;
	static const unsigned k_centroidLineColor = 0xFFFFFFFF;
	static const unsigned k_proxyLineColor = 0xFF00FFFF;
	static const unsigned k_proxyCollidedLineColor = 0xFFCC9900;
	static const unsigned k_jointLineColor = 0xFF00FFFF;
	static const unsigned k_aabbLineColor1 = 0xFFFFFF00;
	static const unsigned k_aabbLineColor2 = 0xFF00FFFF;
	static const unsigned k_rayCastHitLineColor = 0xFF00FF00;
	static const unsigned k_rayCastMissLineColor = 0xFFFF0000;
	static const unsigned k_rayCastNormalLineColor = 0xFF00FFFF;
	static const float k_axisSize;
	static const float k_centroidSize;
	static const float k_contactSize;
	static const float k_contactNormalSize;
protected:
	virtual void Begin(void) { }
	virtual void End(void) { }
public:
	virtual void DrawLine(const Math::Vector3 &point1, const Math::Vector3 &point2, unsigned color) = 0;
	virtual void DrawSphere(const Math::Vector3 &center, float radius, const Math::Matrix3 &orientation, unsigned color) = 0;
	virtual void DrawAABB(const Math::Vector3 &minPoint, const Math::Vector3 &maxPoint, unsigned color) = 0;
	virtual void DrawBox(const Math::Vector3 &center, const Math::Vector3 &halfExtent, const Math::Matrix3 &orientation, unsigned color) = 0;
	virtual void DrawPlane(const Math::Vector3 &center, float width, float height, const Math::Matrix3 &orientation, unsigned color) = 0;
	virtual void DrawProxy(const Proxy &proxy, unsigned color) = 0;
};