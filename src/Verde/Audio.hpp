#pragma once

#include <memory>
#include <glm/vec2.hpp>

namespace internal {
	void InitAudio();
	void FreeAudio();
	void Update(const glm::vec2& camera_position);
} /* internal */

class AudioChunk;

std::shared_ptr<AudioChunk> LoadSound(const std::string& name);

void SetAudioDistance(float z);

void PlayAmbient(const std::shared_ptr<AudioChunk>& p, const glm::vec2& at, float volume);
void PlayAmbient(const std::string& name, const glm::vec2& at, float volume);

void SetBGM(const std::string& name, float fade);
