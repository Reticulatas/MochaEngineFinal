#pragma once

#include "CZone.h"
#include "IZone.h"
#include "Component.h"

class CMapZone : public Component
{
public:
	std::string id;
	IZone zone;

	CMapZone();

	virtual void CopyFrom(const ::CMapZone* c)
	{
		//position = c->position;
		zone = c->zone;
		zone.Init(gameObject);
		id = c->id;
		Component::CopyFrom(c);
	}

	COMPONENTCLONE(CMapZone);

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		___DEFSER(zone,1);
		___DEFSER(id, 1)
		___SERIALIZE_END___

public:

		metadef(CMapZone)
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

___SERIALIZE_CLASS(CMapZone, 1);

//Original Author: Nicholas Fuller





