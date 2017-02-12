#include "AudioContext.hpp"

#include <AL/al.h>

#include "util/Alerror.hpp"

#include "../Settings.hpp"

void AudioContext::start() {
	mRunning = true;
	mThread = std::thread([this]() {
		puts("[AUDIO] Thread started!");

		using namespace std::chrono;
		using namespace std::literals::chrono_literals;

		high_resolution_clock::time_point last = high_resolution_clock::now();
		while(mRunning) {
			float dt;
			{
				high_resolution_clock::time_point now = high_resolution_clock::now();
				dt = duration_cast<microseconds>(now - last).count() * 0.000001f;
				last = now;
			}

			mMusic->update(dt);
			mSfx->update();
			// TODO: update rest

			std::this_thread::sleep_for(20ms);
		}

		puts("[AUDIO] Thread ended!");
	});
}

AudioContext::AudioContext() :
	mDevice(nullptr),
	mContext(nullptr),
	mRunning(false)
{}

AudioContext::~AudioContext() {
	if(mRunning) {
		mRunning = false;
		mThread.join();
	}

	mMusic.reset();
	mSfx.reset();

	if(mContext) {
		alcMakeContextCurrent(NULL);
		alcDestroyContext(mContext);
	}

	if(mDevice)
		alcCloseDevice(mDevice);
}

bool AudioContext::init() {
	if(mRunning) return false;

	mDevice = alcOpenDevice(NULL);
	if(!mDevice) {
		puts("[AUDIO] Failed opening openal device!");
		return false;
	}

	mContext = alcCreateContext(mDevice, NULL);
	if(!mContext) {
		puts("[AUDIO] Failed creating openal context!");
		return false;
	}

	if(!alcMakeContextCurrent(mContext)) {
		puts("[AUDIO] Failed making openal context active!");
		return false;
	}

	mMusic.reset(new AudioMusic);
	if(!mMusic->init()) {
		mMusic.reset();
		return false;
	}

	size_t n_sfx_sources = 8;
	if(YAML::Node audio = GetSettings()["audio"]) {
		if(YAML::Node srcs = audio["sources_sfx"]) {
			n_sfx_sources = srcs.as<size_t>(n_sfx_sources);
		}
	}

	mSfx.reset(new AudioSfx(n_sfx_sources));

	start();

	return true;
}
