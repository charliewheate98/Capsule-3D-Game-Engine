#ifndef __H_JOINT_ANIM_DATA__
#define __H_JOINT_ANIM_DATA__

#include <string>	// Include string variable	
#include <vector>	// Include dynamic arrays
#include <glm\glm.hpp>	// Include 3d math variables

// A struct for containing the animation data
struct JointAnim
{
	std::string					_joint_name;	// Represents the joint name / id
	std::vector<float>			_time_stamps;	// A list of time stamps for each pose
	std::vector<glm::mat4x4>	_matrices;	// A pose for each time stamp

	// Default constructor
	inline JointAnim() : _joint_name("") {}

	// Initial constructor
	inline JointAnim(std::string joint_name)
	{
		_joint_name = joint_name;	// Assign joint name
	}
};

#endif