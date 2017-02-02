#include "Object.hpp"

#include "Level.hpp"

#include "Texture.hpp"

Object::Object() :
	mLevel(nullptr),
	mType(DYNAMIC)
{}

Object::~Object() {
	if(mLevel)
		mLevel->removeObject(this);
}

void Object::setTexture(const std::string &file) {
	mTexture = Texture::Load(file);
}
