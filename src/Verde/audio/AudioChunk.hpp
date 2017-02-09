#pragma once

#include <string>

class AudioChunk {
private:
	unsigned    mBuffer;
	std::string mFile;

	float mGain;
	float mPitch;

	friend class AudioSource;
public:
	AudioChunk();
	~AudioChunk();

	void free();
	bool load(const std::string& path);
};
