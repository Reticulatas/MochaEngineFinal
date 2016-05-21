#pragma once
#include "Component.h"

class CZone : public Component
{
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

	virtual void CopyFrom(const ::CZone* c)
	{
		//position = c->position;
		width = c->width;
		height = c->height;
		depth = c->depth;
		Component::CopyFrom(c);
	}
	
	bool IsPointWithin(Math::Vector3 point);
	void ClampObjectToBounds(GameObject* gobj);

	COMPONENTCLONE(CZone);
private:
	

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		___DEFSER(width, 1)
		___DEFSER(height, 1)
		___DEFSER(depth, 1)
		___SERIALIZE_END___

	metadef(CZone)
		m_addbase(Component)
		endmetadef;

	virtual void OnStart(void);

	virtual void OnInit(void);

	virtual void OnUpdate(void);

	virtual void OnAlwaysUpdate(void);

	virtual void OnFree(void);

	virtual void OnDraw(void);

	virtual void OnEditorUpdate(void);

};

___SERIALIZE_CLASS(CZone,1);

//Original Author: Nicholas Fuller



