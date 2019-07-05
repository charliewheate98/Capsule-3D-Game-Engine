#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#define __PI	3.14159		// For radial calculations

#include <glm\glm.hpp>	// Get 3D variables
#include <glm\gtc\type_ptr.hpp>		// Get glm conversion type
#include <glm\gtx\transform.hpp>	// Get transform functions

// A basic transform structure
typedef struct {
	glm::vec3	_pos;	// X, Y and Z position
	glm::vec3	_sca;	// X, Y and Z scale
	glm::vec3	_rot;	// X, Y, and Z in degrees (Eular rotation)
	glm::vec3	_rad;	// X, Y and Z radius
	glm::mat4	_mod;	// 4x4 model matrix
} Transformv3;

#endif