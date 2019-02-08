#include "Camera.h"
#include "ResourceManager.h"
#include "IO.h"
#include <algorithm>

Camera* activeCamera;
vec3 activeCameraPosition;

Camera::Camera()
{
	perspective(60.0f, 1.0f, 0.1f, 100.0f); 
}

Camera::Camera(ProjectionType projType)
{
	if (projType == ProjectionType::Perspective)
		perspective(60.0f, 1.0f, 0.1f, 100.0f); 
	else
		orthographic(-10, 10, -10, 10, -100, 100);
}

void Camera::perspective(float fovy, float aspect, float zNear, float zFar)
{
	m_pProjectionType = ProjectionType::Perspective;
	m_pProjection = mat4::PerspectiveProjection(
		fovy, aspect, 
		zNear, zFar);

	m_pAspectRatio = aspect;
	m_pNear = zNear;
	m_pFar = zFar;
	m_pFov.y = fovy;  
	m_pFov.x = ToDegrees(2 * atan(tan(ToRadians(m_pFov.y) * 0.5f) * aspect));
}

void Camera::orthographic(float left, float right, float bottom, float top, float zNear, float zFar)
{
	m_pProjectionType = ProjectionType::Orthographic;
	m_pProjection = mat4::OrthographicProjection(
		left, right, 
		top, bottom,
		zNear, zFar);

	m_pOrthoSize = vec4(left, right, top, bottom);
	m_pAspectRatio = (right - left) / (top - bottom);
	m_pNear = zNear;
	m_pFar = zFar;
}

mat4 Camera::getView() const
{
	return m_pViewMatrix;
}

mat4 Camera::getViewProjection() const
{
	return m_pProjection * m_pViewMatrix;
}

mat4 Camera::getProjection() const
{
	return m_pProjection;
}

mat4* Camera::getViewProjectionPtr() 
{
	return &m_pProjection;
}

void Camera::update(float dt)
{
	Transform::update(dt); // original inherited update function
	m_pViewMatrix = m_pLocalToWorld.GetFastInverse();

	objectList = ResourceManager::Transforms;
	cull();
	sort();
}

void Camera::draw()
{

}

void Camera::render()
{
	activeCamera = this;
	activeCameraPosition = getLocalToWorld().GetTranslation();
	for (Transform* object : cullList)
	{
		object->draw();
	}
}

void Camera::cull()
{
	cullList.clear();
	if (cullingActive)
	{
		if (m_pProjectionType == Perspective)
		{
			// assuming aspect ratio is bigger on X than Y
			for (Transform* object : objectList)
			{
				vec3 direction = this->m_pLocalToWorld.GetTranslation() - object->m_pLocalToWorld.GetTranslation();
				direction.Normalize();
				if (Dot(direction, this->m_pLocalToWorld.GetForward()) > cos(ToRadians(m_pFov.x * 0.5f * 1.4f)))
				{
					cullList.push_back(object);
				}
			}
		}
		else
		{
			for (Transform* object : objectList)
			{
				cullList.push_back(object);
			}
		}
	}
	else
	{
		for (Transform* object : objectList)
		{
			cullList.push_back(object);
		}
	}
}

struct
{
	bool operator()(Transform* a, Transform* b) const
	{
		return (Distance(a->getLocalToWorld().GetTranslation(), activeCameraPosition) < Distance(b->getLocalToWorld().GetTranslation(), activeCameraPosition));
	}
} customSort;

void Camera::sort()
{
	std::sort(cullList.begin(), cullList.end(), customSort);
}
