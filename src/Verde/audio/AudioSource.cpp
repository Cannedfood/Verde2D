#include "AudioSource.hpp"

#include <AL/al.h>

#include "AudioChunk.hpp"

#include "ALDebug.hpp"

AudioSource::AudioSource() {
	alGenSources(1, &mSource);
	alSourcef (mSource, AL_PITCH, 2);
	alSourcef (mSource, AL_GAIN, 5);
	alSource3f(mSource, AL_POSITION, 0, 0, 0);
	alSource3f(mSource, AL_VELOCITY, 0, 0, 0);
	alSourcei(mSource, AL_DISTANCE_MODEL, AL_LINEAR_DISTANCE_CLAMPED);
	//alSourcef(mSource, AL_EXPONENT_DISTANCE_CLAMPED, ALfloat value)
}

AudioSource::~AudioSource() {
	alDeleteSources(1, &mSource);
}

void AudioSource::setPosition(const glm::vec2& pos, const glm::vec2& vel) {
	alSource3f(mSource, AL_POSITION, pos.x, pos.y, 0);
	alSource3f(mSource, AL_VELOCITY, vel.x, vel.y, 0);
}

void AudioSource::play(std::shared_ptr<AudioChunk>& chunk) {
	mPlaying = chunk;
	alSourcei(mSource, AL_BUFFER, chunk->mBuffer);
	alSourcePlay(mSource);
	alCheckErr();
}

void AudioSource::loop(std::shared_ptr<AudioChunk>& chunk) {
	mPlaying = chunk;
	alSourcei(mSource, AL_LOOPING, AL_TRUE);
	alSourcei(mSource, AL_BUFFER, chunk->mBuffer);
	alSourcePlay(mSource);
}

bool AudioSource::load(YAML::Node node) {
	if(node["file"]) {
		mPlaying = std::make_shared<AudioChunk>();
		mPlaying->load(node["file"].as<std::string>());

		if(node["loop"]) {
			if(YAML::Node time = node["time"])
				alSourcef(mSource, AL_SEC_OFFSET, time.as<float>());
			else {
				alSourcef(mSource, AL_SEC_OFFSET, (rand() % 10000) * 0.001f);
			}
			loop(mPlaying);
		}
		else {
			if(YAML::Node time = node["time"])
				alSourcef(mSource, AL_SEC_OFFSET, time.as<float>());
			play(mPlaying);
		}
	}
	else {
		puts("Audio node doesn't declare a file!");
		return false;
	}

	return true;
}

void AudioSource::write(YAML::Emitter& e) {
	int tmp = 0;
	e << YAML::BeginMap;
	alGetSourcei(mSource, AL_LOOPING, &tmp);
	if(tmp)
		e << YAML::Key << "loop" << true;
	e << YAML::Key << "file" << mPlaying->mFile;
	e << YAML::EndMap;
}
