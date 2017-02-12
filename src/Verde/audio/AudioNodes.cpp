#include "AudioNodes.hpp"

#include <AL/al.h>
#include "util/Alerror.hpp"

namespace {
	class AudioNodeImpl : public AudioNode {
		AudioNodes* mGroup;

		~AudioNodeImpl() override {
			if(mGroup)
				mGroup->eraseNode(this);
		}
	};
} /* anonymouse namespace */

AudioNodes::AudioNodes(size_t nsources) :
	mSources(nsources)
{
	for(auto& s : mSources) {
		alGenSources(1, &s.source);
		if(!s.source) {
			puts("Failed generating OpenAL audio source. Terminating. (" __FILE__ ")");
			exit(-1);
		}
	}
}

AudioNodes::~AudioNodes() {

}

std::unique_ptr<AudioNode> AudioNodes::createNode() {
	return nullptr;
}

void AudioNodes::eraseNode(AudioNode *n) {

}
