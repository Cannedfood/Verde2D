#include "Audio.hpp"

#include <AL/al.h>
#include "util/Alerror.hpp"

#include "AudioContext.hpp"

#include "../graphics/Camera.hpp"

static AudioContext* pAudio;

namespace internal {

	void InitAudio() {
		pAudio = new AudioContext;
		if(!pAudio->init()) exit(-1);
		puts("[AUDIO] Initialized");

		float orientation[] = {
			0, 0, -1, // Forwards
			0, 1, 0   // Up
		};
		alListenerfv(AL_ORIENTATION, orientation);
		alDistanceModel(AL_NONE);
	}

	void FreeAudio() {
		delete pAudio;
	}

	void UpdateAudio() {
		alListener3f(AL_POSITION, CamPosition().x, CamPosition().y, -CamZoom());
	}

} /* internal */

std::shared_ptr<AudioData> OpenAudio(const char* path, AudioData::OpenType type) {
	return AudioData::Open(path, type);
}

void PlayBgm(const std::shared_ptr<AudioData>& data, float crossfade, bool cubic_fade) {
	if(!data) return;
	pAudio->music().play(data, crossfade, cubic_fade);
}

void PlayBgm(const char* file, float crossfade, bool cubic_fade) {
	printf("Set bgm: %s\n", file);
	PlayBgm(OpenAudio(file), crossfade, cubic_fade);
}

void PlaySfx(const std::shared_ptr<AudioData>& data, const glm::vec2& pos, float gain, float pitch) {
	if(!data) return;
	pAudio->sfx().play(data, pos, gain, pitch);
}

void PlaySfx(const char* file, const glm::vec2& pos, float gain, float pitch) {
	PlaySfx(OpenAudio(file), pos, gain, pitch);
}

void PlaySfx(const std::shared_ptr<AudioData>& data, float gain, float pitch) {
	if(!data) return;
	pAudio->sfx().play(data, gain, pitch);
}

void PlaySfx(const char* file, float gain, float pitch) {
	PlaySfx(OpenAudio(file), gain, pitch);
}

std::unique_ptr<AudioNode> CreateAudioSource();
