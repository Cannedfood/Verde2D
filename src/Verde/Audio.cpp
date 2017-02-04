#include "Audio.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <unordered_map>
#include <list>

class AudioChunk {
	unsigned mHandle;

	AudioChunk(const std::string& path) {

	}
};

namespace {
	ALCdevice*  pDevice  = nullptr;
	ALCcontext* pContext = nullptr;


	struct AudioState {
		std::unordered_map<std::string, std::shared_ptr<AudioChunk>> chunk_cache;

		float listener_distance = 1;

		unsigned bgm[2];

		AudioState() {
			alGenSources(2, bgm);
		}

		~AudioState() {
			alDeleteSources(2, bgm);
		}

		void Update() {

		}
	} *pAudio = nullptr;

} /* anonymous namespace */

namespace internal {
	void InitAudio() {
		pDevice = alcOpenDevice(NULL); // Open default device
		if(!pDevice) {
			puts("[Audio] Failed opening OpenAL device");
			exit(-1);
		}

		pContext = alcCreateContext(pDevice, NULL);
		if(!alcMakeContextCurrent(pContext)) {
			puts("[Audio] Failed creating OpenAL context");
			alcCloseDevice(pDevice);
			exit(-1);
		}

		pAudio = new AudioState;
	}

	void FreeAudio() {
		if(pAudio) {
			delete pAudio;
			pAudio = nullptr;
		}

		if(pContext) {
			alcSuspendContext(pContext);
			alcDestroyContext(pContext);
			pContext = nullptr;
		}

		if(pDevice) {
			alcCloseDevice(pDevice);
			pDevice = nullptr;
		}
	}

	void Update(const glm::vec2& camera_position) {
		alListener3f(AL_POSITION, camera_position.x, camera_position.y, -pAudio->listener_distance);
		pAudio->Update();
	}
} /* internal */
