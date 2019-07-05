#ifndef __PASS_H__
#define __PASS_H__

#define GEOMETRY_PASS	0	// Define geometry pass index
#define LIGHT_PASS		1	// Define light pass index
#define GBUFFER_PASS	2	// Define gbuffer pass index
#define SSAO_PASS		3	// Define ssao pass index
#define FINAL_GATHER	4	// Define final gather pass

#include "Content.h"	// Get content data
#include "Fbo.h"	// Get access to the FBO class
#include "Rbo.h"	// Get access to the RBO class

// This abstract class will store the data needed for a pass post-effect pass
class Pass
{
protected:
	Rbo*							_rbo;	// Render buffer object
	std::vector<Fbo*>				_fbos;	// Dynamic array of frame buffer objects
	std::vector<GLuint>				_shader_programs;	// Shader programs (pointers to allocated mem)

public:
	// Constructors
	inline Pass() = default;

	// Deconstructor
	inline ~Pass()	
	{
		_fbos.clear();	// Clear all frame buffer objects

		if (_rbo) delete _rbo;	// If rbo is initialised, delete it
	}

	// Get frame buffer objects
	inline std::vector<Fbo*> &GetFbos()
	{
		return _fbos;	// Return frame buffer object
	}

	// Get render buffer object
	inline Rbo* GetRbo()
	{
		return _rbo;	// Return render buffer object
	}

	// Virtual functions
	inline virtual void Update(double delta) {}
	inline virtual void Render() {}
};

#endif