#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <cstdint>

#include "Object.hpp"

class Level {
	std::uint32_t        mHighestId;

	std::vector<Object*> mStaticObjects;
	std::vector<Object*> mDynamicObjects;
	std::vector<Object*> mParticleObjects;

	glm::vec2 mGravity = { 0, -9.8f };
public:
	~Level();

	void addObject(Object* o, int type = 0);
	void addOwned(std::unique_ptr<Object>&& o, int type = 0);
	void removeObject(Object* o);

	/// Draws the outlines of all boxes, the color depends on the kind of box
	void debugDraw(const Box& b);
	/// Draws all boxes touching b
	void draw(const Box& b);
	void update(float dt);

	Object* at(const glm::vec2& p, int types);
	bool    at(const glm::vec2& p, std::vector<Object*>& to, size_t n, int types);
};
