#pragma once

#include <stdexcept>

static void _checkAl(const char* statement, const char* file, int line, const char* func) {
	if(int err = alGetError()) {
		printf("%s:%i (%s)", file, line, func);
		switch (err) {
			case AL_INVALID_ENUM: puts("AL_INVALID_ENUM"); break;
			case AL_INVALID_NAME: puts("AL_INVALID_ENUM"); break;
			case AL_INVALID_OPERATION: puts("AL_INVALID_ENUM"); break;
			case AL_INVALID_VALUE: puts("AL_INVALID_ENUM"); break;
			default: printf("%i\n", err); break;
		}

		throw std::runtime_error("An OpenAL error occured");
	}
}

#ifdef _WIN32
#	define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define AL_CHECK(X) do { X; _checkAl(#X, __FILE__, __LINE__, __PRETTY_FUNCTION__); } while(false)

#ifndef AUDIO_NO_DEBUG

/*
#define alGenBuffers
#define alDeleteBuffers
#define alBufferData

#define alGenSources
#define alDeleteSources
#define alSourcei
#define alSourceiv
#define alSource3i
#define alSourcef
#define alSourcefv
#define alSource3f
#define alSourcePlay
#define alSourcePause
#define alSourceStop
#define alSourceQueueBuffers
#define alSourceUnqueueBuffers
*/

#define alGenBuffers(...) AL_CHECK( alGenBuffers(__VA_ARGS__) )
#define alDeleteBuffers(...) AL_CHECK( alDeleteBuffers(__VA_ARGS__) )
#define alBufferData(...) AL_CHECK( alBufferData(__VA_ARGS__) )

#define alGenSources(...) AL_CHECK( alGenSources(__VA_ARGS__) )
#define alDeleteSources(...) AL_CHECK( alDeleteSources(__VA_ARGS__) )
#define alSourcei(...) AL_CHECK( alSourcei(__VA_ARGS__) )
#define alSourceiv(...) AL_CHECK( alSourceiv(__VA_ARGS__) )
#define alSource3i(...) AL_CHECK( alSource3i(__VA_ARGS__) )
#define alSourcef(...) AL_CHECK( alSourcef(__VA_ARGS__) )
#define alSourcefv(...) AL_CHECK( alSourcefv(__VA_ARGS__) )
#define alSource3f(...) AL_CHECK( alSource3f(__VA_ARGS__) )
#define alSourcePlay(...) AL_CHECK( alSourcePlay(__VA_ARGS__) )
#define alSourcePause(...) AL_CHECK( alSourcePause(__VA_ARGS__) )
#define alSourceStop(...) AL_CHECK( alSourceStop(__VA_ARGS__) )
#define alSourceQueueBuffers(...) AL_CHECK( alSourceQueueBuffers(__VA_ARGS__) )
#define alSourceUnqueueBuffers(...) AL_CHECK( alSourceUnqueueBuffers(__VA_ARGS__) )

#endif
