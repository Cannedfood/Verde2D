#pragma once

#include <list>

#include "Box.hpp"

namespace internal {
	void UpdateCamera();
}

constexpr static float KEEP_ZOOM = 0;

const Box&       GetCamFrustum();
const glm::vec2& GetCamPosition();
float            GetCamZoom();
void             SetCamPosition(const glm::vec2& f, float zoom, float weight);
