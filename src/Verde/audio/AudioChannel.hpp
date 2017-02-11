#pragma once

#include <vector>

class AudioChannel {
	std::vector<unsigned> mFreeSources;
	
public:
	AudioChannel();
	~AudioChannel();

	void init(unsigned max_sources);
};
