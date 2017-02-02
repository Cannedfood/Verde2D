#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <memory>

#include "Box.hpp"

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
