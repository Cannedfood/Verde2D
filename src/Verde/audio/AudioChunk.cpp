#include "AudioChunk.hpp"

#include <AL/al.h>

#include <memory>

#include "ALDebug.hpp"

template<typename T, typename TdelReturn, typename TdelArg>
auto scoped_delete(T* t, TdelReturn(*del)(TdelArg)) -> std::unique_ptr<T, TdelReturn(*)(TdelArg)> {
	return std::unique_ptr<T, TdelReturn(*)(TdelArg)>(t, del);
}

#define DR_WAV_IMPLEMENTATION
#include "../3rd_party/dr_wav.h"

AudioChunk::AudioChunk() :
	mBuffer(0)
{}

AudioChunk::~AudioChunk() {
	this->free();
}

void AudioChunk::free() {
	if(mBuffer)
		alDeleteBuffers(1, &mBuffer);
}

bool AudioChunk::load(const std::string& path) {
	free();

	unsigned channels, sampleRate;
	size_t   sampleCount;
	auto pWav = scoped_delete(
			drwav_open_and_read_file_s16(path.c_str(), &channels, &sampleRate, &sampleCount),
			drwav_free
		);

	if(!pWav) return false;

	if(channels == 2)
		printf("[WARN][Audio] Loaded AudioChunk %s with 2 channels: Stereo audio cannot have positional sound!\n", path.c_str());

	alGenBuffers(1, &mBuffer);

	alBufferData(
		mBuffer,
		channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
		pWav.get(), sampleCount * channels, sampleRate
	);

	alCheckErr();

	mFile = path;

	return true;
}
