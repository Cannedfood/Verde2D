#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <string>

namespace internal {
	void InitAudio();
	void FreeAudio();
	void UpdateAudio(const glm::vec2& camera_position, float camera_zoom, float dt);
} /* internal */

class AudioChunk;
class AudioSource;

std::shared_ptr<AudioChunk>  LoadSound(const std::string& name);
std::shared_ptr<AudioSource> CreateSoundSource();

void SetAudioDistance(float z);

void PlaySound(AudioSource* source, AudioChunk* data);

void PlayAmbient(const std::shared_ptr<AudioChunk>& p, const glm::vec2& at, float volume);
void PlayAmbient(const std::string& name, const glm::vec2& at, float volume);

void SetBGM(const std::string& name, float fade);
