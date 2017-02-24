#pragma once

#include "Object.hpp"
#include "Box.hpp"

#include <memory>
#include <yaml-cpp/yaml.h>

class Texture;

class Chunk {
	Level*                   mLevel;
	Chunk*                   mParentChunk;

	glm::vec2                mOffset;

	std::vector<std::unique_ptr<Chunk>>  mChunks;
	std::vector<std::unique_ptr<Object>> mObjects;

	std::string mFile;

	void _removeFromParent();
public:
	void init(Level* l);

	inline Level* level() const { return mLevel; }

	std::unique_ptr<Object> remove(Object* o);
	Object*                 add   (std::unique_ptr<Object>&& o, int type = -1);

	Chunk* createChunk();
	void   removeChunk(Chunk* c);

	void offset(const glm::vec2& p);

	Box bounds();

	void clear();
	void clean();
	bool empty();

	void load(YAML::Node n);
	void save(YAML::Emitter& e);
};
