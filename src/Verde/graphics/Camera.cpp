#include "Camera.hpp"

//#include <glm/glm.hpp>
extern "C" {
#	include <memory.h>
#	include <stdio.h>
}

static Box       camBounds;
static glm::vec2 camPosition;
static float     camZoom = 0.1f;
static float     camTransform[16];
static float     camAspectRatio = 1;

void UpdateCam() {
	/*
	printf(
		"Camera:\n"
		"  pos:  %.2f, %.2f\n"
		"  zoom: %.2f\n"
		"  aspect: %.3f\n",
		camPosition.x, camPosition.y,
		camZoom,
		camAspectRatio
	);
	*/

	glm::vec2 size = glm::vec2(1, camAspectRatio) / camZoom;
	camBounds = {
		camPosition - size,
		camPosition + size
	};

	float xscale = 2 * camZoom * camAspectRatio;
	float yscale = 2 * camZoom;

	float projection_mat[] = {
		xscale, 0, 0, 0,
		0, yscale, 0, 0,
		0, 0, 1, 0,
		-camPosition.x * xscale, -camPosition.y * yscale, 0, 1
	};

	memcpy(camTransform, projection_mat, sizeof(camTransform));
}

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
