#ifndef __DIRECTIONAL_H__
#define __DIRECTIONAL_H__

#include "Light.h"

class DirectionalLight : public Light
{
private:
	GLuint	  _u_light_pos;
	GLuint	  _u_light_colour;
	GLuint    _u_light_intensity;
public:
	inline DirectionalLight(GLuint shader_program, glm::vec3 position, glm::vec3 colour, float intensity)
	{
		Create(shader_program, position, colour, intensity);
	}
	inline ~DirectionalLight() {}

	inline void Create(GLuint shader_program, glm::vec3 position, glm::vec3 colour, float intensity)
	{
		_t = LIGHT;
		_light_type = DIRECTIONAL_LIGHT;

		_cast_shadow = true;

		_position  = position;
		_colour    = colour;
		_intensity = intensity;

		_u_light_pos       = glGetUniformLocation(shader_program, "lightPos");
		_u_light_colour    = glGetUniformLocation(shader_program, "lightColour");
		_u_light_intensity = glGetUniformLocation(shader_program, "lightIntensity");
	}

	inline virtual void Update(double& delta) {}

	inline virtual void Render()
	{
		glm::vec3 _light_pos_view = glm::vec3(/*Content::_map->GetCamera()->GetViewMatrix() */ glm::vec4(_position.x, _position.y, _position.z, 1.0f));

		glUniform3f(_u_light_pos, _light_pos_view.x, _light_pos_view.y, _light_pos_view.z);
		glUniform3f(_u_light_colour, _colour.x, _colour.y, _colour.z);
		glUniform1f(_u_light_intensity, _intensity);
	}
};

#endif
