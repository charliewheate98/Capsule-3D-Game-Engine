#ifndef __EBO_H__
#define __EBO_H__

#include <vector>	// Dynamic arrays
#include <glew.h>	// OpenGL functions and variables


// This class will store element buffer object data from other buffers
class Ebo
{
private:
	GLuint						_ebo;	// Element buffer object
	std::vector<unsigned int>	_index_data;	// Element buffer object data

public:
	// Default constructor
	inline Ebo() {}

	// Initial constructor
	inline Ebo(std::vector<unsigned int> index_data)
	{
		_index_data = index_data;		// Assign index data
	}

	// Deconstructor
	inline ~Ebo()
	{
		glDeleteBuffers(1, &_ebo);	// Delete the element buffer object
	}

	// Get the element buffer object
	inline GLuint &GetElementBufferObject()
	{
		return _ebo;	// Return element buffer object
	}

	// Get the index data
	inline std::vector<unsigned int> &GetIndexData()
	{
		return _index_data;		// Return the index data
	}

	// Function for creating ebo
	inline void Create()
	{
		glGenBuffers(1, &_ebo);	// Generate our ebo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);	// Bind our ebo
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _index_data.size() * sizeof(unsigned int), &_index_data[0], GL_STATIC_DRAW);	// Buffer our ebo data
	}
};

#endif 