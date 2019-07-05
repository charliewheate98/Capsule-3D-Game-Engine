#ifndef __LIGHT_PASS_H__
#define __LIGHT_PASS_H__

#include "LightMaster.h"
#include <glew.h>	// Get opengl variables
#include <glm\glm.hpp>	// glm variables
#include <glm\gtc/type_ptr.hpp>		// Conversion type

// This class will store the light pass data
class LightPass : public Pass
{
private:
	GLuint	_u_camera_pos;	// The camera position uniform for calculating light frags
public:
	GLuint  _u_mod;
	GLuint  _u_shadow_matrix;
	GLuint  _u_lsm; // light space matrix for rendering the shadows in viewspace

	// Constructor
	inline LightPass(GLuint shader_program)
	{
		Create(shader_program);		// Create the post effect
	}
	inline ~LightPass(){}

	// Initialise gbuffer
	inline void Create(GLuint shader_program)
	{
		_shader_programs.push_back(shader_program);		// Assign shader program

		glUseProgram(_shader_programs[0]);	// Use shader program

		// Initialise each sample into uniform locations
		glUniform1i(glGetUniformLocation(shader_program, "gPosition"), 0);
		glUniform1i(glGetUniformLocation(shader_program, "gNormal"), 1);
		glUniform1i(glGetUniformLocation(shader_program, "gAlbedo"), 2);
		glUniform1i(glGetUniformLocation(shader_program, "gSpecRough"), 3);
		glUniform1i(glGetUniformLocation(shader_program, "gMetalness"), 4);
		glUniform1i(glGetUniformLocation(shader_program, "gEmissive"), 5);
		glUniform1i(glGetUniformLocation(shader_program, "gPositionSS"), 6);
		glUniform1i(glGetUniformLocation(shader_program, "gNormalSS"), 7);
		glUniform1i(glGetUniformLocation(shader_program, "gShadowmap"), 8);
		glUniform1i(glGetUniformLocation(shader_program, "gSsao"), 9);
		glUniform1i(glGetUniformLocation(shader_program, "irradianceMap"), 10);
		glUniform1i(glGetUniformLocation(shader_program, "prefilterMap"), 11);
		glUniform1i(glGetUniformLocation(shader_program, "brdfLUT"), 12);

		Content::_map->AddActor(new DirectionalLight(shader_program, glm::vec3(2.0f, 5.0f, 3.0f), glm::vec3(1.0f, 0.9f, 0.85f), 5.0f), LIGHT);
	    //Content::_map->AddActor(new PointLight(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)), LIGHT);
	    //Content::_map->AddActor(new SpotLight(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)), LIGHT);

		LightMaster::getUniforms(shader_program, Content::_map->GetActors());

		_u_camera_pos = glGetUniformLocation(shader_program, "camera_pos");		// Initialise camera uniform location
		_u_lsm = glGetUniformLocation(shader_program, "lightSpaceMatrix"); // load in the light space matrix for the shadow mapping
		_u_shadow_matrix = glGetUniformLocation(shader_program, "shadowMatrix");
		_u_mod = glGetUniformLocation(shader_program, "mod");
	}

	// Virtual voids
	inline virtual void Update(double delta) {}
	inline virtual void Render()
	{
		glDisable(GL_BLEND);

		// Use shader program
		glUseProgram(_shader_programs[0]);	

		// render the lights
		for (Actor * l : Content::_map->GetActors())
		{
			if(l->GetObjectType() == LIGHT)
				l->Render();
		}

		LightMaster::setUniforms(Content::_map->GetActors());

		// set the camera position uniform
		glUniform3fv(_u_camera_pos, 1, glm::value_ptr(Content::_map->GetCamera()->GetPosition()));		// Bind the camera position uniform location
	}
};

#endif