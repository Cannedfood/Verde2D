#pragma once

#include "Box.hpp"

#include <glm/glm.hpp>
#include <memory>

#include <yaml-cpp/yaml.h>


class Texture;
class Level;
class Chunk;

class Object {
public:
	enum Type {
		STATIC   = 1,  //!< Only active interaction
		DYNAMIC  = 2, //!< Passive & active interaction
		PARTICLE = 4  //!< Only passive interaction
	};

	Level*    mLevel;
	Chunk*    mChunk;

	Type      mType;

	float     mHeight = 0;

	glm::vec2 mMotion;
	glm::vec2 mPosition;

	Box       mRelativeBounds;
	Box       mBounds;

	std::shared_ptr<Texture> mTexture;


	enum Flags {
		F_OWNED_BY_LEVEL = 1, //< Marks that this entity should be deleted when it's removed. You can still delete this object manually though.
		F_OWNED_BY_CHUNK = 2
	};

	unsigned mFlags = 0;

	Object();
	~Object();

	void setTexture(const std::string& file);

	void _onRemove();

	void write(YAML::Emitter& to);
	void read(YAML::Node& from);
};
