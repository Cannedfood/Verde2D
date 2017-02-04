#include "AtlasAnimation.hpp"

#include "../Object.hpp"

#include <GL/gl.h>

void AtlasAnimation::draw(const Object* o) {
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	bind();

	glm::vec2 p0 = glm::vec2(0, 1);
	glm::vec2 p1 = glm::vec2(1, 0);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(p0.x, p0.y); glVertex2fv(&o->mBounds.min[0]);
		glTexCoord2f(p1.x, p0.y); glVertex2f (o->mBounds.max.x, o->mBounds.min.y);
		glTexCoord2f(p0.x, p1.y); glVertex2f (o->mBounds.min.x, o->mBounds.max.y);
		glTexCoord2f(p1.x, p1.y); glVertex2fv(&o->mBounds.max[0]);
	glEnd();
}

void AtlasAnimation::write(YAML::Emitter& e) {
	using namespace YAML;
	e << BeginMap;
	e << Key << "frames"  << mNumFrames;
	e << Key << "fps"     << 1.f / mFrameTime;
	e << Key << "texture"; Texture::write(e);
	e  << EndMap;
}
