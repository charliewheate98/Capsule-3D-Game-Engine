// Marco Definitions
#ifndef __VERTEXBONEDATA_H__
#define __VERTEXBONEDATA_H__

// obtain the needed helper functions for this struct
#include "HelperFunctions.h"

// the needed c++ internal classes
#include <iostream> // file streaming and handling
#include <glm/glm.hpp> // maths library

// animation marcros
#define NUM_BONES_PER_VERTEX 4 // you can have a max of 4 bones per vertex

// unsigned int typedef for more simplistic purposes
typedef unsigned int uint;

// This struct stores the bone data for each vertex
struct VertexBoneData
{
	// bones attached to the vertex
	uint IDs[NUM_BONES_PER_VERTEX];

	// bone weights
	float Weights[NUM_BONES_PER_VERTEX];

	// Constructer
	VertexBoneData()
	{
		// reset when loading new animation
		Reset();
	};

	// reset when loading new animation
	void Reset()
	{
		// reset these arrays to zero on load of new animation
		ZERO_MEM(IDs);
		ZERO_MEM(Weights);
	}

	// calculate the bone data 
	void AddBoneData(uint BoneID, float Weight)
	{
		for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
			if (Weights[i] == 0.0) {
				IDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}

		// should never get here - more bones than we have space for
		assert(0);
	}
};

// end of class
#endif