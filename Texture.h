#ifndef __TEXTURE_2D_URI__
#define __TEXTURE_2D_URI__ ((char*)"Res/Game/Textures/")
#endif

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Object.h"		// Get object class
#include "DdsLoader.h"	// Get dds loader
#include <stb_image.h>

#define NORMAL			0	// Texture mep index definition
#define ALBEDO			1	// Texture mep index definition
#define SPECROUGH		2	// Texture map index definition
#define METALIC			3	// Texture map index definition
#define EMISSIVE		4	// Texture map index definition
#define CUBEMAP			5	// Texture map index definition

// This namespace will store all the functions and data structures for all texture types
namespace Texture
{
	// This function creates the texture object
	static inline GLuint Create(size_t t, size_t w, size_t h, std::vector<GLubyte**> d)
	{
		GLuint id;	// Create our texture id

		glGenTextures(1, &id);	// Generate a texture

		glBindTexture(t == GL_TEXTURE_CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, id);	// Bind the texture id

		glTexParameteri(t == GL_TEXTURE_CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Assign min value
		glTexParameteri(t == GL_TEXTURE_CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Assign mag value
		glTexParameteri(t == GL_TEXTURE_CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Assign wrap s value
		glTexParameteri(t == GL_TEXTURE_CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// Assign wrap t value

		if (t == GL_TEXTURE_CUBE_MAP)	// If the type is equal to cube map index...
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);		// Set R component for texture's third dimension

			for (size_t i = 0; i < 6; i++)	// For each orientation...
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, d[i]);	// Store cube map pixel data to memory

			return id;		// Return after all buffer data has been stored to memory
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, d[0]);	// Otherwise store 2d texture pixel data to memory

		return id;	// Return single texture id
	}

	// This will be our abstract texture map class
	struct TextureBase : public Object
	{
		bool		type_2d;	// Sampler type
		GLuint		id;		// The texture object id
		GLenum		unit;	// The texture unit
		size_t		width;	// Texture width
		size_t		height;		// Texture height
		size_t		num_mips;	// Number of mip maps
		GLint		wrap_s;		// Wrap s filter
		GLint		wrap_t;		// Wrap t filter
		GLint		min_filter;		// Min filter
		GLint		mag_filter;		// Mag filter
		GLubyte**	data;	// Texture data

							// Deconstructor
		inline ~TextureBase()
		{
			glDeleteTextures(1, &id);	// Delete texture object
			if (data) delete data;	// Delete buffer data
		}

		// Parse virtual voids
		inline virtual void Update(double delta) {}
		inline virtual void Render() {}
	};

	// This class will store an opengl texture map object
	struct Texture2d : public TextureBase
	{
		// Defaut constructor
		inline Texture2d() = default;

		// Initial constructor 0
		inline Texture2d(GLenum mt, size_t w, size_t h, GLubyte** d)
		{
			if (!w || !h)	// If width and height are equal to zero
			{
				std::cout << "Error: Failed to create texture - size is equal to null!\n";	// Print error message
				return;		// Return
			}

			type_2d = true;	// Assign type to sampler2d
			SetName("texture");	// Set default texture name to file name

			unit = mt;	// Assign texture unit	

			width = w;	// Assign width
			height = h;		// Assign height
			data = d;	// Assign data
			num_mips = 0;	// Assign mip count
			wrap_s = GL_REPEAT;	// Assign wrap s filter
			wrap_t = GL_REPEAT;	// Assign wrap t filter
			min_filter = GL_LINEAR;		// Assign min filter
			mag_filter = GL_LINEAR;		// Assign mag filter

			id = Create(mt, w, h, { d });	// Create new texture object
		}

		// Initial constructor 1
		inline Texture2d(std::string file, GLenum m_type, GLint wrap_filter, GLint min_mag_filter)
		{
			SetName(file + "-t2d");	// Set texture name to file name
			type_2d = true;	// Assign type to sampler2d

			unit = m_type;	// Assign texture unit

			wrap_s = wrap_filter;	// Assign wrap s filter
			wrap_t = wrap_filter;	// Assign wrap t filter
			min_filter = min_mag_filter;	// Assign min filter
			mag_filter = min_mag_filter;	// Assign mag filter

			id = LoadDds({ static_cast<std::string>(__TEXTURE_2D_URI__) + file }, 
				width, height, num_mips, data, 
				wrap_filter, wrap_filter, 
				GL_LINEAR_MIPMAP_LINEAR, min_mag_filter, GL_TEXTURE_2D, true);	// Load dds file and store texture data
		}

		// Update virtual void
		inline virtual void Update(double delta) {}

		// This function will bind the texture object
		inline virtual void Render()
		{
			glActiveTexture(GL_TEXTURE0 + unit);	// Set active texture type
			glBindTexture(GL_TEXTURE_2D, id);	// Bind the texture object
		}
	};

	// This class will handle all of the cube map data into memory
	struct TextureCubemap : public TextureBase
	{
		// Defaut constructor
		inline TextureCubemap() = default;

		// Initial constructor 0
		inline TextureCubemap(size_t w, size_t h, std::vector<GLubyte**> d)
		{
			if (!w || !h)	// If width and height are equal to zero
			{
				std::cout << "Error: Failed to create texture cubemap - size is equal to null!\n";	// Print error message
				return;		// Return
			}

			if (d.size() != 6)	// If there aren't enough buffers
			{
				std::cout << "Error: Failed to create texture cubemap - not enough buffers to sample!\n";	// Print error message
				return;		// Return
			}

			SetName("cubetexture");	// Set default texture name to file name
			type_2d = false;	// Assign type to samplerCube

			unit = CUBEMAP;	// Assign texture unit

			width = w;	// Assign width
			height = h;		// Assign height
			num_mips = 0;	// Assign mip count
			wrap_s = GL_CLAMP_TO_EDGE;	// Assign wrap s filter
			wrap_t = GL_CLAMP_TO_EDGE;	// Assign wrap t filter
			min_filter = GL_LINEAR_MIPMAP_LINEAR;		// Assign min filter
			mag_filter = GL_LINEAR;		// Assign mag filter

			id = Create(GL_TEXTURE_CUBE_MAP, w, h, d);	// Create textures object
		}
	
		unsigned int loadCubemap(std::vector<std::string> faces)
		{
			unsigned int textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

			int width, height, nrComponents;
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			return textureID;
		}

		// generate a empty cubemap texture with no textures within it 
		unsigned int GenerateEmptyCubemap(GLsizei width, GLsizei height, bool mipmap = false)
		{
			unsigned int textureID;

			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

			for (unsigned int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			mipmap == true ? glGenerateMipmap(GL_TEXTURE_CUBE_MAP) : 0;		

			return textureID;
		}

		// Initial constructor 1
		inline TextureCubemap(std::vector<std::string> files)
		{
			SetName(files[0] + "-cm");	// Set default texture name to file name
			type_2d = false;	// Assign type to samplerCube

			unit = CUBEMAP;	// Assign texture unit

			wrap_s = GL_CLAMP_TO_EDGE;	// Assign wrap s filter
			wrap_t = GL_CLAMP_TO_EDGE;	// Assign wrap t filter
			min_filter = GL_LINEAR_MIPMAP_LINEAR;		// Assign min filter
			mag_filter = GL_LINEAR;		// Assign mag filter

			id = loadCubemap(files);// LoadDds(files, width, height, num_mips, 0, wrap_s, wrap_t, min_filter, mag_filter, GL_TEXTURE_CUBE_MAP, true);	// Load dds files and store texture data
		}

		// Update virtual void
		inline virtual void Update(double delta) {}

		// This function will bind the texture object
		inline virtual void Render()
		{
			glActiveTexture(GL_TEXTURE0 + unit);	// Set active texture type
			glBindTexture(GL_TEXTURE_CUBE_MAP, id);	// Bind the texture object
		}
	};

	struct TextureHDR : public TextureBase
	{
		inline TextureHDR() = default;

		inline TextureHDR(const char* file)
		{
			int img_width;
			int img_height;
			int num_components;

			float * img_data = stbi_loadf(file, &img_width, &img_height, &num_components, 0);

			if (img_data)
			{
				glGenTextures(1, &id);
				glBindTexture(GL_TEXTURE_2D, id);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, img_width, img_height, 0, GL_RGB, GL_FLOAT, img_data);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(img_data);
			}
		}

		inline virtual void Update(double delta) {}
		inline virtual void Render()
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, id);
		}
	};
}

#endif