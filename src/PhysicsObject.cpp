#include "PhysicsObject.hpp"

#include "Level.hpp"

#include "Texture.hpp"

PhysicsObject::PhysicsObject() :
	mLevel(nullptr),
	mType(DYNAMIC)
{}

PhysicsObject::~PhysicsObject() {
	if(mLevel)
		mLevel->removeObject(this);
}

void PhysicsObject::setTexture(const std::string &file) {
	mTexture = Texture::Load(file);
}
