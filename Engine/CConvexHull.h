#pragma once
#include "Component.h"

class CConvexHull : public Component
{
	bool mOnce;
public:
	virtual void OnStart(void);

	virtual void OnInit(void);

	virtual void OnUpdate(void);

	virtual void OnAlwaysUpdate(void);

	virtual void OnFree(void);

	virtual void OnEditorUpdate(void);

	virtual void CopyFrom(const ::CConvexHull* c);

	COMPONENTCLONE(CConvexHull);

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		___SERIALIZE_END___

public:
	
	metadef(CConvexHull)
		m_addbase(Component)
		endmetadef;

};

___SERIALIZE_CLASS(CConvexHull, 1);

//safety