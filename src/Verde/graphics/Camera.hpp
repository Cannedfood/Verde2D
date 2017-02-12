#pragma once

#include "../Box.hpp"

#include <glm/vec2.hpp>

void UpdateCam();

const Box&       CamBounds();
const glm::vec2& CamPosition();
float            CamZoom();
float            CamAspect();

void CamPosition(const glm::vec2& v);
void CamZoom    (float f);
void CamAspect  (float w_div_h);

/// Returns a 4x4 matrix
float* CamTransform();

glm::vec2 ViewportToWorld(const glm::vec2& v);
glm::vec2 WorldToViewport(const glm::vec2& v);
