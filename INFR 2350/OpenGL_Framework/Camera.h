#pragma once
#include "Transform.h"
#include "Framebuffer.h"
#include <vector>

enum ProjectionType
{
	Perspective,
	Orthographic
};

class Camera : public Transform
{
public:	
	Camera();
	Camera(ProjectionType projType);

	void perspective(
		float fovy, float aspect, 
		float zNear, float zFar);

	void orthographic(
		float left, float right,
		float bottom, float top,
		float zNear, float zFar);

	void Camera::attachFrameBuffer(Framebuffer * fb)
	{
		m_pFB = fb;
	}

	mat4 getView() const;
	mat4 getViewProjection() const;
	mat4 getProjection() const;
	mat4* getViewProjectionPtr();
	void update(float dt);
	void draw();
	void render();

	void cull();
	void sort();

	bool cullingActive = false;
private:
	mat4 m_pProjection;
	mat4 m_pViewMatrix;
		
	vec4 m_pOrthoSize;
	vec2 m_pFov; 
	float m_pAspectRatio;
	float m_pNear;
	float m_pFar;
	ProjectionType m_pProjectionType = ProjectionType::Perspective;

	std::vector<Transform*> objectList;
	std::vector<Transform*> cullList;
	Framebuffer* m_pFB;
};