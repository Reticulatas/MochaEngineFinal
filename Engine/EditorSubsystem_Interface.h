#pragma once
#include <list>

class GameObject;

namespace EditorSS
{
	void BeginTesting(void);
	void EndTesting(void);
	bool IsTesting(void);

	bool IsInMissionCritical(void);
}