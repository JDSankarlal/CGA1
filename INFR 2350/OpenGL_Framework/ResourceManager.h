#pragma once
#include "GameObject.h"

class ResourceManager
{
public:
	static void addEntity(Transform* entity);
	static void addShader(ShaderProgram* shader);

	static std::vector<ShaderProgram*> Shaders;
	static std::vector<Transform*> Transforms;
};