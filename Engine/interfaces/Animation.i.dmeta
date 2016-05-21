%{
#include "Animation.h"
%}

struct KeyFrame
{
  float Time;
  VQS LocalTransform;

  KeyFrame()
  {
    Time = 0;
  }
};

struct Track
{
  std::vector<KeyFrame> mKeyFrames;
};

class Animation : public Asset
{
  std::string mTrackName;
  float mDuration;
  std::vector<Track> mTracks;

public:

  Animation(std::string name = "");
  virtual void OnAssetFileChanged();

  float GetDuration() const;
  std::vector<Track>& GetTracks();
  std::string GetName();
};