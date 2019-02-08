#pragma once
#include "GL/glew.h"
#include <string>
#include "Texture.h"

struct TextureFilter
{
	GLenum mag = GL_LINEAR;
	GLenum min = GL_LINEAR_MIPMAP_LINEAR;
};

GLenum filterModes[];

#pragma warning(push)
#pragma warning(disable : 4201) // Nameless struct warning
struct TextureWrap
{
	TextureWrap();

	union
	{
		struct
		{
			GLenum x, y, z;
		}; 
		struct
		{
			GLenum s, t, r;
		};
	};
};
#pragma warning(pop)

class Texture
{
public:
	Texture() = default;
	Texture(const std::string &file, bool mipmap = true);
	~Texture();

	// Loads the texture
	bool load(const std::string &file, bool mipmap = true);
	bool loadLUT(const std::string file);
	// Unloads the texture
	bool unload();

	// Description:
	// Creates the texture, allocates memory and uploads data to GPU
	// If you do not want to upload data to the GPU pass in a nullptr for the dataPtr.
	// For a description on filtering and edgeBehaviour see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
	// For a description on internalFormat, textureFormat and dataType see https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
	void createTexture(int w, int h, GLenum target, GLenum filtering, GLenum edgeBehaviour, GLenum internalFormat, GLenum textureFormat, GLenum dataType, void* newDataPtr);
	
	// Binds the texture to the last active slot
	void bind() const;
	// Binds the texture to a specific slot
	void bind(int textureSlot) const;
	// Unbinds the last active texture slot
	void unbind() const;
	// Unbinds a specific texture slot
	void unbind(int textureSlot) const;
	GLuint getID();

	void sendTexParameters();
	void setFilterParameters(GLenum mag, GLenum min);
	void setWrapParameters(GLenum wrap);

	static GLenum minFilterOverride;
	static GLenum magFilterOverride;
	static float anisotropyAmount;

	int countMipMapLevels(bool mipmap);
	void generateMipMaps();
	GLuint _TexHandle;

	std::string filename;
	std::string filetype;

	GLenum _InternalFormat;
	GLenum _Target;
	TextureFilter _Filter;
	TextureWrap _Wrap;
		
	int sizeX;
	int sizeY;
	int channels;
};