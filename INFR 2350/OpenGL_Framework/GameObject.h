#pragma once

#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include <vector>

class GameObject : public Transform
{
public:
	GameObject();
	GameObject(const std::string &meshFile, const std::string &textureFile); // Not defined
	GameObject(const std::string &meshFile, const std::vector <std::string> &textureFiles); // Not defined
	GameObject(Mesh* _mesh, Texture *_texture);
	GameObject(Mesh* _mesh, std::vector <Texture*>& _textures);
	~GameObject();

	void setMesh(Mesh* _mesh);
	void setTexture(Texture* _texture);
	void setTextures(std::vector <Texture*>& _textures);
	void setShaderProgram(ShaderProgram* _shaderProgram);
	void draw();

private:
	Mesh* mesh;
	std::vector<Texture*> textures;
	ShaderProgram* material;
};