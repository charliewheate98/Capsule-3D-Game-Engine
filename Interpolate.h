#ifndef __INTERPOLATE_H__
#define __INTERPOLATE_H__

#include <glm\glm.hpp>	// Include glm variables

// This namespace will contain inpolation functions from a range of linear types
namespace Interpolate
{
	struct Keyframe
	{
		glm::vec3 _position;
		
		inline Keyframe(glm::vec3 position)
		{
			_position = position;
		}
	};

	// This function will interpolate an object in constant form
	static inline void Linestep(glm::vec3 &in_position, glm::vec3 &in_velocity, glm::vec3 look_vector, float speed, double delta)
	{
		in_velocity = look_vector * speed;	// Update our velocity vector
		in_position += in_velocity * (float)delta;	// Update our position
	}

	// This function will interpolate an object in constant form
	static inline void Smoothstep(glm::vec3 &in_position, glm::vec3 &in_velocity, glm::vec3 look_vector, float speed, double delta)
	{
		in_velocity = look_vector * speed;	// Update our velocity vector
		in_position += in_velocity * (float)delta;	// Update our position
	}
}

#endif