#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <cstdint>
#include <unordered_map>

#include "Object.hpp"

class Level {
	std::uint32_t        mNextId = 1;

	std::vector<Object*> mStaticObjects;
	std::vector<Object*> mDynamicObjects;
	std::vector<Object*> mParticleObjects;

	glm::vec2 mGravity = { 0, -9.8f };

	std::unordered_map<uint32_t, Object*> mObjectIds;
	std::unordered_map<std::string, uint32_t> mObjectAlias;
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

	bool hitTest    (const Box& b);
	bool hitTestArea(const Box& b, Box* to);

	void     alias(Object* o, const std::string& alias);
	uint32_t getId(const std::string& alias);
	Object*  get(const std::string& alias);
	Object*  get(uint32_t id);

	void __freeId(uint32_t id);
	void __createId(Object* o);
};
