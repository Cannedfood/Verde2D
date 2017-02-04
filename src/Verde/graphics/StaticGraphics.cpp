#include "StaticGraphics.hpp"

#include "../Object.hpp"

#include <GL/gl.h>

StaticGraphics::StaticGraphics() :
	mDoesWrap(false),
	mWrapping(0.1f)
{}

StaticGraphics::~StaticGraphics() {}

void StaticGraphics::draw(const Object* o) {
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	bind();

	glm::vec2 p0, p1;
	if(mDoesWrap) {
		p0 = o->mBounds.min * mWrapping * glm::vec2(1, -1);
		p1 = o->mBounds.max * mWrapping * glm::vec2(1, -1);
	}
	else {
		p0 = glm::vec2(0, 1);
		p1 = glm::vec2(1, 0);
	}
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(p0.x, p0.y); glVertex2fv(&o->mBounds.min[0]);
		glTexCoord2f(p1.x, p0.y); glVertex2f (o->mBounds.max.x, o->mBounds.min.y);
		glTexCoord2f(p0.x, p1.y); glVertex2f (o->mBounds.min.x, o->mBounds.max.y);
		glTexCoord2f(p1.x, p1.y); glVertex2fv(&o->mBounds.max[0]);
	glEnd();
}

void StaticGraphics::write(YAML::Emitter& e) {
	if(mDoesWrap) {
		e << YAML::Key << "wrapping" << YAML::BeginSeq;
		e << mWrapping.x << mWrapping.y;
		e << YAML::EndSeq;
	}

	Texture::write(e);
}

bool StaticGraphics::setWrapping(const glm::vec2& wrapping) {
	mWrapping = wrapping;
	mDoesWrap = true;
	return true;
}
