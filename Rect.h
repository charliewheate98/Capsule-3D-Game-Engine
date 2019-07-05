#ifndef __RECT_H__
#define __RECT_H__

#include "Globals.h"	// Get aspect ratio
#include "Vao.h"	// Get access to vao header


// This class will be used for visual asthetics in the UI category
class Rect
{
private:
	Vao * _vao;	// Vertex array object

public:
	double _w;	// Width
	double _h;	// Height

	// Default constructor
	inline Rect() = default;

	// Initial constructor
	inline Rect(double w, double h, GLfloat aspect, bool visible)
	{
		_w = w;		// Assign w
		_h = h;		// Assign h

		if (visible)	// If visible is true...
			Create(w, h, aspect);	// Create the vertex array
	}

	// Deconstructor
	inline ~Rect()
	{
		delete _vao;	// Delete the vertex buffer object
	}

	// This function will create a vao for rendering
	inline void Create(double w, double h, GLfloat aspect)
	{
		double wsc_w = w / _vp_width;	// Convert wdc to ndc for width
		double wsc_h = h / _vp_height;	// Convert wdc to ndc for height

		float vertex_position_data[4][2] = { {(GLfloat)wsc_w,  (GLfloat)wsc_h},	// Create an array of vertex positions
											{(GLfloat)wsc_w, (GLfloat)-wsc_h },
											{(GLfloat)-wsc_w, (GLfloat)-wsc_h},
											{(GLfloat)-wsc_w, (GLfloat)wsc_h } };

		float vertex_texcoordinate_data[4][2] = { {1.0f, 0.0f},		// Create an array of vertex coordinates
											{1.0f, 1.0f / aspect},
											{0.0f, 1.0f / aspect},
											{0.0f, 0.0f } };

		GLubyte vertex_index_data[6] =		{ 2, 1, 0,	// Create an array of vertex indices
											  3, 2, 0 };

		std::vector<float*>			positions(vertex_position_data, vertex_position_data + 4);	// Temp container for positions
		std::vector<float*>			texcoords(vertex_texcoordinate_data, vertex_texcoordinate_data + 4);	// Temp container for texcoords
		std::vector<unsigned int>	indices(vertex_index_data, vertex_index_data + 6);	// Assign vertex index data

		_vao = new Vao({ new Vbo(positions, 2, 0), new Vbo(texcoords, 2, 1) }, new Ebo(indices));	// Create the vertex buffer object
	}

	// This will render the rect
	inline void Render(unsigned int num_instances)
	{
		_vao->Bind();	// Bind the vertex buffer object
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, num_instances);	// Draw the vertex array
	}
};

#endif