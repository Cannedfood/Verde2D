#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <cstdint>

#include "PhysicsObject.hpp"

struct Manifold {
	PhysicsObject* a;
	PhysicsObject* b;
	glm::vec2      normal;
	float          penetration;
};

class Level {
	std::uint32_t               mHighestId;
	std::vector<PhysicsObject*> mStaticObjects;
	std::vector<Box*>           mWaterAreas;

	std::vector<PhysicsObject*> mDynamicObjects;
	std::vector<PhysicsObject*> mParticleObjects;

	std::vector<Manifold> mManifolds;

	glm::vec2 mGravity = { 0, -9.8f };
public:
	~Level();

	const std::vector<Manifold>& getManifolds() { return mManifolds; }

	void addObject(PhysicsObject* o, PhysicsObject::Type type);
	void removeObject(PhysicsObject* o);

	void debugDraw();
	void draw();
	void update(float dt);
};
