#pragma once

#include "AudioData.hpp"

#include <glm/vec2.hpp>

#include <memory>
#include <vector>
#include <mutex>

class AudioSfx {
	struct Source {
		unsigned source;
		std::shared_ptr<AudioData> data;
	};

	unsigned            mNextSource;
	std::vector<Source> mSources;
	std::mutex          mMutex;

	// Requires ownership of mMutex
	Source& getSource();
public:
	AudioSfx(size_t n_sources);
	~AudioSfx();

	void update();

	void play(const std::shared_ptr<AudioData>& data, const glm::vec2& p, float gain = 1, float pitch = 1);
	void play(const std::shared_ptr<AudioData>& data, float gain = 1, float pitch = 1);
};
