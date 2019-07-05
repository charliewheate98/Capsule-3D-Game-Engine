#ifndef __CUBEMAP_URI__
#define __CUBEMAP_URI__ ((char*)"Res/Content/Cubemap/")
#endif

#ifndef __CUBEMAP_H__
#define __CUBEMAP_H__

#include "Texture.h"	// Get access to textureCube

// This class will store and render a cubemap to frame buffer
class Cubemap : public Object
{
private:
	uniform						_uniform;	// The cubemap uniform
	std::vector<std::string>	_faces;		// Face file directories
	Texture::TextureCubemap*	_cubemap;	// The cubemap texture

public:
	// Default constructor
	inline Cubemap() = default;

	// Initial constructor
	inline Cubemap(GLuint &shader_program, std::vector<std::string> faces)
	{
		if (faces.size() != 6)	// If num faces are not equal to 6
		{
			std::cout << "Error: Failed to create cubemap - N faces is not equal to 6!\n";	// Print error message
			return;		// Return as failed
		}

		_faces = faces;		// Assign face information
		_uniform = glGetUniformLocation(shader_program, "cubemap");	// Get uniform location for cubemap
		std::vector<std::string> faces_opt;		// Temp optimised container

		for (size_t i = 0; i < 6; i++)	// For each face
			faces_opt.push_back(static_cast<std::string>(__CUBEMAP_URI__) + faces[i]);	// Assign file with uri directory

		_cubemap = new Texture::TextureCubemap(faces_opt);	// Create cubemap texture from opt faces
	}

	// Deconstructor
	inline ~Cubemap()
	{
		if (_cubemap) delete _cubemap;	// Delete texture mem
	}

	// Virtual voids
	inline virtual void Update(double delta) {}
	inline virtual void Render()
	{
		glUniform1i(_uniform, _cubemap->unit);	// Bind the texture cubemap uniform
		_cubemap->Render();		// Bind the texture
	}
};

#endif