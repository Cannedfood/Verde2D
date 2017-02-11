#pragma once

#include "AudioData.hpp"

#include <memory>
#include <mutex>

class AudioMusic {
	std::mutex mMutex;

	unsigned mFromSource;
	unsigned mToSource;

	std::shared_ptr<AudioData> mFromData;
	std::shared_ptr<AudioData> mToData;

	float mFromGain;

	float mSlope;
	float mInterpolation;
	bool  mCubic;
	bool  mPaused;

	float _getInterpolation();
public:
	AudioMusic();
	~AudioMusic();

	void pause(bool b = true);
	void unpause() { pause(false); }

	bool init();
	void update(float dt);

	void play(const std::shared_ptr<AudioData>& data, float slope_secs = 0, bool cubic = false);
};
