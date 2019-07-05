#ifndef __VBO_H__
#define __VBO_H__

#include <vector>	// Get access to dynamic array
#include <glew.h>	// Get our glew variables
#include <glm/glm.hpp>	// Get glm variables


// This class will contain element buffer object data as an abstract class
class Vbo
{
private:
	GLuint					_vbo;	// Our vertex array object
	size_t					_float_ptr_offset;	// The float pointer offset
	uint16_t				_location_offset;	// The vertex location offset
	std::vector<float*>		_buffer_data;	// Our buffer data

public:
	// Default constructor
	inline Vbo() {}

	// Initial constructor
	inline Vbo(std::vector<float*> buffer_data, size_t float_ptr_offset, uint16_t location_offset)
	{
		_buffer_data = buffer_data;		// Assign buffer data
		_float_ptr_offset = float_ptr_offset;	// Assign float pointer offset
		_location_offset = location_offset;		// Assign location offset
	}

 	// Deconstructor
 	inline ~Vbo()
 	{
		_buffer_data.clear();	// Delete buffer data

		glDeleteBuffers(1, &_vbo); 	// Delete buffer object
 	}

 	// This function returns the buffer object
 	inline GLuint &GetVertexBufferObject()
 	{
 		return _vbo; 	// Return the buffer object
 	}

	// Get the vertex buffer data
	inline std::vector<float*> &GetBufferData()
	{
		return _buffer_data;	// Return the buffer data
	}

	// This function will bind our vao and vbo objects
 	inline void Create()
 	{
		size_t size = sizeof(_buffer_data[0]) * _float_ptr_offset;	// Calculate bytes
		
		glGenBuffers(1, &_vbo);		// Generate our buffer object
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);	// Bind our buffer object
		glBufferData(GL_ARRAY_BUFFER, _buffer_data.size() * size, _buffer_data[0], GL_STATIC_DRAW);		// Buffer our position data
		glEnableVertexAttribArray(_location_offset);	// Enable each vertex location attrib
		glVertexAttribPointer(_location_offset, _float_ptr_offset, GL_FLOAT, GL_FALSE, 0, (void*)0);	// Set the vertex pointer data
 	}
};

#endif