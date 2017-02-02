#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>

/// An axis-aligned bounding box
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

	/// Can go in the wrong direction if the boxes don't actually touch
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

	/// Creates a copy of the box, offset by v
	Box offset(const glm::vec2& v) const { return Box { min + v, max + v }; }

	/// Returns a box representing the overlap between this and the other box
	Box overlap(const Box& other) const {
		return Box {
			glm::min(min, other.min),
			glm::max(max, other.max)
		};
	}

	/// Returns the width/height of this box
	glm::vec2 size() const { return max - min; }
	/// Returns the 2d volume of this box
	float     volume() const { return glm::compMul(size()); }
	/// Returns the middle point of this box
	glm::vec2 middle() const { return (max + min) * 0.5f; }

	/// Makes sure that min contains the minimum and max the maximum values
	void fix() {
		glm::vec2 new_min = glm::min(min, max);
		max = glm::max(min, max);
		min = new_min;
	}
};
