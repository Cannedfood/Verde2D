#include "Audio.hpp"

#include "AudioChunk.hpp"
#include "ALDebug.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <unordered_map>
#include <list>
#include <set>

namespace {
	ALCdevice*  pDevice  = nullptr;
	ALCcontext* pContext = nullptr;

	struct AudioState {
		std::unordered_map<std::string, std::shared_ptr<AudioChunk>> chunk_cache;


		float listener_distance = 1;

		unsigned bgm[2];

		glm::vec2 last_cam_position;

		AudioState() {
			alGenSources(2, bgm);

			float orientation[] = {
				0, 0, -1, // Forward
				0, 1,  0  // Up
			};
			alListenerfv(AL_ORIENTATION, orientation);
		}

		~AudioState() {
			alDeleteSources(2, bgm);
		}

		void Update(const glm::vec2& cam_position, float cam_zoom, float dt) {
			glm::vec2 cam_motion = (last_cam_position - cam_position) * dt;
			last_cam_position = cam_position;

			alListener3f(AL_POSITION, cam_position.x, cam_position.y, cam_zoom * -2);
			alListener3f(AL_VELOCITY, cam_motion.x, cam_motion.y, 0);
		}
	} *pAudio = nullptr;

} /* anonymous namespace */

namespace internal {
	static void AudioFail(const char* msg) {
		puts(msg);
		FreeAudio();
		exit(-1);
	}

	void InitAudio() {
		pDevice = alcOpenDevice(NULL); // Open default device
		if(!pDevice)
			AudioFail("[Audio] Failed opening OpenAL device");

		pContext = alcCreateContext(pDevice, NULL);
		if(!alcMakeContextCurrent(pContext))
			AudioFail("[Audio] Failed creating OpenAL context");

		alCheckErr();

		pAudio = new AudioState;

		alCheckErr();

		internal::UpdateAudio({0, 0}, 1, 1);
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

	void UpdateAudio(const glm::vec2& camera_position, float zoom, float dt) {
		pAudio->Update(camera_position, zoom, dt);
	}
} /* internal */
