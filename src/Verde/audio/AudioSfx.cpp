#include "AudioSfx.hpp"

#include <AL/al.h>

#include "util/Alerror.hpp"

AudioSfx::AudioSfx(size_t n_sources) {
	mSources.resize(n_sources);
	for(auto& s : mSources) {
		alGenSources(1, &s.source);
		if(!s.source) {
			printf("Failed generating sound node! Probably out of sources. terminating. (" __FILE__ ":%i)\n", __LINE__);
			exit(-1);
		}
		alSourcei(s.source, AL_LOOPING, AL_FALSE);
	}

	mNextSource = 0;
}

AudioSfx::~AudioSfx() {
	for(auto& s : mSources) {
		if(s.data)
			s.data->stop(s.source);
		alDeleteSources(1, &s.source);
	}
}

void AudioSfx::update() {
	if(auto lock = std::unique_lock<std::mutex>(mMutex, std::try_to_lock)) {
		for(auto& s : mSources) {
			if(s.data) {
				s.data->update(s.source);
			}
		}
	}
}

AudioSfx::Source& AudioSfx::getSource() {
	auto& s = mSources[mNextSource];
	mNextSource = (mNextSource + 1) % mSources.size();

	if(s.data)
		s.data->stop(s.source);

	return s;
}

void AudioSfx::play(const std::shared_ptr<AudioData> &data, const glm::vec2 &p, float gain, float pitch) {
	std::lock_guard<std::mutex> lock(mMutex);

	auto& s = getSource();

	s.data = data;

	alSourcei (s.source, AL_SOURCE_RELATIVE, AL_FALSE);
	alSource3f(s.source, AL_POSITION, p.x, p.y, 0);
	alSourcef (s.source, AL_GAIN, gain);
	alSourcef (s.source, AL_PITCH, pitch);

	s.data->start(s.source);
}

void AudioSfx::play(const std::shared_ptr<AudioData> &data, float gain, float pitch) {
	std::lock_guard<std::mutex> lock(mMutex);

	auto& s = getSource();

	s.data = data;

	alSourcei (s.source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSource3f(s.source, AL_POSITION, 0, 0, 0);
	alSourcef (s.source, AL_GAIN, gain);
	alSourcef (s.source, AL_PITCH, pitch);

	s.data->start(s.source);
	mNextSource = (mNextSource + 1) % mSources.size();
}
