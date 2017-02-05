#pragma once

#include "Behavior.hpp"

class HoverCamFollow : public Behavior {
private:
	uint32_t mFollowing = 0;

	bool mFlee      = false;
	float mHeight   = .5f;
	float mDistance = 0.8f;
	float mRandomizeTime = 0;

	void randomize();
public:
	void load  (Object* o, YAML::Node n) override;
	void write (Object* o, YAML::Emitter& e) override;

	void start (Object* o) override;
	void update(Object* o, float dt) override;
};
