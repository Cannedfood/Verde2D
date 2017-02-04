#pragma once

#include "Object.hpp"

#include <memory>
#include <yaml-cpp/yaml.h>

class Texture;

class Chunk {
	Level*                   mLevel;
	glm::vec2                mOffset;

	std::vector<std::unique_ptr<Object>> mObjects;
public:
	bool remove(Object* o);
	void add(std::unique_ptr<Object>&& o);

	void read (YAML::Node n);
	void write(YAML::Emitter& e);
};
