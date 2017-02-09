#include "ALDebug.hpp"

#include <AL/al.h>
#include <cstdio>

const char* alErrorString(ALenum error) {
#define ERRCASE(X) case X: return #X
	switch (error) {
		ERRCASE(AL_INVALID_NAME);
		ERRCASE(AL_INVALID_ENUM);
		ERRCASE(AL_INVALID_VALUE);
		ERRCASE(AL_INVALID_OPERATION);
		ERRCASE(AL_OUT_OF_MEMORY);
		default: return nullptr;
	}
#undef ERRCASE
}

bool alCheckErrImpl(const char* file, const char* fun, int line) {
	if(ALenum err = alGetError()) {
		printf("OpenAL error: %X (%s) in %s:%s:%i\n", err, alErrorString(err), file, fun, line);
		return true;
	}
	else return false;
}
