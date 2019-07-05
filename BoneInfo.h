// Marco Definitions
#ifndef __BONEINFO_H__
#define __BONEINFO_H__

// c++ include files needed for this struct
#include <iostream> // file streaming & handlings
#include <glm/glm.hpp> // math class

// This struct contains the data for an individual bone (this is then stored within an array)
struct BoneInfo
{
	// bone matrices
	glm::mat4 BoneOffset; // the bones position
	glm::mat4 FinalTransformation; // the final transformation of the bone (position, rotation, scale) | This is then interpolated to result in animation

	// Constructer | initialise here
	BoneInfo()
	{
		// set the matrices to ZER0 by default
		BoneOffset = glm::mat4(0);
		FinalTransformation = glm::mat4(0);
	}
};

// end of class
#endif