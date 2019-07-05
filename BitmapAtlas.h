#ifndef __BITMAP_ATLAS_H__
#define __BITMAP_ATLAS_H__

#define BITMAP_X_COUNT	16	// The default number of chars on each row
#define ASCII_OFFSET	32	// The subtracting offset from ascii values

#include "DdsLoader.h"	// For loading in dds files
#include "Uniform.h"	// For our atlas texture maps

// This will contain bitmap data
struct BitmapAtlas
{
	double			_width;		// Width of bitmap
	double			_height;	// Height of bitmap
	double			_element_size;	// The size of each atlas element
	unsigned int	_type;	// Font type
	GLuint			_texture_id;	// The texture ID
	uniform			_u_texture;		// The uniform location variable
	std::string		_name;	// The name of the bitmap

							// Default constructor
	inline BitmapAtlas() = default;

	// Initial constructor
	inline BitmapAtlas(std::string bitmap, unsigned int width, unsigned int height, unsigned int type)
	{		
		_width = width;	// Assign bitmap width
		_height = height;	// Assign bitmap height
		_type = type;	// Assign the font type

		LoadFile(bitmap);	// Load the bitmap file

		_element_size = (unsigned int)_width / BITMAP_X_COUNT;	// Calculate the element size in pixels
	}

	// Deconstructor
	inline ~BitmapAtlas()
	{
		glDeleteTextures(1, &_texture_id);	// Delete the bitmap texture
	}

	// This function will load a bitmap font file
	inline void LoadFile(std::string bitmap)
	{
		_name = bitmap;		// Assign bitmap file name to bitmap name

		for (unsigned int i = 0; i < 4; i++)	// Iterate four times...
			_name.pop_back();	// Remove file extension from name

		size_t w, h, m;		// Temp input variables
		_texture_id = LoadDds({ bitmap }, w, h, m, 0, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_TEXTURE_2D, false);	// Load bitmap as dds format
	}
};

#endif