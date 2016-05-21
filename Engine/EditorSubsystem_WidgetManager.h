#pragma once
#include "MathIncludes.h"
#include "GameObject.h"

namespace EditorSubsystemTools
{
	class WidgetManager
	{
	public:
		enum WidgetType
		{
			LIGHT, SOUND, CAMERA
		};

		void RegisterWidget(GameObject* _attachedObj, WidgetType _type);
		void DeRegisterWidget(GameObject* _attachedObj);

		void Tick();

		GameObject* GetAttachedObjFromWidgetObj(GameObject* widgetObj);

	private:
		struct Widget
		{
			GameObject* attachedObj;
			WidgetType _type;
			GameObject* widgetObj;

			void MakeWidget(GameObject* _attacheObj, WidgetType _type);
			void Destroy();
			void Tick();

			bool operator==(const Widget& other) const;
		};
		std::list<Widget> widgets;
		std::list<Widget*> widgetsToDelete;
	};
};

