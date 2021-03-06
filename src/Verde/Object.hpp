#pragma once

#include "Box.hpp"
#include "behavior/Behavior.hpp"

#include <glm/glm.hpp>
#include <memory>

#include <yaml-cpp/yaml.h>

class AudioSource;
class Graphics;
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
	uint32_t  mId;

	float     mHeight = 0;

	glm::vec2 mMotion;
	glm::vec2 mPosition;

	Box       mRelativeBounds;
	Box       mBounds;

	bool                         mFlipOrientation = false;
	double                       mAnimationTime = 0;
	std::shared_ptr<Graphics>    mGraphics;
	std::shared_ptr<AudioSource> mAudio;
	std::unique_ptr<Behavior>    mBehavior;

	YAML::Node mData;

	enum Flags {
		F_OWNED_BY_LEVEL = 1, //< Marks that this entity should be deleted when it's removed. You can still delete this object manually though.
		F_OWNED_BY_CHUNK = 2
	};

	unsigned mFlags = 0;

	Object();
	~Object();

	void setGraphics(const std::string& file);

	void _onRemove();
	void _onAttach();

	void save(YAML::Emitter& to);
	void load(YAML::Node& from);

	uint32_t getId();
	void alias(const std::string& alias);

	// Centers the position into the bounds
	void center();
};
