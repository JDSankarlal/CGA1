#pragma once
#include <string>
#include "MiniMath/Core.h"
#include "GL\glew.h"
#include <vector>

class Camera;
class Transform;

class ShaderProgram
{
public: 
	ShaderProgram();
	ShaderProgram(const std::string &vertFile, const std::string &fragFile);
	~ShaderProgram();

	static bool initDefault();
	void setDefault();

	bool load(const std::string &vertFile, const std::string &fragFile);
	bool reload();
	bool isLoaded() const;
	void unload();
	bool linkProgram();

	void bind() const;
	static void unbind();
	void bindUBO(const std::string & uniformBlockName, unsigned int bindSlot) const;

	GLint getUniformLocation(const std::string &uniformName) const;

	void sendUniform(const std::string &name, const float scalar) const;
	void sendUniform(const std::string &name, const vec3 &vector) const;
	void sendUniform(const std::string &name, const vec4 &vector) const;
	void sendUniform(const std::string &name, const mat4 &matrix, bool transpose = false) const;
	void sendUniformCamera(Camera *camera);

private: 
	bool _IsInit = false;
	GLuint _VertShader = 0;
	GLuint _FragShader = 0;
	GLuint _Program = 0;

	std::string _VertFilename;
	std::string _FragFilename;

	static std::string _ShaderDirectory;

	static bool _IsInitDefault;
	static GLuint _VertShaderDefault;
	static GLuint _FragShaderDefault;
	static GLuint _ProgramDefault;

	bool compileShader(GLuint shader) const;
	void outputShaderLog(GLuint shader) const;
	void outputProgramLog() const;
};