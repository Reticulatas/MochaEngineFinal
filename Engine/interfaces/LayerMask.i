%{
#include "LayerMask.h"
%}

class LayerMask
{
	LayerMask();
	~LayerMask();
public:
	enum Layers
	{
		LAYER_0 = 0x1,
		LAYER_1 = 0x2,
		LAYER_2 = 0x4,
		LAYER_3 = 0x8,
		LAYER_4 = 0x10,
		LAYER_5 = 0x20,
		LAYER_NORAYCAST = 0x40,
		LAYER_UI = 0x80,

		LAYER_TOTAL
	};

	void AddLayer(Layers layer);
	void RemoveLayer(Layers layer);
	int GetLayerMasks();
	bool IsOnLayer(Layers layer);
};