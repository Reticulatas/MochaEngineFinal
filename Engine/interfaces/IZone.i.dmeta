%{
#include "IZone.h"
%}

class IZone
{
	IZone();
	~IZone();
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

	void DrawBounds();
};