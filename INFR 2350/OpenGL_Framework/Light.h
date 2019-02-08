#pragma once
#include "Transform.h"
#include "UniformBuffer.h"

class Light : public Transform
{
public:
	Light();

	enum LightType
	{
		Directional,
		Point,
		Spotlight
	};

	vec4 position = vec4(0.f);
	vec4 color = vec4(1.f, 1.f, 1.f, 1.f);
	vec4 direction;

	float attenConst = 1.f;
	float attenLinear = 1.f;
	float attenQuad = 1.f;
	float radius;

	LightType LightType = Point;

	void update(float dt);
	float calculateRadius();

	UniformBuffer m_pUBO;
private:
};