#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <yaml-cpp/yaml.h>

class AudioChunk;

class AudioSource {
	unsigned mSource;

	std::shared_ptr<AudioChunk> mPlaying;
public:
	AudioSource();
	~AudioSource();

	void setPosition(const glm::vec2& v, const glm::vec2& velocity = glm::vec2{0, 0});

	void play(std::shared_ptr<AudioChunk>& chunk);
	void loop(std::shared_ptr<AudioChunk>& chunk);

	bool load(YAML::Node node);
	void write(YAML::Emitter& e);
};
