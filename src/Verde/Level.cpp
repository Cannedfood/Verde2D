#include "Level.hpp"

#include "Object.hpp"
#include "graphics/Graphics.hpp"

#ifdef _WIN32
#	include <Windows.h> // Dependency of include <GL/gl.h>
#endif

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
	if(o->mGraphics)
		o->mGraphics->draw(o);
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
	for(auto* o : mDynamicObjects)  o->_onRemove();
	for(auto* o : mStaticObjects)   o->_onRemove();
	for(auto* o : mParticleObjects) o->_onRemove();
}

void Level::alias(Object *o, const std::string &alias) {
	mObjectAlias.emplace(alias, o->getId());
}

uint32_t Level::getId(const std::string &alias) {
	auto iter = mObjectAlias.find(alias);
	if(iter == mObjectAlias.end())
		return 0;
	else
		return iter->second;
}

Object* Level::get(const std::string &alias) {
	// TODO: clean up aliases from time to time
	auto iter = mObjectAlias.find(alias);
	if(iter == mObjectAlias.end())
		return nullptr;
	else
		return get(iter->second);
}

Object* Level::get(uint32_t id) {
	return mObjectIds[id];
}

void Level::__createId(Object* o) {
	o->mId = mNextId++;
	mObjectIds.emplace(o->mId, o);
}

void Level::__freeId(uint32_t id) {
	mObjectIds.erase(id);
}

void Level::addObject(Object* o, int type) {
	switch(type) {
		case Object::DYNAMIC:  mDynamicObjects.push_back(o);  o->mType = Object::DYNAMIC; break;
		case Object::STATIC:   mStaticObjects.push_back(o);   o->mType = Object::STATIC; break;
		case Object::PARTICLE: mParticleObjects.push_back(o); o->mType = Object::PARTICLE; break;
		default: addObject(o, o->mType);       break;
	}

	o->mLevel = this;
	o->_onAttach();

	//printf("Added object %p\n", o);
}

void Level::addOwned(std::unique_ptr<Object>&& o, int type) {
	o->mFlags |= Object::F_OWNED_BY_LEVEL;
	addObject(o.release(), type);
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

	if(o->mId != 0) {

	}

	o->_onRemove();

	//printf("Removed object %p\n", o);
}

void Level::debugDraw(const Box& b) {
	glColor4f(1, 1, 1, 1);
	for(auto* p : mStaticObjects) {
		if(p->mBounds.touches(b))
			drawBBox(p->mBounds);
	}

	glColor4f(0, 0, 1, 1);
	for(auto* p : mParticleObjects) {
		if(p->mBounds.touches(b))
			drawBBox(p->mBounds);
	}

	glColor4f(1, 0, 0, 1);
	for(auto* p : mDynamicObjects) {
		if(p->mBounds.touches(b))
			drawBBox(p->mBounds);
	}
}

void Level::draw(const Box& b) {
	for(auto* p : mStaticObjects) {
		if(p->mBounds.touches(b))
			drawObject(p);
	}

	for(auto* p : mParticleObjects) {
		if(p->mBounds.touches(b))
			drawObject(p);
	}

	for(auto* p : mDynamicObjects) {
		if(p->mBounds.touches(b))
			drawObject(p);
	}
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
	for(auto* o : mStaticObjects) {
		if(o->mBehavior)
			o->mBehavior->update(o, dt);
	}
	for(auto* o : mDynamicObjects) {
		if(o->mBehavior)
			o->mBehavior->update(o, dt);
	}
	for(auto* o : mParticleObjects) {
		if(o->mBehavior)
			o->mBehavior->update(o, dt);
	}

	auto comp = [](auto* a, auto* b) {
		return a->mHeight < b->mHeight;
	};

	std::stable_sort(mStaticObjects.begin(), mStaticObjects.end(), comp);
	std::stable_sort(mDynamicObjects.begin(), mDynamicObjects.end(), comp);
	std::stable_sort(mParticleObjects.begin(), mParticleObjects.end(), comp);

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

Object* Level::at(const glm::vec2& p, int types) {
	if(types & Object::STATIC) {
		for(Object* o : mStaticObjects) {
			if(o->mBounds.contains(p))
				return o;
		}
	}
	if(types & Object::DYNAMIC) {
		for(Object* o : mDynamicObjects) {
			if(o->mBounds.contains(p))
				return o;
		}
	}
	if(types & Object::PARTICLE) {
		for(Object* o : mParticleObjects) {
			if(o->mBounds.contains(p))
				return o;
		}
	}

	return nullptr;
}

bool Level::at(const glm::vec2& p, std::vector<Object*>& to, size_t n, int types) {
	size_t left = n;

	if(types & Object::STATIC) {
		for(Object* o : mStaticObjects) {
			if(o->mBounds.contains(p)) {
				to.emplace_back(o);
				if(--left == 0) goto L_FINISHED;
			}
		}
	}
	if(types & Object::DYNAMIC) {
		for(Object* o : mDynamicObjects) {
			if(o->mBounds.contains(p)) {
				to.emplace_back(o);
				if(--left == 0) goto L_FINISHED;
			}
		}
	}
	if(types & Object::PARTICLE) {
		for(Object* o : mParticleObjects) {
			if(o->mBounds.contains(p)) {
				to.emplace_back(o);
				if(--left == 0) goto L_FINISHED;
			}
		}
	}

L_FINISHED:
	return (n - left) != 0;
}

bool Level::hitTest(const Box& b) {
	for(auto* o : mDynamicObjects) {
		if(o->mBounds.touches(b)) return true;
	}

	for(auto* o : mStaticObjects) {
		if(o->mBounds.touches(b)) return true;
	}

	return false;
}

bool Level::hitTestArea(const Box& b, Box* intersection) {
	bool first = true;

	// OPTIMIZE: hopefully this expands, so we don't have if(first) all the time?
	for(auto* o : mDynamicObjects) {
		if(o->mBounds.touches(b)) {
			if(first) {
				first = false;
				*intersection = b.overlap(o->mBounds);
			}
			else {
				*intersection = intersection->expand(b.overlap(o->mBounds));
			}
		}
	}

	for(auto* o : mStaticObjects) {
		if(o->mBounds.touches(b)) {
			if(first) {
				first = false;
				*intersection = b.overlap(o->mBounds);
			}
			else {
				*intersection = intersection->expand(b.overlap(o->mBounds));
			}
		}
	}

	return !first;
}
