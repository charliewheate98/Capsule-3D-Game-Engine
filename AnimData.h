#ifndef __ANIM_DATA_H__
#define __ANIM_DATA_H__

#include "Joint.h"	// Include the joint struct
#include "JointAnim.h"	// Include joint anim data

// The main anim data struct
struct AnimData
{
	Joint	_root_joint;	// The root joint of heriarchy

	// Default constructor
	inline AnimData() {}
};

#endif
