#pragma comment(lib, "SOIL_ext.lib")

#include "Texture.h"
#include "SOIL/SOIL.h"
#include <vector>
#include "IO.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <fstream>
float Texture::anisotropyAmount = 16.0f; 
GLenum Texture::magFilterOverride = GL_LINEAR;
GLenum Texture::minFilterOverride = GL_LINEAR_MIPMAP_LINEAR;

GLenum filterModes[] =
{
	GL_NEAREST,
	GL_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR
};

Texture::Texture(const std::string & file, bool mipmap)
{
	this->load(file, mipmap);
}

Texture::~Texture()
{
	this->unload();
}

struct RGB
{
	float r, g, b;

};

bool Texture::loadLUT(const std::string file)
{
	glEnable(GL_TEXTURE_3D);
	std::vector<RGB> LUT;
	std::string LUTPath = ("../assets/CUBE/" + file);
	std::ifstream LUTfile(LUTPath.c_str());
	//int LUTSize;

	while (!LUTfile.eof())
	{
		std::string LUTline;
		std::getline(LUTfile, LUTline);

		if (LUTline.empty()) continue;

		RGB line;
		if (sscanf_s(LUTline.c_str(), "%f %f %f", &line.r, &line.g, &line.b) == 3) LUT.push_back(line);

	}
	glGenTextures(1, &this->_TexHandle);
	glBindTexture(GL_TEXTURE_3D, this->_TexHandle);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, LUT.size(), LUT.size(), LUT.size(), 0, GL_RGB, GL_FLOAT, &LUT[0]);
	glBindTexture(GL_TEXTURE_3D, 0);
	//glDisable(GL_TEXTURE_3D);

	return true;
}
bool Texture::load(const std::string & file, bool mipmap)
{
	this->filename = "../assets/textures/" + file;

	unsigned char* textureData = SOIL_load_image((this->filename).c_str(),
		&this->sizeX, &this->sizeY, &this->channels, SOIL_LOAD_RGBA);

	if (this->sizeX == 0 || this->sizeY == 0 || this->channels == 0)
	{
		SAT_DEBUG_LOG_ERROR("TEXTURE BROKE: %s", this->filename.c_str());
		return false;
	}
	
	// If the texture is 2D, set it to be a 2D texture;
	_Target = GL_TEXTURE_2D;
	_InternalFormat = GL_RGBA8;	

	int levels = countMipMapLevels(mipmap);

	glGenTextures(1, &this->_TexHandle);
	this->bind();
	glTextureStorage2D(this->_TexHandle, levels, this->_InternalFormat, this->sizeX, this->sizeY);
	glTextureSubImage2D(this->_TexHandle, 0, // We are editing the first layer in memory (Regardless of mipmaps)
		0, 0, // No offset
		this->sizeX, this->sizeY, // the dimensions of our image loaded
		GL_RGBA, GL_UNSIGNED_BYTE, // Data format and type
		textureData); // Pointer to the texture data

	if (mipmap)
	{
		generateMipMaps();
	}

	glTextureParameteri(this->_TexHandle, GL_TEXTURE_MIN_FILTER, this->_Filter.min);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_MAG_FILTER, this->_Filter.mag);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_WRAP_S, this->_Wrap.x);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_WRAP_T, this->_Wrap.y);

	this->unbind();
	SOIL_free_image_data(textureData);
	return true;
}

bool Texture::unload()
{
	if (this->_TexHandle)
	{
		glDeleteTextures(1, &this->_TexHandle);
		return true;
	}
	return false;
}




int Texture::countMipMapLevels(bool mipmap)
{
	int levels = 1;

	if (mipmap)
	{
		float largest = static_cast<float>(max(this->sizeX, this->sizeY));
		levels += static_cast<int>(std::floor(std::log2(largest)));
	}
	return levels;
}

void Texture::generateMipMaps()
{
	glGenerateTextureMipmap(this->_TexHandle);
	glTextureParameterf(this->_TexHandle, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyAmount);
}

void Texture::createTexture(int w, int h, GLenum target, GLenum filtering, GLenum edgeBehaviour, GLenum internalFormat, GLenum textureFormat, GLenum dataType, void * newDataPtr)
{
	sizeX = w;
	sizeY = h;
	_Filter.mag = filtering;
	_Wrap.x = edgeBehaviour;
	_Wrap.y = edgeBehaviour;
	_InternalFormat = internalFormat;
	_Target = target;

	GLenum error = 0;

	// Not necessary to enable GL_TEXTURE_* in modern context.
	//	glEnable(m_pTarget);
	//	error = glGetError();

	unload();

	glGenTextures(1, &_TexHandle);
	glBindTexture(target, _TexHandle);
	error = glGetError();

	glTexParameteri(_Target, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(_Target, GL_TEXTURE_MAG_FILTER, filtering);
	glTexParameteri(_Target, GL_TEXTURE_WRAP_S, edgeBehaviour);
	glTexParameteri(_Target, GL_TEXTURE_WRAP_T, edgeBehaviour);
	error = glGetError();

	glTexImage2D(_Target, 0, internalFormat, w, h, 0, textureFormat, dataType, newDataPtr);
	error = glGetError();

	if (error != 0)
	{
		SAT_DEBUG_LOG_ERROR("[Texture.cpp : createTexture] Error when creating texture. ");
	}

	glBindTexture(_Target, 0);
}

void Texture::bind() const
{
	glBindTexture(this->_Target, this->_TexHandle);

	// Overrides for mipmap and anisotropy
	// These shouldn't be called on every bind, and can instead be set using glTextureParameter once on texture creation
	//glTexParameteri(this->_Target, GL_TEXTURE_MIN_FILTER, minFilterOverride);
	//glTexParameteri(this->_Target, GL_TEXTURE_MAG_FILTER, magFilterOverride);	
	//glTexParameterf(this->_Target, GL_TEXTURE_MAX_ANISOTROPY_EXT, Texture::anisotropyAmount);
}

void Texture::bind(int textureSlot) const
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	this->bind();
}

void Texture::unbind() const
{
	glBindTexture(this->_Target, GL_NONE);
}

void Texture::unbind(int textureSlot) const
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	this->unbind();
}

GLuint Texture::getID()
{
	return _TexHandle;
}

void Texture::sendTexParameters()
{
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_MIN_FILTER, this->_Filter.min);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_MAG_FILTER, this->_Filter.mag);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_WRAP_S, this->_Wrap.x);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_WRAP_T, this->_Wrap.y);
}

void Texture::setFilterParameters(GLenum mag, GLenum min)
{
	_Filter.mag = mag;
	_Filter.min = min;
}

void Texture::setWrapParameters(GLenum wrap)
{
	_Wrap.x = wrap;
	_Wrap.y = wrap;
	_Wrap.z = wrap;
}

TextureWrap::TextureWrap()
{
	x = GL_REPEAT;
	y = GL_REPEAT;
	z = GL_REPEAT;
}
