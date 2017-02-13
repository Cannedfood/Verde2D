#include "Camera.hpp"

//#include <glm/glm.hpp>
extern "C" {
#	include <memory.h>
#	include <stdio.h>
}

#include <GL/gl.h>

static Box       camBounds;
static glm::vec2 camPosition;
static float     camZoom = 10;
static float     camTransform[16];
static float     camAspectRatio = 1;
static glm::vec2 camViewport;

namespace internal {
	void UpdateCam() {
		glm::vec2 size = glm::vec2(camAspectRatio, 1) * camZoom;
		camBounds = {
			camPosition - size * .5f,
			camPosition + size * .5f
		};

		memset(camTransform, 0, sizeof(camTransform)); // integer(0) == float(0)
		camTransform[0 + 0 * 4] = 2 / size.x;
		camTransform[1 + 1 * 4] = 2 / size.y;
		camTransform[0 + 3 * 4] = -((camBounds.max.x + camBounds.min.x)/size.x);
		camTransform[1 + 3 * 4] = -((camBounds.max.y + camBounds.min.y)/size.y);
		camTransform[3 + 3 * 4] = 1;
	}

	void SetViewport(const glm::vec2& v) {
		camAspectRatio = v.x / v.y;
		glViewport(0, 0, v.x, v.y);
		camViewport = v;
	}

	const glm::vec2& GetViewport() {
		return camViewport;
	}

} /// internal


const Box&       CamBounds() {
	return camBounds;
}
const glm::vec2& CamPosition() {
	return camPosition;
}
float            CamZoom() {
	return camZoom;
}
float            CamAspect() {
	return camAspectRatio;
}

void CamPosition(const glm::vec2& v) {
	camPosition = v;
}
void CamZoom    (float f) {
	camZoom = f;
}
void CamAspect  (float w_div_h) {
	camAspectRatio = w_div_h;
}

float* CamTransform() {
	return camTransform;
}

#include <GL/gl.h>
glm::vec2 ViewportToWorld(const glm::vec2& v) {
	return (v - glm::vec2(camTransform[12], camTransform[13])) / glm::vec2(camTransform[0], camTransform[5]);
}

glm::vec2 WorldToViewport(const glm::vec2& v) {
	return v * glm::vec2(camTransform[0], camTransform[5]) + glm::vec2(camTransform[12], camTransform[13]);
}
