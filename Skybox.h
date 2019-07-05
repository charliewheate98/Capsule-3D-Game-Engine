#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <vector>	// Dynamic arrays
#include "Mesh.h"	// Include the deriving class


// This class will store and render a skybox
class Skybox : public Mesh
{
public:
	// Default constructor
	inline Skybox() { _mt = M_STATIC; }

	// Initial constructor
	inline Skybox(GLuint shader_program, std::vector<Material*> mats)
	{
		_mt = M_STATIC;		// Assign mesh type

		if (mats.empty())	// If the cubemap is NOT initialised...
		{
			std::cout << "Error: Failed to assign material - material is NULL!\n";	// Print error message
			return;		// Return error
		}

		_mats = mats;	// Assign material

		Create();	// Create the vertex array object
		_trans._sca = glm::vec3((GLfloat)CAMERA_FAR / 2.0f);	// Assign skybox scale
		UpdateModel();	// Initialise model matrix
		
		_u_mod = glGetUniformLocation(shader_program, "mod");	// Get our model matrix uniform
	}

	// This function will create the skybox vao
	inline void Create()
	{
		float vertex_position_data[24][3] = {	// Create vertex position array
			{ -1, -1, 1 },
			{ -1, 1, 1 },
			{ 1, -1, 1 },
			{ 1, 1, 1 },
			{ -1, 1, 1 },
			{ -1, 1, -1 },

			{ 1, 1, 1 },
			{ 1, 1, -1 },
			{ -1, 1, -1 },
			{ -1, -1, -1 },
			{ 1, 1, -1 },
			{ 1, -1, -1 },

			{ -1, -1, -1 },
			{ -1, -1, 1 },
			{ 1, -1, -1 },
			{ 1, -1, 1 },
			{ 1, -1, 1 },
			{ 1, 1, 1 },

			{ 1, -1, -1 },
			{ 1, 1, -1 },
			{ -1, -1, -1 },
			{ -1, 1, -1 },
			{ -1, -1, 1 },
			{ -1, 1, 1 }
		};

		GLubyte vertex_index_data[36] = {	// Create an array of vertex indices
			0, 1, 2,	
			2, 1, 3,
			4, 5, 6,
			6, 5, 7,
			8, 9, 10,
			10, 9, 11,
			12, 13, 14,
			14, 13, 15,
			16, 17, 18,
			18, 17, 19,
			20, 21, 22,
			22, 21, 23
		};

		std::vector<float*>			positions(vertex_position_data, vertex_position_data + 24);		// Assign position vertex data
		std::vector<unsigned int>	indices(vertex_index_data, vertex_index_data + 36);		// Assign index vertex data

		_vao = new Vao({ new Vbo(positions, 3, 0), new Vbo(positions, 3, 1) }, new Ebo(indices));		// Initialise vao
	}

	// Virtual functions
	inline virtual void Update(double &delta) {}
	inline virtual void Render()
	{
		glUniformMatrix4fv(_u_mod, 1, GL_FALSE, glm::value_ptr(_trans._mod));	// Bind our uniform data

		_mats[0]->Bind();	// Bind the material
		_vao->Bind();	// Bind vertex buffer object

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);	// Draw geometry to current buffer
	}
};

#endif