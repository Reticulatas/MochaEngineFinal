%{
#include "EditorSubsystem.h"
%}

%template(GameObjectVector)std::vector<GameObject*>;

namespace EditorSubsystemTools
{
	class WidgetManager
	{
	private:
		WidgetManager();
		~WidgetManager();
	public:
		enum WidgetType
		{
			LIGHT, SOUND, CAMERA
		};

		GameObject* GetAttachedObjFromWidgetObj(GameObject* widgetObj);
	};
};

class EditorSubsystem
{
	EditorSubsystem();
	~EditorSubsystem();
public:
	bool IsTesting();

	EditorState* GetEditorState() const;
	CCamera* EditorCamera() const;
	GameObject* EditorTransformWidget() const;

	void Register_Tick(MochaDotNetCallback _callback);
	void SetSelectedObjects(std::vector<GameObject*> objs);

	void BeginTesting();
	void EndTesting();

	EditorSubsystemTools::WidgetManager widgetManager;

	static EditorSubsystem* getInstance();
	
	int gridSize;
};

class EditorState : public BaseState
{
	EditorState();
	~EditorState();
public:
};

