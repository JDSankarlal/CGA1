#include "ResourceManager.h"

std::vector<Transform*> ResourceManager::Transforms;
std::vector<ShaderProgram*> ResourceManager::Shaders;

void ResourceManager::addEntity(Transform * entity)
{
	Transforms.push_back(entity);
}

void ResourceManager::addShader(ShaderProgram * shader)
{
	Shaders.push_back(shader);
}
