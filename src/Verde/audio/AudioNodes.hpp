#pragma once

#include "AudioNode.hpp"

#include <memory>
#include <mutex>
#include <vector>

class AudioNodeImpl;

class AudioNodes {
private:
	std::mutex                  mMutex;

	struct Source {
		unsigned source;
		AudioNodeImpl* node;
	};

	std::vector<Source>         mSources;
	std::vector<AudioNodeImpl*> mNodes;

public:
	std::unique_ptr<AudioNode> createNode();

	void eraseNode(AudioNode* n);

	AudioNodes(size_t nsources);
	~AudioNodes();
};
