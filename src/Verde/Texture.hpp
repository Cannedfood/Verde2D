#pragma once

#include <yaml-cpp/yaml.h>

#include <glm/vec2.hpp>
#include <string>
#include <memory>
#include <chrono>

class Texture {
	unsigned    mHandle;

	bool        mDoesWrap;
	glm::vec2   mWorldSize;

	std::chrono::microseconds mLoadTime;
	std::string               mFile;

public:
	const glm::vec2& getWrapping() { return mWorldSize; }
	bool doesWrap() const { return mDoesWrap; }

	void setWrapping(const glm::vec2& wrapping) { mWorldSize = wrapping; mDoesWrap = true; }
	void disableWrapping() { mDoesWrap = false; }

	Texture();
	~Texture();

	bool load(const std::string& s);
	void free();
	void bind();

	void write(YAML::Emitter& e);

	static std::shared_ptr<Texture> Load(const std::string& s);
	static std::shared_ptr<Texture> Load(YAML::Node n);

	static void InitCache();
	static void FreeCache();
	static void CleanCache();
};
