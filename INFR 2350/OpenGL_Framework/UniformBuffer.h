#pragma once
#include <GL/glew.h>
#include <MiniMath/Matrix.h>
#include <MiniMath/Vector.h>

class UniformBuffer
{
public:
	UniformBuffer();
	~UniformBuffer();

	void allocateMemory(unsigned int bytes); // Initializes the Uniform Buffer Object
	void sendMatrix(mat4 matrix, int offset);
	void sendVector(vec3 vector, int offset);
	void sendFloat(float scalar, int offset);
	void sendBool(bool boolean, int offset);
	void sendData(void* data, int size, int offset = 0);
	void bind(GLuint slot);	// Binds the uniform buffer to an active slot (similar to active texture slots)
	void destroy();

private:
	void bind();	// Binds the uniform buffer for sending data (similar to VBOs)
	void unbind();
	bool _IsInit = false;
	GLuint _uboHandle;
	GLuint _BindLocation;
};