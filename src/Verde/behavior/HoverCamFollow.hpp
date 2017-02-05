#pragma once

#include "Behavior.hpp"

class HoverCamFollow : public Behavior {
	uint32_t mFollowing = 0;

	bool mFlee = false;

	void load  (Object* o, YAML::Node n) override;
	void write (Object* o, YAML::Emitter& e) override;

	void start (Object* o) override;
	void update(Object* o, float dt) override;
};
