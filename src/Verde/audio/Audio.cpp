#include "Audio.hpp"

#include "AudioContext.hpp"

static AudioContext* pAudio;

namespace internal {

	void InitAudio() {
		pAudio = new AudioContext;
		if(!pAudio->init()) exit(-1);
	}

	void FreeAudio() {
		delete pAudio;
	}

	void UpdateAudio() {

	}

} /* internal */

std::shared_ptr<AudioData> OpenAudio(const char* path, AudioData::OpenType type) {
	return AudioData::Open(path, type);
}

void PlayBgm(const std::shared_ptr<AudioData>& data, float crossfade, bool cubic_fade) {
	pAudio->music().play(data, crossfade, cubic_fade);
}

void PlayBgm(const char* file, float crossfade, bool cubic_fade) {
	PlayBgm(OpenAudio(file), crossfade, cubic_fade);
}

void PlaySfx(const glm::vec2& pos, const std::shared_ptr<AudioData>& data, float gain) {

}

void PlaySfx(const glm::vec2& pos, const char* file, float gain) {
	PlaySfx(pos, OpenAudio(file), gain);
}

std::unique_ptr<AudioNode> CreateAudioNode();
