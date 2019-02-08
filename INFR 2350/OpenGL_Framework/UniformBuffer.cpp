#include "UniformBuffer.h"
#include <assert.h>

UniformBuffer::UniformBuffer()
{

}

UniformBuffer::~UniformBuffer()
{
	destroy();
}

void UniformBuffer::allocateMemory(unsigned int bytes)
{
	if (_uboHandle)
	{
		destroy();
	}
	glGenBuffers(1, &_uboHandle);
	bind();
	glBufferData(GL_UNIFORM_BUFFER, bytes, GL_NONE, GL_DYNAMIC_DRAW);
	unbind();
	_IsInit = true;
}

void UniformBuffer::sendMatrix(mat4 matrix, int offset)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(mat4), &matrix);
	unbind();
}

void UniformBuffer::sendVector(vec3 vector, int offset)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(vec3), &vector);
	unbind();
}

void UniformBuffer::sendFloat(float scalar, int offset)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &scalar);
	unbind();
}

void UniformBuffer::sendBool(bool boolean, int offset)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(bool), &boolean);
	unbind();
}

void UniformBuffer::sendData(void * data, int size, int offset)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	unbind();
}

void UniformBuffer::bind(GLuint slot)
{
	assert(_IsInit);

	glBindBufferBase(GL_UNIFORM_BUFFER, slot, _uboHandle);
	_BindLocation = slot;
}

void UniformBuffer::destroy()
{
	if (_uboHandle)
	{
		glDeleteBuffers(1, &_uboHandle);
	}
}

void UniformBuffer::bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, _uboHandle);
}

void UniformBuffer::unbind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
}
