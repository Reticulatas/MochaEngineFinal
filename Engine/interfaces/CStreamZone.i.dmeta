%{
#include "CStreamZone.h"
#include "CStreamMarker.h"
	%}

class CStreamMarker : public Component
{
	CStreamMarker();
	~CStreamMarker();
public:
	std::string markerID;
};

class CStreamZoneListener : public Component
{
	CStreamZoneListener();
	~CStreamZoneListener();
};

class CStreamZone : public Component
{
	CStreamZone();
	~CStreamZone();
public:
	enum StreamMode {
		GENERIC_LOAD,
		GENERIC_UNLOAD,
		PARALLEL_LOAD,
		ZONE_UNLOAD
	};

	IZone zone;

	std::string mapZoneId;
	StreamMode mode;
	std::string fileToStream;
	bool forceZoneUnusableAfterStream;
	bool useOffsetFromThisPos;
	Math::Vector3 offset;

	bool mUseMarkers;
	std::string mMarkerID;

	bool mPreAccumulateUnloadObjs;
	bool incrementalDeload;	

	bool GetIsStreaming();
	bool GetHasFinishedStreaming();

	void SetStreamMode(StreamMode _mode);
	void SetFileToStream(std::string _file);
};
