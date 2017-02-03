#include "Object.hpp"

#include "Level.hpp"

#include "Texture.hpp"

Object::Object() :
	mLevel(nullptr),
	mType(DYNAMIC)
{}

Object::~Object() {
	if(mLevel) {
		mFlags &= ~F_OWNED_BY_LEVEL;
		mLevel->removeObject(this);
	}
}

void Object::setTexture(const std::string &file) {
	mTexture = Texture::Load(file);
}

void Object::_onRemove() {
	mLevel = nullptr;
	if(mFlags & F_OWNED_BY_LEVEL)
		delete this;
}
