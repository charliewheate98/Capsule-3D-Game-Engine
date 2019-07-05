#ifndef __POINT_H__
#define __POINT_H__

#include "Light.h"

class PointLight : public Light
{
private:
	float _radius;
public:
	inline PointLight(glm::vec3 position, glm::vec3 colour)
	{
		Create(position, colour);
	}
	inline ~PointLight() {}

	inline void Create(glm::vec3 position, glm::vec3 colour)
	{
		_t = LIGHT;
		_light_type = POINT_LIGHT;

		_position = position;
		_colour   = colour;
	}

	inline virtual void Update(double& delta) {}
	inline virtual void Render() {}
};

#endif
