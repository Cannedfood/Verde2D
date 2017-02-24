#pragma once

#include "../AudioData.hpp"

#include <cstdint>
#include <memory>

class AudioDataWavStatic : public AudioData {
	unsigned mBuffer;

public:
	AudioDataWavStatic();
	~AudioDataWavStatic();

	bool init(void* p_drwav);
	bool load(const char* file);

	void start(unsigned source) override;
	void stop(unsigned source) override;
	void pause(unsigned source, bool b) override;

	void update(unsigned source) override;

	int flags() override;
};

class AudioDataWavStreamed : public AudioData {
	void*       mWav;

	unsigned mFormat;

	unsigned                   mLastHandledBuffer;
	unsigned                   mBuffers[4];
	std::size_t                mSamplesPerBuffer;
	std::unique_ptr<int16_t[]> mBufferData;

public:
	AudioDataWavStreamed();
	~AudioDataWavStreamed();

	bool init(void* p_drwav, float forward_buffering_seconds);
	bool load(const char* file, float forward_buffering_seconds);

	void start(unsigned source) override;
	void stop(unsigned source) override;
	void pause(unsigned source, bool b) override;

	void update(unsigned source) override;

	int flags() override;
};

namespace AudioDataWav {
	std::shared_ptr<AudioData> Open(const char* file, AudioData::OpenType type = AudioData::AUTO);
}
