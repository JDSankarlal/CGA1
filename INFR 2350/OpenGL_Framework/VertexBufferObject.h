#pragma once

#include "GL/glew.h"
#include <vector>
#include "IO.h"

// Vertex Buffer Locations
enum AttributeLocations
{
	VERTEX = 0,
	TEXCOORD = 1,
	NORMAL = 2,
	COLOR = 3,
	INSTANCED_COL_0 = 12,
	INSTANCED_COL_1 = 13,
	INSTANCED_COL_2 = 14,
	INSTANCED_COL_3 = 15
};


struct VertexBufferData
{
	VertexBufferData()
	{
		numVertices = 0;
		numElements = 0;
		elementType = GL_FLOAT;
		attributeType = AttributeLocations::VERTEX;
	}


	AttributeLocations attributeType;
	GLuint numVertices;
	GLuint numElements;
	GLuint numElementsPerAttribute;
	GLuint sizeOfElement;
	GLenum elementType;

	void* data;

	// What does a VBO require?
	// Each VBO has a location AKA which VBO slot for the VAO
	// We can also keep track of what kind of data it is too
		// Along with the size of the elements: sizeof(float)
	// How many elements per attribute? 1/2/3/4?
	// Total number of vertices
	// Total number of elements?
	// Maybe we can store the type of attribute as a string too, why not?
	// the data itself

};

class VertexArrayObject
{
public:
	VertexArrayObject();
	~VertexArrayObject();

	int addVBO(VertexBufferData descriptor);

	VertexBufferData* getVboData(AttributeLocations loc);

	GLuint getVaoHandle() const;
	GLenum getPrimitiveType() const;
	GLuint getVboHandle(AttributeLocations loc) const;

	void createVAO(GLenum vboUsage = GL_STATIC_DRAW);
	void reuploadVAO();

	void draw() const;

	void bind() const;
	void unbind() const;

	void destroy();

private:
	GLuint vaoHandle; // Handle for the VAO itself
	GLenum primitiveType; // How the primitive is drawn Ex GL_TRIANGLE/GL_LINE/GL_POINT
	GLenum vboUsageType;
	// https://www.khronos.org/opengl/wiki/Primitive	//GL_TRIANGLE_STRIP/GL_LINE_STRIP
	std::vector<VertexBufferData> vboData;	// Vector for the vbo data and their respective handles
	std::vector<GLuint> vboHandles;
	// We separate the handles from the data itself so that you can reuse the same data on the CPU
	// and send it to 2 separate VAO's for instance, morpth targets with multiple keyframes
};