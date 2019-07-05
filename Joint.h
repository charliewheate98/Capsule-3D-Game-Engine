#ifndef __JOINT_H__
#define __JOINT_H__

#include <vector>	// Get dynamic array
#include <glm\glm.hpp>	// Get glm variables

struct Joint
{

	unsigned int		_id;
	std::string			_name;
	glm::mat4x4			_matrix;

	std::vector<Joint>	_children;

	inline Joint() {}
	inline Joint(unsigned int id, std::string name, glm::mat4x4 matrix)
	{

	}
};

#endif