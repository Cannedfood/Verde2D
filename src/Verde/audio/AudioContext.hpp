#pragma once

#include <thread>
#include <mutex>

#include <AL/alc.h>

#include "AudioMusic.hpp"

class AudioContext {
	std::mutex mMutex;

	ALCdevice*  mDevice;
	ALCcontext* mContext;

	bool        mRunning;
	std::thread mThread;

	std::unique_ptr<AudioMusic> mMusic;

	void start();
public:
	AudioContext();
	~AudioContext();

	bool init();

	std::unique_lock<std::mutex> lock() {
		return std::unique_lock<std::mutex>(mMutex);
	}

	std::unique_lock<std::mutex> try_lock() {
		return std::unique_lock<std::mutex>(mMutex, std::try_to_lock);
	}

	inline AudioMusic& music() { return *mMusic; }
};
