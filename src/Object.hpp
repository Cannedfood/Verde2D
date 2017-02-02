#pragma once

#include "Box.hpp"

#include <glm/glm.hpp>
#include <memory>

class Texture;
class Level;

class Object {
public:
	enum Type {
		STATIC,  //!< Only active interaction
		DYNAMIC, //!< Passive & active interaction
		PARTICLE //!< Only passive interaction
	};

	Level*    mLevel;
	Type      mType;

	glm::vec2 mMotion;
	glm::vec2 mPosition;

	Box       mRelativeBounds;
	Box       mBounds;

	std::shared_ptr<Texture> mTexture;

	Object();
	~Object();

	void setTexture(const std::string& file);
};
