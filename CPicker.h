// Marco definitions
#ifndef __CPICKER_H__
#define __CPICKER_H__

/*
* Includes needed
*/
#include "PostProcessing.h" // Get bloom pass
#include "Manipulators.h"

/*
* CPicker: this class handles assign different colour IDs to meshes and
* getting the selected mesh based on colour
*/
class CPicker
{
public:
	// this fbo stores all the objects rendered in there colour ID
	Fbo*			_colour_fbo;
	Fbo*			_gizmo_colour_fbo;

	int				_selected_id;

	bool picked;
	bool translation;

	// This is the shader program that handles assigning a solid colour to a mesh
	unsigned int	_shader_program;

	// Uniforms
	GLuint			_loc_MVP; // Model, view, projection matrix
	GLuint			_loc_colourID; // RGB colourID uniform

public:
	/*
	* Constructer
	*/
	inline CPicker(unsigned int shader_program)
	{
		picked = false;
		translation = false;

		// initialise the shader program to the one in the parameters
		_shader_program = shader_program;

		// initialise the colour picking Frame buffer object
		_colour_fbo = new Fbo(1920, 1080, { new FboAttachment(1920, 1080, GL_RGB, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, false);
		_gizmo_colour_fbo = new Fbo(1920, 1080, { new FboAttachment(1920, 1080, GL_RGB, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, false);

		// Load in the uniforms
		_loc_MVP = glGetUniformLocation(shader_program, "MVP");
		_loc_colourID = glGetUniformLocation(shader_program, "colour_id");

		_selected_id = -1;	// Set selected id to minus one by default

		Manipulators::Create(shader_program);
	}

	/*
	* Destructer
	*/
	inline ~CPicker()
	{
		// delete the colour FBO from memory 
		delete _colour_fbo;

		// delete the colourID shader
		glDeleteShader(_shader_program);
	}

	/*
	* Update the colour picking every frame to check if a object has been selected or detected.
	*/
	inline void Render()
	{
		{
			// Bind the frame buffer object
			_colour_fbo->Bind();

			// clear the screen before rendering to the fbo
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (Mouse::IsLeftClick())	// If control button is down
			{
				// Loop through all the actors
				for (unsigned int i = 0; i < Content::_map->GetActors().size(); i++)
				{
					// each object needs a unqiue colour
					int r = (i & 0x000000FF) >> 0;
					int g = (i & 0x0000FF00) >> 8;
					int b = (i & 0x00FF0000) >> 16;

					glm::mat4 model_matrix = Content::_map->GetActors()[i]->GetModelMatrix();

					// the final model view projection matrix
					glm::mat4 MVP = Content::_map->GetCamera()->GetProjectionMatrix() * 
						Content::_map->GetCamera()->GetViewMatrix() * model_matrix;

					// use the colour ID shader
					glUseProgram(_shader_program);

					// set the model view projection matrix
					glUniformMatrix4fv(_loc_MVP, 1, GL_FALSE, glm::value_ptr(MVP));

					// set each models unquie colour ID
					glUniform3f(_loc_colourID, r / 255.f, g / 255.f, b / 255.f);

					Content::_map->GetActors()[i]->Render();
				}

				// Read the current pixel that is selected
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				unsigned char data[4];
				glReadPixels((GLint)Mouse::GetPointX(), 1080 - (GLint)Mouse::GetPointY(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);

				// convert the RGB colour data retrieved into a single integer actor ID
				int picked_id = data[0] + data[1] * 256 + data[2] * 256 * 256;

				if (picked_id != 0x000000)		// If an actor has been selected
				{
					picked = true;
					_selected_id = picked_id;	// Set current selected actor id
				}
				else	// Otherwise
				{
					picked = false;
					_selected_id = -1;	// Set selection to nothing
				}
			}

			// unbind FBO
			_colour_fbo->Unbind();
		}
	}
};

#endif
