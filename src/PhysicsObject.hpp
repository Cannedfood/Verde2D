#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <memory>

struct Box {
	glm::vec2 min, max;

	bool touches(const Box& other) const {
		return
			other.max.x > min.x &&
			max.x > other.min.x &&
			other.max.y > min.y &&
			max.y > other.min.y;
	}

	bool contains(const glm::vec2& p) const {
		return
			max.x >= p.x &&
			min.x <= p.x &&
			max.y >= p.y &&
			min.y <= p.y;
	}

	// Can go in the wrong direction if the boxes don't actually touch
	void shortestSeparation(const Box& other, glm::vec2* store_in) {
		glm::vec2 p = max - other.min;
		glm::vec2 n = min - other.max;

		*store_in = {
			fabs(p.x) < fabs(n.x) ? p.x : n.x,
			fabs(p.y) < fabs(n.y) ? p.y : n.y
		};

		if(fabs(store_in->x) < fabs(store_in->y))
			store_in->y = 0;
		else
			store_in->x = 0;
	}

	Box offset(const glm::vec2& v) const { return Box { min + v, max + v }; }

	Box overlap(const Box& other) const {
		return Box {
			glm::min(min, other.min),
			glm::max(max, other.max)
		};
	}

	glm::vec2 size() const { return max - min; }
	float     volume() const { return glm::compMul(size()); }

	glm::vec2 middle() const { return (max + min) * 0.5f; }

	void fix() {
		glm::vec2 new_min = glm::min(min, max);
		max = glm::max(min, max);
		min = new_min;
	}
};

class Texture;
class Level;

class PhysicsObject {
public:
	enum Type {
		STATIC,  //!< Only active interaction
		DYNAMIC, //!< Passive & active interaction
		PARTICLE //!< Only passive interaction
	};

	Level*    mLevel;
	Type      mType;

	glm::vec2 mMotion;
	glm::vec2 mPosition;

	Box       mRelativeBounds;
	Box       mBounds;

	std::shared_ptr<Texture> mTexture;

	PhysicsObject();
	~PhysicsObject();

	void setTexture(const std::string& file);
};
