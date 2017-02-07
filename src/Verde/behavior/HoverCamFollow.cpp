#include "HoverCamFollow.hpp"

#include "../Object.hpp"
#include "../Level.hpp"

void HoverCamFollow::load  (Object* o, YAML::Node n) {}
void HoverCamFollow::write (Object* o, YAML::Emitter& e) {
	e << YAML::BeginMap;
	e << YAML::Key << "type" << "hovercam-follow";
	e << YAML::EndMap;
}

void HoverCamFollow::start(Object *o) {
	mFollowing = o->mLevel->getId("player");
	randomize();
}

void HoverCamFollow::randomize() {
	mHeight = 0.2f + (rand() % 100) * 0.01f;
	mDistance = 0.5f + (rand() % 100) * 0.01f;
	mRandomizeTime = 0.6f + (rand() % 100) * 0.007f;
}

void HoverCamFollow::update(Object* o, float dt) {
	Box hover_volume;
	hover_volume.max.x = o->mRelativeBounds.max.x * 0.9f + o->mPosition.x;
	hover_volume.min.x = o->mRelativeBounds.min.x * 0.9f + o->mPosition.x;

	hover_volume.max.y = o->mBounds.min.y - 0.1f;
	hover_volume.min.y = hover_volume.max.y - mHeight;

	Box intersect;
	if(o->mLevel->hitTestArea(hover_volume, &intersect)) {
		o->mMotion.y -= std::copysign(1 * dt, o->mMotion.y);
		o->mMotion.y += intersect.area() * 50 * dt;
	}

	mRandomizeTime -= dt;
	if(mRandomizeTime <= 0)
		randomize();

	if(mFollowing) {
		if(Object* fol = o->mLevel->get(mFollowing)) {
			const float break_speed  = 4.f;
			const float normal_speed = 4.f;
			const float flee_speed   = 16.f;

			const float flee_to = 8.f * mDistance;
			const float keep    = 7.f * mDistance;
			const float flee_at = 2.5f * mDistance;

			glm::vec2 dif = fol->mPosition - o->mPosition;
			float     len = glm::length(dif);

			if(!mFlee) {
				if(len < flee_at)
					mFlee = true;
				else if(len < keep) {
					o->mMotion.x *= 0.999f;
					o->mMotion.x -= std::copysign(break_speed, dif.x) * dt;
				}
				else
					o->mMotion.x += std::copysign(normal_speed, dif.x) * dt;

				o->mFlipOrientation = dif.x < 0;
			}
			else {
				if (len >= flee_to) {
					if (o->mMotion.x * dif.x < -1)
						o->mMotion.x *= 0.999f;
					else {
						mFlee = false;
						randomize();
					}
				}
				else
					o->mMotion.x -= std::copysign(flee_speed, dif.x) * dt;

				o->mFlipOrientation = dif.x > 0;
			}
		}
	}
}
