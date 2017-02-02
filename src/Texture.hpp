#pragma once

#include <glm/vec2.hpp>
#include <string>
#include <memory>

class Texture {
	unsigned  mHandle;

	bool      mDoesWrap;
	glm::vec2 mWorldSize;

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

	static std::shared_ptr<Texture> Load(const std::string& s);

	static void Init();
	static void Quit();
};