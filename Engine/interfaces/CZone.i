%{
#include "CZone.h"
%}

class CZone : public Component
{
	CZone();
	~CZone();
public:

	Math::Vector3& GetCenter();

	float width;
	float height;
	float depth;

	float GetLeftBound();
	float GetRightBound();
	float GetBottomBound();
	float GetTopBound();
	float GetFrontBound();
	float GetBackBound();

	Math::Vector3 GetDiagonalCornerZero();
	Math::Vector3 GetDiagonalCornerOne();
	
	bool IsPointWithin(Math::Vector3 point);
	void ClampObjectToBounds(GameObject* gobj);

};
