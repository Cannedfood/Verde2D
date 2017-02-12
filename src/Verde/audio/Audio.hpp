#pragma once

#include <glm/vec2.hpp>

#include "AudioData.hpp"
#include "AudioNode.hpp"

namespace internal {

	void InitAudio();
	void FreeAudio();
	void UpdateAudio();

} /* internal */

std::shared_ptr<AudioData> OpenAudio(const char* path, AudioData::OpenType type = AudioData::AUTO);

void PlayBgm(const std::shared_ptr<AudioData>& data, float crossfade = 0, bool cubic_fade = false);
void PlayBgm(const char* file, float crossfade = 0, bool cubic_fade = false);


void PlaySfx(const std::shared_ptr<AudioData>& data, const glm::vec2& pos, float gain = 1, float pitch = 1);
void PlaySfx(const char* file,                       const glm::vec2& pos, float gain = 1, float pitch = 1);

void PlaySfx(const std::shared_ptr<AudioData>& data, float gain = 1, float pitch = 1);
void PlaySfx(const char* file,                       float gain = 1, float pitch = 1);


std::unique_ptr<AudioNode> CreateAudioSource();
