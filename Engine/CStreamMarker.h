#pragma once
#include "IZone.h"
#include "Component.h"

class CStreamMarker : public Component
{
public:
	virtual void CopyFrom(const ::CStreamMarker* c)
	{
		markerID = c->markerID;
		Component::CopyFrom(c);
	}

	std::string markerID;

	COMPONENTCLONE(CStreamMarker);

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		___DEFSER(markerID, 1)
		___SERIALIZE_END___

	virtual void OnStart(void);
	virtual void OnInit(void);
	virtual void OnUpdate(void);
	virtual void OnAlwaysUpdate(void);
	virtual void OnFree(void);
	virtual void OnDraw(void);
	virtual void OnEditorUpdate(void);

	metadef(CStreamMarker)
		m_addbase(Component)
		endmetadef
};
___SERIALIZE_CLASS(CStreamMarker, 1);