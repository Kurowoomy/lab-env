#pragma once
#include "core/Math_Library.h"

class PointLight {
public:
	Vec3 position = Vec3(0, 0, 10);
	Vec3 color = Vec3(1, 1, 1);
	float intensity = 0.4;

	Matrix4 transform;
};