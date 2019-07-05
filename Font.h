#ifndef __FONT_H__
#define __FONT_H__

#include <iostream>
#include <string>
#include <vector>
#include "Object.h"
#include "BitmapAtlas.h"

#define FT_REGULAR	0	// Regular format
#define FT_BOLD		1	// Bold format
#define FT_ITALIC	2	// Italic format
#define MAX_STYLES	3	// Max amount of font styles


// This will contain font data
struct Font : public Object
{
	unsigned short				_size;	// Font size
	std::vector<BitmapAtlas*>	_bitmaps;	// The bitmap font family

											// Default constructor
	inline Font() = default;

	// Initial constructor
	inline Font(uniform shader_program, std::vector<std::string> bitmap_files, POINT bitmap_size)	// Family loading MUST BE IN ORDER!
	{
		std::string uri = "Res/Fonts/";	// Get the font uri from engine uri

		if (bitmap_files.size() <= MAX_STYLES && !bitmap_files.empty())	// If the bitmap count is equal or less than the maximum and isn't empty...
		{
			size_t first = bitmap_files[0].find("[") + 1;	// Get the first point for index subtraction
			size_t last = bitmap_files[0].find("]");	// Get the last point for index subtraction

			if (bitmap_files[0].npos != first)	// IF the file contains the bitmap size...
			{
				std::string temp_size = bitmap_files[0].substr(first, last - first);	// Get the size of font in string
				_size = std::stoi(temp_size);	// Convert temp string to int
			}

			for (unsigned int i = 0; i < bitmap_files.size(); i++)	// Iterate through each bitmap file name
			{
				if (bitmap_files[i].npos != bitmap_files[i].find("_r"))	// If a regular family has been found...
					_bitmaps.push_back(new BitmapAtlas(uri + bitmap_files[i], bitmap_size.x, bitmap_size.y, FT_REGULAR));	// Add a bitmap to the font family
				else if (bitmap_files[i].npos != bitmap_files[i].find("_b"))	// If a bold family has been found...
					_bitmaps.push_back(new BitmapAtlas(uri + bitmap_files[i], bitmap_size.x, bitmap_size.y, FT_BOLD));	// Add a bitmap to the font family
				else if (bitmap_files[i].npos != bitmap_files[i].find("_i"))	// If an italic family has been found...
					_bitmaps.push_back(new BitmapAtlas(uri + bitmap_files[i], bitmap_size.x, bitmap_size.y, FT_ITALIC));	// Add a bitmap to the font family
				else
					std::cout << "Error: Failed to import bitmap font as extension type _[] could not be found!\n";		// If no extensions were found, print out error message

				std::string index = std::to_string(i);	// Convert i to string
				_bitmaps[_bitmaps.size() - 1]->_u_texture = glGetUniformLocation(shader_program, ("font[" + index + "]").c_str());	// Get each font texture uniform location
			}
		}
		else
			std::cout << "Error: Too many bitmap atlases!\n";		// If the file count exceeds the limit, print out error message
	}

	// Deconstructor
	inline ~Font()
	{
		_bitmaps.clear();	// Delete bitmap data
	}

	// Bind the specific font style atlas
	inline void Bind(float style)
	{
		glUniform1i(_bitmaps[(unsigned int)style]->_u_texture, 0);	// Bind the texture uniform
		glActiveTexture(GL_TEXTURE0 + (unsigned int)style);	// Assign texture type
		glBindTexture(GL_TEXTURE_2D, _bitmaps[(unsigned int)style]->_texture_id);		// Bind the texture ID
	}
};

#endif