#ifndef __VAO_H__
#define __VAO_H__

#include "Ebo.h"	// Include the element buffer object header
#include "Vbo.h"	// Include the vertex buffer object header


// This class will store a vertex array of vertex buffer object data
class Vao
{
private:
	GLuint				_vao;	// Vertex array object
	size_t				_num_attribs;	// The number of vertex attributes
	Ebo*				_ebo_data;	// Element buffer object data
	std::vector<Vbo*>	_vbo_data;	// Vertex buffer object data

public:
	// Default constructor
	inline Vao() : _num_attribs(0) {}

	// Initial constructor
	inline Vao(std::vector<Vbo*> vbo_data, Ebo* ebo_data)
	{
		_vbo_data = vbo_data;	// Assign vertex buffer object data
		_ebo_data = ebo_data;	// Assign element object data
		_num_attribs = vbo_data.size();	// Initialise num attribs

		Create(vbo_data);	// Create the vao
	}

	// Deconstructor
	inline ~Vao()
	{
		unsigned int i;		// Temp index variable
		for (i = 0; i < _num_attribs; i++) 	// Iterate through each attrib...
			glDisableVertexAttribArray(i); 	// Disable each vertex attribute array

		_vbo_data.clear();	// Delete all vbos

		if (_ebo_data)	// If the ebo is not NULL
			delete _ebo_data;	// Delete the object

		glDeleteVertexArrays(1, &_vao); 	// Delete our vertex array object
	}

	// Get the number of attributes
	inline size_t &GetNumAttribs()
	{
		return _num_attribs;	// Return the number of attributes
	}

	// This function returns the vertex array object
	inline GLuint &GetVertexArrayObject()
	{
		return _vao; 	// Return the vertex array object
	}

	// Get vertex buffer data
	inline std::vector<Vbo*> &GetVertexBufferData()
	{
		return _vbo_data;	// Return the vertex buffer data
	}

	// Create the vertex array object
	inline void Create(std::vector<Vbo*> vbo_data)
	{
		glGenVertexArrays(1, &_vao);	// Generate a vao to store our ebo
		glBindVertexArray(_vao);	// Bind our vertex array

		unsigned int i;		// Teno index variable
		for (i = 0; i < _num_attribs; i++)
			_vbo_data[i]->Create();		// Generate the vbo data

		if (_ebo_data)	// If we're using an ebo...
			_ebo_data->Create();	// Generate the ebo

		glVertexAttribDivisor(_num_attribs, 1);	// Update 3 attribs for each iteration call
	}

	// This function binds the vertex array object
	inline void Bind()
	{
		glBindVertexArray(_vao); 	// Bind our vertex array object
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo_data->GetElementBufferObject()); 	// Bind our element buffer object_ebo_data->Bind();	// Bind our element buffer object
	}
};

#endif