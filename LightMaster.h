#ifndef __LIGHTMASTER_H__
#define __LIGHTMASTER_H__

#include "Content.h"
#include "Light.h"
#include "Point.h"
#include "Spot.h"
#include "Directional.h"

#define MAX_LIGHTS 100

class LightMaster
{
private:
	// point
	static unsigned int _u_radius[MAX_LIGHTS];

	// spot
	static unsigned int _u_cutOff[MAX_LIGHTS];
	static unsigned int _u_outerCutOff[MAX_LIGHTS];
public:
	inline LightMaster() = default;
	inline ~LightMaster() {}

	static void getUniforms(int shader_program, std::vector<Actor*> _actors)
	{
		for (unsigned int i = 0; i < _actors.size(); i++)
		{
			if (_actors.size() > 0)
			{
				if (_actors[i]->GetObjectType() == LIGHT)
				{
					switch (((Light*)_actors[i])->GetLightType())
					{
					case POINT_LIGHT: // POINT
					{
						((Light*)_actors[i])->_u_point_positions[i] = glGetUniformLocation(shader_program, ("pointLights[" + std::to_string(i) + "].position").c_str());
						((Light*)_actors[i])->_u_point_colours[i] = glGetUniformLocation(shader_program, ("pointLights[" + std::to_string(i) + "].colour").c_str());

						break;
					}
					case SPOT_LIGHT: // SPOT
					{
						((Light*)_actors[i])->_u_spot_positions[i] = glGetUniformLocation(shader_program, ("spotLights[" + std::to_string(i) + "].position").c_str());
						((Light*)_actors[i])->_u_spot_directions[i] = glGetUniformLocation(shader_program, ("spotLights[" + std::to_string(i) + "].direction").c_str());
						((Light*)_actors[i])->_u_spot_colours[i] = glGetUniformLocation(shader_program, ("spotLights[" + std::to_string(i) + "].colour").c_str());
						((Light*)_actors[i])->_u_spot_cutoff[i] = glGetUniformLocation(shader_program, ("spotLights[" + std::to_string(i) + "].cutoff").c_str());
						((Light*)_actors[i])->_u_spot_outercutoff[i] = glGetUniformLocation(shader_program, ("spotLights[" + std::to_string(i) + "].outerCutoff").c_str());

						break;
					}
					default:
						break;
					}
				}			
			}
			else
				std::cerr << "Not enough lights in scene" << std::endl;
		}
	}

	static void setUniforms(std::vector<Actor*> _actors)
	{
		if (_actors.size() > 0)
		{
			for (unsigned int i = 0; i < _actors.size(); i++)
			{
				if (_actors[i]->GetObjectType() == LIGHT)
				{
					switch (((Light*)_actors[i])->GetLightType())
					{
					case POINT_LIGHT: // POINT
					{
						glm::vec3 light_pos_view = glm::vec3(Content::_map->GetCamera()->GetViewMatrix() * glm::vec4(((Light*)_actors[i])->GetLightPosition().x, ((Light*)_actors[i])->GetLightPosition().y, ((Light*)_actors[i])->GetLightPosition().z, 1.0f));

						glUniform3f(((Light*)_actors[i])->_u_point_positions[i], light_pos_view.x, light_pos_view.y, light_pos_view.z);
						glUniform3f(((Light*)_actors[i])->_u_point_colours[i], ((Light*)_actors[i])->GetLightColour().x, ((Light*)_actors[i])->GetLightColour().y, ((Light*)_actors[i])->GetLightColour().z);

						break;
					}
					case SPOT_LIGHT: // SPOT
					{
						glm::vec3 spot_light_pos_view = glm::vec3(Content::_map->GetCamera()->GetViewMatrix() *
							glm::vec4(((Light*)_actors[i])->GetLightPosition().x, ((Light*)_actors[i])->GetLightPosition().y, ((Light*)_actors[i])->GetLightPosition().z, 1.0f));
						glm::vec3 spot_light_dir_view = glm::vec3(Content::_map->GetCamera()->GetViewMatrix() * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));

						glUniform3f(((Light*)_actors[i])->_u_spot_positions[i], spot_light_pos_view.x, spot_light_pos_view.y, spot_light_pos_view.z);
						glUniform3f(((Light*)_actors[i])->_u_spot_directions[i], spot_light_dir_view.x, spot_light_dir_view.y, spot_light_dir_view.z);
						glUniform3f(((Light*)_actors[i])->_u_spot_colours[i], ((Light*)_actors[i])->GetLightColour().x, ((Light*)_actors[i])->GetLightColour().y, ((Light*)_actors[i])->GetLightColour().z);
						glUniform1f(((Light*)_actors[i])->_u_spot_cutoff[i], glm::cos(glm::radians(12.5f)));
						glUniform1f(((Light*)_actors[i])->_u_spot_outercutoff[i], glm::cos(glm::radians(17.5f)));

						break;
					}
					default:
						break;
					}
				}
			}
		}
	}
};

// point
unsigned int LightMaster::_u_radius[MAX_LIGHTS];

// spot
unsigned int LightMaster::_u_cutOff[MAX_LIGHTS];
unsigned int LightMaster::_u_outerCutOff[MAX_LIGHTS];

#endif