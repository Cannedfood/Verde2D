#include "AudioDataWav.hpp"

#define DR_WAV_IMPLEMENTATION
#include "3rd_party/dr_wav.h"

#include <AL/al.h>

#include "../util/Alerror.hpp"

namespace AudioDataWav {
	std::shared_ptr<AudioData> Open(const char* file, AudioData::OpenType type) {
		drwav* wav = drwav_open_file(file);
		if(!wav) {
			printf("Failed opening '%s'\n", file);
			return nullptr;
		}

		// To stream or not to stream  - HamLED (William Shakesbit)
		bool stream_it;
		switch (type) {
			case AudioData::AUTO:
				// Larger than 100kB ? -> stream it
				stream_it = (wav->totalSampleCount * wav->bytesPerSample) > (1024 * 100);
				if(stream_it)
					printf("[AUDIO WAV] Decided to stream '%s'\n", file);
				else
					printf("[AUDIO WAV] Decided NOT to stream '%s'\n", file);
				break;
			case AudioData::STREAM: stream_it = true; break;
			case AudioData::BUFFER: stream_it = false; break;
		}

		if(stream_it) {
			auto p = std::make_shared<AudioDataWavStreamed>();
			if(!p->init(wav, 2))
				return nullptr;
			else
				return p;
		}
		else {
			auto p = std::make_shared<AudioDataWavStatic>();
			if(!p->init(wav))
				return nullptr;
			else
				return p;
		}
	}
}


static size_t next_pow2(size_t t) {
	int shifted = 0;
	for(shifted = 0; t != 0; t >>= 1, shifted++);
	return 1 << (shifted - 1);
}

AudioDataWavStatic::AudioDataWavStatic() :
	mBuffer(0)
{}

AudioDataWavStatic::~AudioDataWavStatic() {
	if(mBuffer)
		alDeleteBuffers(1, &mBuffer);
}

bool AudioDataWavStatic::init(void* p_drwav) {
	drwav* wav = (drwav*) p_drwav;
	if(!wav) return false;

	unsigned channels, sampleRate;
	size_t totalSampleCount;
	void* data = drwav__read_and_close_s16(wav, &channels, &sampleRate, &totalSampleCount);

	if(!mBuffer) alGenBuffers(1, &mBuffer);

	alBufferData(
		mBuffer,
		channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
		data, totalSampleCount, sampleRate
	);

	drwav_free(data);

	return true;
}

bool AudioDataWavStatic::load(const char *file) {
	return init(drwav_open_file(file));
}

void AudioDataWavStatic::start(unsigned source) {
	alSourcei(source, AL_BUFFER, mBuffer);
	alSourcePlay(source);
}
void AudioDataWavStatic::stop(unsigned source) {
	alSourceStop(source);
	alSourcei(source, AL_BUFFER, 0);
}
void AudioDataWavStatic::pause(unsigned source) {
	alSourcePause(source);
}
void AudioDataWavStatic::update(unsigned source) {
	// Nothing to do
}
int AudioDataWavStatic::flags() {
	return F_STATIC;
}


AudioDataWavStreamed::AudioDataWavStreamed() :
	mWav(nullptr)
{
	memset(mBuffers, 0, sizeof(mBuffers));
}
AudioDataWavStreamed::~AudioDataWavStreamed() {
	if(mWav)
		drwav_close((drwav*) mWav);
	if(mBuffers[0])
		alDeleteBuffers(4, mBuffers);
}

bool AudioDataWavStreamed::init(void *p_drwav, float forward_buffering_seconds) {
	drwav* wav = (drwav*) p_drwav;
	if(!wav) return false;

	if(mWav) drwav_close((drwav*) mWav);
	mWav = wav;

	if(wav->channels == 1)
		mFormat = AL_FORMAT_MONO16;
	else if(wav->channels == 2)
		mFormat = AL_FORMAT_STEREO16;
	else {
		printf("Only mono and stereo wav files are supported!");
		drwav_close(wav);
		return false;
	}

	mSamplesPerBuffer = next_pow2(forward_buffering_seconds * 0.25f * wav->sampleRate);

	mBufferData.reset(new short[mSamplesPerBuffer]);

	if(mBuffers[0] == 0)
		alGenBuffers(4, mBuffers);

	for (size_t i = 0; i < 4; i++) {
		drwav_read_s16(wav, mSamplesPerBuffer, mBufferData.get());
		alBufferData(mBuffers[i], mFormat, mBufferData.get(), mSamplesPerBuffer * sizeof(int16_t), wav->sampleRate);
	}

	return true;
}

bool AudioDataWavStreamed::load(const char *file, float forward_buffering_seconds) {
	return init(drwav_open_file(file), forward_buffering_seconds);
}

void AudioDataWavStreamed::start(unsigned source) {
	if(!mWav) return;

	alSourceQueueBuffers(source, 4, mBuffers);
	mLastHandledBuffer = 0;

	alSourcePlay(source);
}
void AudioDataWavStreamed::stop(unsigned source) {
	alSourceStop(source);
	int nQueuedBuffers;
	alGetSourcei(source, AL_BUFFERS_QUEUED, &nQueuedBuffers);
	unsigned tmp_buffers[4];
	alSourceUnqueueBuffers(source, nQueuedBuffers, tmp_buffers);
}
void AudioDataWavStreamed::pause(unsigned source) {
	alSourcePause(source);
}
void AudioDataWavStreamed::update(unsigned source) {
	int processed;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

	// TODO: get all processed buffers at once
	// TODO: stop or loop at end
	while(processed--) {
		size_t read = drwav_read_s16((drwav*) mWav, mSamplesPerBuffer, mBufferData.get());
		if(read == 0) {
			//puts("Stream EOF!");
			drwav_seek_to_sample((drwav*) mWav, 0);
			read = drwav_read_s16((drwav*) mWav, mSamplesPerBuffer, mBufferData.get());
		}

		unsigned buffer;
		alSourceUnqueueBuffers(source, 1, &buffer);
		alBufferData(buffer, mFormat, mBufferData.get(), read * sizeof(int16_t), ((drwav*) mWav)->sampleRate);
		alSourceQueueBuffers(source, 1, &buffer);
		//puts("Pushed buffer!");
	}
}

int AudioDataWavStreamed::flags() {
	return F_STREAM;
}
