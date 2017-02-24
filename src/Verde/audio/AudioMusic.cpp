#include "AudioMusic.hpp"

#include <AL/al.h>

#include "util/Alerror.hpp"

#include <cmath>

template<typename T> inline
auto genlock(T& mtex) -> std::unique_lock<T> {
	return std::unique_lock<T>(mtex);
}

template<typename T> inline
auto try_genlock(T& mtex) -> std::unique_lock<T> {
	return std::unique_lock<T>(mtex, std::try_to_lock);
}


AudioMusic::AudioMusic() :
	mInterpolation(1),
	mFromGain(1),
	mSlope(1),
	mCubic(false),
	mPaused(false)
{

}

AudioMusic::~AudioMusic() {
	auto lock = genlock(mMutex);
	if(mFromSource){
		if(mFromData)
			mFromData->stop(mFromSource);

		if(mToData)
			mToData->stop(mToSource);

		alDeleteSources(1, &mFromSource);
		alDeleteSources(1, &mToSource);
	}
}

bool AudioMusic::init() {
	alGenSources(1, &mFromSource);
	alGenSources(1, &mToSource);

	return true;
}

float AudioMusic::_getInterpolation() {
	if(mCubic) {
		// Cubic interpolation:

		// Mapping from x = [0, 1] to x = [-1, 1]
		float x = mInterpolation * 2.f - 1.f;

		// y = x^3 -> y € [-1, 1]
		float y = std::pow(x, 3.f);

		// Mapping from y = [-1, 1] to y = [0, 1]
		return    y * 0.5f + 0.5f;
	}
	else
		return mInterpolation;
}

void AudioMusic::update(float dt) {
	if(mPaused) return;

	if(mInterpolation != 1) {
		auto lock = genlock(mMutex);

		mInterpolation += dt / mSlope;
		if(mInterpolation >= 1) {
			puts("[AUDIO MUSIC] Interpolation finished!");
			mInterpolation = 1;
			if(mFromData) {
				mFromData->stop(mFromSource);
			}

			std::swap(mToSource, mFromSource);
			mFromGain = 1;
			mFromData = std::move(mToData);

			// Resetting gains
			alSourcef(mFromSource, AL_GAIN, 1);
			alSourcef(mToSource,   AL_GAIN, 0);
		}
		else {
			float k = _getInterpolation();
			alSourcef(mToSource, AL_GAIN, k);
			alSourcef(mFromSource, AL_GAIN, (1 - k) * mFromGain);

			// printf("Gains: %1.2f (base: %1.2f) : %1.2f\t|\tk = %f\n",
			// 	(1 - k) * mFromGain,
			// 	mFromGain,
			// 	k,
			// 	mInterpolation
			// );
		}

		if(mFromData)
			mFromData->update(mFromSource);
		if(mToData)
			mToData->update(mToSource);
	}
	else {
		if(auto lock = std::unique_lock<std::mutex>(mMutex, std::try_to_lock)) {
			if(mFromData)
				mFromData->update(mFromSource);
		}
	}
}

void AudioMusic::pause(bool b) {
	mPaused = b;
	auto lock = genlock(mMutex);
	if(mFromData)
		mFromData->pause(mFromSource, b);
	if(mToData)
		mToData->pause(mToSource, b);
}

void AudioMusic::play(
	const std::shared_ptr<AudioData> &data,
	float slope_secs, bool cubic)
{
	auto lock = genlock(mMutex);

	printf("[AUDIO MUSIC] Playing new song:\n"
	       "\t\tsource:'%s'\n"
	       "\t\tinterpolation: %fs\tcubic:%i\n",
		   data->sourceName().c_str(), slope_secs, cubic);

	if(slope_secs == 0) { // Instantly switch music (ugh! whadda u think I wrote all the interpolation for??)
		if(mFromData) {
			mFromData->stop(mFromSource);
		}
		if(mToData) {
			mToData->stop(mToSource);
			mToData.reset();
		}

		mFromData = data;
		mInterpolation = 1;
		mFromGain = 1;
		alSourcef(mFromSource, AL_GAIN, 1);
		data->start(mFromSource);
	}
	else {
		if(mInterpolation != 1) { // Currently interpolating
			// Interpolationg from this audio was interrupted. Stop it.
			if(mFromData) {
				mFromData->stop(mFromSource);
			}

			// Keep gain of the
			mFromGain = _getInterpolation();
			mFromData = std::move(mToData);
			std::swap(mFromSource, mToSource);

			printf("\t\tbase gain %f\n", mFromGain);
		}
		else
			mFromGain = 1;

		mToData = data;
		mInterpolation = 0;
		mSlope = slope_secs;
		mCubic = cubic;

		alSourcef(mToSource, AL_GAIN, 0);
		mToData->start(mToSource);
	}
}
