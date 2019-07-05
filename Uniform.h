#ifndef __UNIFORMS_H__
#define __UNIFORMS_H__

#include <vector>	// For creating dynamic arrays
#include <glew.h>	// Include GLEW for extensions

typedef GLuint uniform;		// Define a singular uniform variable

// A class to store a uniform block (an array of uniform values)
struct UniformBlock 
{
	std::vector<uniform>	_uniform;	// A dynamic array for storing a uniform block

	// Default constructor
	inline UniformBlock() {}

	// Initial constructor
	inline UniformBlock(std::vector<uniform> uniform)
	{
		_uniform = uniform;		// Assign raw value to "value"
	}
};

// This class stores an array of uniform blocks
struct UniformBlockArray
{
	// This will store an array of uniform arrays
	std::vector<UniformBlock> _data;

	// Default constructor
	inline UniformBlockArray() {}

	// Initial constructor 1
	inline UniformBlockArray(std::vector<UniformBlock> data)
	{
		_data = data;	// Assign raw value to data
	}
};

#endif