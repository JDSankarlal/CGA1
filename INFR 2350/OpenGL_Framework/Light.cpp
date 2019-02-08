#include "Light.h"

Light::Light()
{
	m_pUBO.allocateMemory(sizeof(vec4) * 3 + sizeof(float) * 4);
}

void Light::update(float dt)
{
	Transform::update(dt);
	//position = vec4(getWorldPos(), 0.f);
	//direction = vec4(getWorldRot().GetForward(), 0.f);
	calculateRadius();
	m_pUBO.sendData(&position, sizeof(vec4) * 3 + sizeof(float) * 4, 0);
}

float Light::calculateRadius()
{
	float luminance = Dot((vec3(color)/color.w), vec3(0.3f, 0.59f, 0.11f));
	float luminanceMin = 0.05f;

	//use quadratic formula to find radius
	radius = (-attenLinear + sqrtf(attenLinear * attenLinear - 4.0f * attenQuad * (attenConst - luminance/luminanceMin))) / (2.0f * attenQuad);

	return radius;
}