#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <cstdint>

#include "Object.hpp"

struct Manifold {
	Object* a;
	Object* b;
	glm::vec2      normal;
	float          penetration;
};

class Level {
	std::uint32_t               mHighestId;
	std::vector<Object*> mStaticObjects;
	std::vector<Box*>           mWaterAreas;

	std::vector<Object*> mDynamicObjects;
	std::vector<Object*> mParticleObjects;

	std::vector<Manifold> mManifolds;

	glm::vec2 mGravity = { 0, -9.8f };
public:
	~Level();

	const std::vector<Manifold>& getManifolds() { return mManifolds; }

	void addObject(Object* o, Object::Type type);
	void removeObject(Object* o);

	/// Draws the outlines of all boxes, the color depends on the kind of box
	void debugDraw(const Box& b);
	/// Draws all boxes touching b
	void draw(const Box& b);
	void update(float dt);
};
