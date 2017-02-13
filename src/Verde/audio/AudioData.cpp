#include "AudioData.hpp"

#include "format/AudioDataWav.hpp"

#include <cstring>
#include <unordered_map>

bool ends_in_n(const char* a, size_t a_len, const char* b, size_t b_len) {
	if(b_len > a_len) return false;
	size_t a_off = a_len - b_len;
	return strcmp(a + a_off, b) == 0;
}

static std::unordered_map<std::string, std::weak_ptr<AudioData>>* pAudioCache;

void AudioData::InitCache() {
	pAudioCache = new std::unordered_map<std::string, std::weak_ptr<AudioData>>;
}
void AudioData::FreeCache() {
	delete pAudioCache;
	pAudioCache = nullptr;
}
void AudioData::CleanCache() {

}

std::shared_ptr<AudioData> AudioData::OpenNotCached(const char* cpath, OpenType type) {
	std::string path (cpath);

	std::shared_ptr<AudioData> result;
	if(ends_in_n(path.c_str(), path.size(), ".wav", 4)) {
		printf("[AUDIO LOADER] '%s' is probably a WAV file... (Using wav loader...)\n", path.c_str());
		result = AudioDataWav::Open(path.c_str(), type);
	}
	else {
		printf("[AUDIO LOADER] Failed opening '%s': unknown file ending.\n", path.c_str());
		return nullptr;
	}

	if(result)
		result->mSourceName = path;

	return result;
}

std::shared_ptr<AudioData> AudioData::Open(const char *cpath, AudioData::OpenType type) {
	if(!pAudioCache)
		return OpenNotCached(cpath, type);
	else {
		std::string path (cpath);

		std::shared_ptr<AudioData> result;

		auto iter = pAudioCache->find(path);
		if(iter != pAudioCache->end()) {
			result = iter->second.lock();
		}

		if(!result) {
			result = OpenNotCached(cpath, type);
			if(result && result->flags() & F_CACHABLE)
				pAudioCache->emplace_hint(iter, path, result);
		}

		return result;
	}
}
