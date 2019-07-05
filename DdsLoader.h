#ifndef __DDS_LOADER_H__
#define __DDS_LOADER_H__

#include <glew.h>

#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844



// This function imports a dds file and returns the dds data as a struct
inline GLuint LoadDds(std::vector<std::string> file, size_t &img_width, size_t &img_height, size_t &num_mips, GLvoid* data, GLint wrap_s, GLint wrap_t, GLint min_filter, GLint mag_filter, size_t texture_type, bool anistropic_filtering)
{

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(texture_type, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned int i = 0; i < file.size(); i++)	// For each image...
	{
		FILE *fp;

		unsigned char header[124];
		unsigned int height;
		unsigned int width;
		unsigned int linearSize;
		unsigned int mipMapCount;
		unsigned int fourCC;
		unsigned int components;
		unsigned int format;
		unsigned int bufsize;
		unsigned char* buffer;


		/* try to open the file */
		errno_t err;
		err = fopen_s(&fp, file[i].c_str(), "rb");
		if (fp == NULL)
			return 0;

		/* verify the type of file */
		char filecode[4];
		fread(filecode, 1, 4, fp);
		if (strncmp(filecode, "DDS ", 4) != 0)
		{
			fclose(fp);
			return 0;
		}

		/* get the surface desc */
		fread(&header, 124, 1, fp);

		height = *(unsigned int*)&(header[8]);
		width = *(unsigned int*)&(header[12]);
		linearSize = *(unsigned int*)&(header[16]);
		mipMapCount = *(unsigned int*)&(header[24]);
		fourCC = *(unsigned int*)&(header[80]);
		bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
		buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));

		fread(buffer, 1, bufsize, fp);

		/* close the file pointer */
		fclose(fp);

		components = (fourCC == FOURCC_DXT1) ? 3 : 4;
		switch (fourCC)
		{
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			free(buffer);
			return 0;
		}

		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

		unsigned int offset = 0;
		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
		{
			unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
			glCompressedTexImage2D(texture_type != GL_TEXTURE_CUBE_MAP ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, format, width, height,
				0, size, buffer + offset);

			if ((level < 1) && (i < 1))		// Only assign input variable values from first image
			{
				img_width = width;	// Assign texture width
				img_height = height;	// Assign texture height
				data = buffer;	// Assign buffer data
				num_mips = mipMapCount;		// Assign number of mips
			}

			offset += size;
			width /= 2;
			height /= 2;
		}

		if (anistropic_filtering)	// If anistropic_filtering is true...
		{
			GLfloat f_largest;	// A contianer for storing the amount of texels in view for anistropic filtering
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f_largest);		// Query the amount of texels for calculation
			glTexParameterf(texture_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, f_largest);	// Apply filter to texture
		}

		if (mipMapCount <= 0)
			glGenerateMipmap(texture_type);	// Generate mipmap

		free(buffer);	// Free buffers from memory
	}

	// Parameters
	glTexParameteri(texture_type, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(texture_type, GL_TEXTURE_MAX_LEVEL, 9);
	glTexParameteri(texture_type, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, wrap_t);

	// Set additional cubemap parameters
	if (texture_type == GL_TEXTURE_CUBE_MAP)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_s);

	return textureID;	// Return texture id
}

#endif