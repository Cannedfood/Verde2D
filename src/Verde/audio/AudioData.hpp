#pragma once

#include <memory>
#include <string>

class AudioData {
	std::string mSourceName;
public:
	enum Flags {
		F_STREAM = 1,
		F_STATIC = 0,
		F_CACHABLE = 2
	};

	const std::string& sourceName() { return mSourceName; }

	virtual void start(unsigned source) = 0;
	virtual void pause(unsigned source) = 0;
	virtual void stop(unsigned source) = 0;

	virtual void update(unsigned source) = 0;

	virtual int flags() = 0;

	bool isStream() { return flags() & F_STREAM; }
	bool isStatic() { return (~flags()) & F_STREAM; }

	virtual ~AudioData() {}

	enum OpenType {
		AUTO,
		STREAM,
		BUFFER
	};

	static void InitCache();
	static void FreeCache();
	static void CleanCache();

	static std::shared_ptr<AudioData> OpenNotCached(const char* path, OpenType type = AUTO);
	static std::shared_ptr<AudioData> Open(const char* path, OpenType type = AUTO);
};
