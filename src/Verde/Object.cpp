#include "Object.hpp"

#include "Level.hpp"

#include "graphics/Texture.hpp"
#include "graphics/Graphics.hpp"

Object::Object() :
	mLevel(nullptr),
	mType(DYNAMIC),
	mAnimationTime(0)
{}

Object::~Object() {
	if(mLevel) {
		mFlags &= ~F_OWNED_BY_LEVEL;
		mLevel->removeObject(this);
	}
}

void Object::setGraphics(const std::string &file) {
	mGraphics = Graphics::Load(file);
}

void Object::_onRemove() {
	mLevel = nullptr;
	if(mFlags & F_OWNED_BY_LEVEL)
		delete this;
}

void Object::write(YAML::Emitter& e) {
	e << YAML::BeginMap;

	e << YAML::Key << "type" << YAML::Value << (int) mType;

	if(mMotion.x != 0 || mMotion.y != 0) {
		e << YAML::Key << "motion" << YAML::Flow << YAML::BeginSeq;
		e << mMotion.x << mMotion.y;
		e << YAML::EndSeq;
	}

	if(mPosition.x != 0 || mPosition.y != 0) {
		e << YAML::Key << "position" << YAML::Flow << YAML::BeginSeq;
		e << mPosition.x << mPosition.y;
		e << YAML::EndSeq;
	}

	if(mHeight != 0)
		e << YAML::Key << "height" << mHeight;

	e << YAML::Key << "bounds" << YAML::Flow << YAML::BeginSeq;
	e << mRelativeBounds.min.x << mRelativeBounds.min.y << mRelativeBounds.max.x << mRelativeBounds.max.y;
	e << YAML::EndSeq;

	if(mGraphics) {
		e << YAML::Key << "graphics";
		mGraphics->write(e);
	}

	e << YAML::EndMap;
}

void Object::read(YAML::Node& n) {
	mType = (Object::Type) n["type"].as<int>();

	if(YAML::Node nn = n["motion"]) {
		mMotion.x = nn[0].as<float>();
		mMotion.y = nn[1].as<float>();
	}

	if(YAML::Node nn = n["position"]) {
		mPosition.x = nn[0].as<float>();
		mPosition.y = nn[1].as<float>();
	}

	if(YAML::Node nn = n["bounds"]) {
		mRelativeBounds.min.x = nn[0].as<float>();
		mRelativeBounds.min.y = nn[1].as<float>();
		mRelativeBounds.max.x = nn[2].as<float>();
		mRelativeBounds.max.y = nn[3].as<float>();
	}
	else puts("NO BOUNDS!!");

	mGraphics = Graphics::Load(n["graphics"]);
}
