#pragma once

#include <MiniMath/Core.h>
#include <vector>
#include <string>

class Transform
{
public:
	vec3 m_pLocalScale;
	vec3 m_pLocalRotationEuler;
	vec3 m_pLocalPosition;

	mat4 m_pLocalRotation;
	
	mat4 m_pLocalTransform;
	mat4 m_pLocalToWorld;

	Transform* m_pParent;
	std::vector<Transform*> m_pChildren;

	Transform();
	~Transform();

	void addChild(Transform* newChild);
	void removeChild(Transform* ripChild);

	void setLocalPos(vec3 newPosiiton); // Local Position
	void setLocalRotX(float newAngle); // Local Rotation X
	void setLocalRotY(float newAngle); // Local Rotation Y
	void setLocalRotZ(float newAngle); // Local Rotation Z
	void setLocalRot(vec3 newAngle); // Local Rotation
	void setScale(vec3 newScale); // Local Scale
	void setScale(float newScale); // Local Scale

	mat4 getLocalToWorld(); // Get Local to World Matrix
	vec3 getWorldPos(); // Get World Position
	mat4 getWorldRot(); // Get World Rotation
	vec3 getLocalScale(); // Get Local Scale
	vec3 getLocalPos(); // Get Local Position
	mat4 getLocalRot(); // Get Local Rotation

	std::vector<Transform*> getChildren();
	Transform* getParent();

	virtual void update(float dt);	
	virtual void draw();

protected:
	// Other Properties
	std::string name;
	vec4 color; 
};