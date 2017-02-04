#pragma once

#include <yaml-cpp/yaml.h>

#include <glm/vec2.hpp>
#include <string>
#include <memory>

class Texture {
	unsigned    mHandle;

	std::string mFile;

public:
	Texture();
	~Texture();

	bool load(const std::string& s);
	void free();
	void bind();

	void write(YAML::Emitter& e);
};
