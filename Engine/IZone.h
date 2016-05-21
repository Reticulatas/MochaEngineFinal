#pragma once
#include "MathIncludes.h"

class IZone
{
public:
	float width;
	float height;
	float depth;

	float GetLeftBound();
	float GetRightBound();
	float GetBottomBound();
	float GetTopBound();
	float GetFrontBound();
	float GetBackBound();

	void Init(GameObject* g);

	Math::Vector3 GetDiagonalCornerZero();

	Math::Vector3 GetDiagonalCornerOne();

	bool IsPointWithin(Math::Vector3 point);
	void ClampObjectToBounds(GameObject* gobj);

	std::vector<GameObject*> GetObjectsInBounds();

	Math::Vector3 GetCenter();

	void DrawBounds();

	IZone();
	IZone(IZone& zone);

	___SERIALIZE_BEGIN___
		___DEFSER(width, 1)
		___DEFSER(height, 1)
		___DEFSER(depth, 1)
		___DEFSER(attachedObj, 1)
	___SERIALIZE_END___
private:
	GameObject* attachedObj;
};

___SERIALIZE_CLASS(IZone, 1);

//original author: Nicholas Fuller