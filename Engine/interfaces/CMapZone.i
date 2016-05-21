%{
#include "CMapZone.h"
	%}

class CMapZone : public Component
{
	CMapZone();
	~CMapZone();
public:

	std::string id;
	IZone zone;
};
