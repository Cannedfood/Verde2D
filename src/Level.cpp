#include "Level.hpp"

#include "Object.hpp"
#include "Texture.hpp"

#include <GL/gl.h>

#include <cstdio>
#include <algorithm>

static void drawBBox(const Box& b) {
	glBegin(GL_LINE_LOOP);
		glVertex2fv(&b.min[0]);
		glVertex2f (b.max.x, b.min.y);
		glVertex2fv(&b.max[0]);
		glVertex2f (b.min.x, b.max.y);
	glEnd();
}

static void drawObject(Object* o) {
	if(o->mTexture) {
		glEnable(GL_TEXTURE_2D);
		glColor3f(1, 1, 1);
		o->mTexture->bind();

		glm::vec2 p0, p1;
		if(o->mTexture->doesWrap()) {
			p0 = o->mBounds.min * o->mTexture->getWrapping() * glm::vec2(1, -1);
			p1 = o->mBounds.max * o->mTexture->getWrapping() * glm::vec2(1, -1);
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
	else {
		glDisable(GL_TEXTURE_2D);
		glColor3f(0.65f, 0.37f, 0.67f);
		glBegin(GL_TRIANGLE_STRIP);
			glVertex2fv(&o->mBounds.min[0]);
			glVertex2f (o->mBounds.max.x, o->mBounds.min.y);
			glVertex2f (o->mBounds.min.x, o->mBounds.max.y);
			glVertex2fv(&o->mBounds.max[0]);
		glEnd();
		glColor3f(1, 1, 1);
		glBegin(GL_LINE_LOOP);
			glVertex2fv(&o->mBounds.min[0]);
			glVertex2f (o->mBounds.max.x, o->mBounds.min.y);
			glVertex2fv(&o->mBounds.max[0]);
			glVertex2f (o->mBounds.min.x, o->mBounds.max.y);
		glEnd();
	}
}

Level::~Level() {
	for(auto* o : mDynamicObjects)  o->mLevel = nullptr;
	for(auto* o : mStaticObjects)   o->mLevel = nullptr;
	for(auto* o : mParticleObjects) o->mLevel = nullptr;
}

void Level::addObject(Object* o, Object::Type type) {
	switch(type) {
		default: addObject(o, o->mType);       break;
		case Object::DYNAMIC:  mDynamicObjects.push_back(o);  o->mType = Object::DYNAMIC; break;
		case Object::STATIC:   mStaticObjects.push_back(o);   o->mType = Object::STATIC; break;
		case Object::PARTICLE: mParticleObjects.push_back(o); o->mType = Object::PARTICLE; break;
	}

	o->mLevel = this;

	printf("Added object %p\n", o);
}

void Level::removeObject(Object *o) {
	if(o->mLevel != this) return;

	switch (o->mType) {
		case Object::DYNAMIC:
			mDynamicObjects.erase(std::find(mDynamicObjects.begin(), mDynamicObjects.end(), o));
		break;

		case Object::STATIC:
			mStaticObjects.erase(std::find(mStaticObjects.begin(), mStaticObjects.end(), o));
		break;

		case Object::PARTICLE:
			mParticleObjects.erase(std::find(mParticleObjects.begin(), mParticleObjects.end(), o));
		break;
	}

	o->mLevel = nullptr;

	printf("Removed object %p\n", o);
}

void Level::debugDraw() {
	glColor4f(1, 1, 1, 1);
	for(auto* p : mStaticObjects)
		drawBBox(p->mBounds);

	glColor4f(0, 0, 1, 1);
	for(auto* p : mParticleObjects)
		drawBBox(p->mBounds);

	glColor4f(1, 0, 0, 1);
	for(auto* p : mDynamicObjects)
		drawBBox(p->mBounds);
}

void Level::draw() {
	for(auto* p : mStaticObjects)
		drawObject(p);

	for(auto* p : mParticleObjects)
		drawObject(p);

	for(auto* p : mDynamicObjects)
		drawObject(p);
}

static void resolveOneWay(std::vector<Object*>& av, std::vector<Object*>& bv, float dt) {
	for(auto* a : av) {
		for(auto* b : bv) {
			if(a->mBounds.touches(b->mBounds)) {
				glm::vec2 correction;
				a->mBounds.shortestSeparation(b->mBounds, &correction);
				a->mPosition -= correction;

				glm::vec2 normal = glm::normalize(correction);
				float sameness   = glm::dot(normal, a->mMotion);
				if(sameness > 0) {
					a->mMotion -= normal * sameness;
				}
			}
		}
	}
}

static void resolveTwoWay(std::vector<Object*>& a, std::vector<Object*>& b) {

}

void Level::update(float dt) {
	for(auto* o : mStaticObjects)
		o->mBounds = o->mRelativeBounds.offset(o->mPosition);

	for(auto* o : mDynamicObjects) {
		o->mPosition += o->mMotion * dt;
		o->mBounds    = o->mRelativeBounds.offset(o->mPosition);

		o->mMotion   += mGravity * dt;
	}

	for(auto* o : mParticleObjects) {
		o->mPosition += o->mMotion * dt;
		o->mBounds    = o->mRelativeBounds.offset(o->mPosition);

		o->mMotion   += mGravity * dt;
	}

	resolveOneWay(mDynamicObjects,  mStaticObjects, dt);
	resolveOneWay(mParticleObjects, mStaticObjects, dt);
	resolveOneWay(mParticleObjects, mDynamicObjects, dt);
}
