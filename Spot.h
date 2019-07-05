#ifndef __SPOT_H__
#define __SPOT_H__

#include "Light.h"

class SpotLight : public Light
{
private:
public:
	inline SpotLight(glm::vec3 position, glm::vec3 colour)
	{
		Create(position, colour);
	}
	inline ~SpotLight() {}

	inline void Create(glm::vec3 position, glm::vec3 colour)
	{
		_t = LIGHT;
		_light_type = SPOT_LIGHT;

		_cast_shadow = false;

		_position = position;
		_colour = colour;
	}

	inline virtual void Update(double& delta) {}
	inline virtual void Render() 
	{
		
	}
};

#endif
