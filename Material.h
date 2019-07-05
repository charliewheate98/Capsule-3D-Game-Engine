#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <glm\gtc\type_ptr.hpp>		// Get valur_ptr for glm
#include <glm\glm.hpp>	// Get access to vectors
#include "Object.h"		// Derive from object class
#include "Uniform.h"	// Get access to our uniforms
#include "Texture.h"	// Get access to our texture object

#define MAX_TEXTURES	5	// Define the maximum texture count



// This class will store all of the PBR properties for binding to ebos
class Material : public Object
{
private:
	GLuint								_texture_uniforms[MAX_TEXTURES];	// Texture map uniform data
	std::vector<Texture::TextureBase*>	_textures;	// Albedo texture map

public:
	// Default constructor
	inline Material() {}
	
	// Initial constructor
	inline Material(GLuint shader_program, std::string name, std::vector<Texture::TextureBase*> textures)
	{
		SetName(name);	// Assign name data
		
		_textures = textures;	// Assign textures

		if (textures.size() > MAX_TEXTURES)		// If the texture count exceeds maximum
		{
			std::cout << "Error: Failed to create material - texture count exceeds max tex count!\n";	// Print out error message
			return;		// Return
		}
		
		for (size_t i = 0; i < textures.size(); i++)	// Iterate through each texture
		{
			if (!textures[i]->type_2d)	// If texture type is type cube...
			{
				std::cout << "Error: Failed to initialise material - texture index " << i << " is type cube!\n";
				return;
			}
			else	// Otherwise, choose texture type as 2d
			{
				Texture::Texture2d* t = ((Texture::Texture2d*)textures[i]);		// Create temp reference var
				switch (t->unit)	// Iterate through each map type
				{
				case NORMAL:
					_texture_uniforms[i] = glGetUniformLocation(shader_program, "normal");	// Get uniform location for map
					break;
				case ALBEDO:
					_texture_uniforms[i] = glGetUniformLocation(shader_program, "albedo");	// Get uniform location for map
					break;
				case SPECROUGH:
					_texture_uniforms[i] = glGetUniformLocation(shader_program, "specrough");	// Get uniform location for map
					break;
				case METALIC:
					_texture_uniforms[i] = glGetUniformLocation(shader_program, "metalness");	// Get uniform location for map
					break;
				case EMISSIVE:
					_texture_uniforms[i] = glGetUniformLocation(shader_program, "emissive");	// Get uniform location for map
					break;
				}
			}
		}
		
		size_t empty_slots = MAX_TEXTURES - textures.size();	// Calculate empty slots if any
		
		for (size_t i = 0; i < empty_slots; i++)	// For each empty slot...
		{
			size_t map_type_index = _textures.size() + i;	// Get the index offset
			GLubyte data_a[] = { 255, 255, 255 };	// Set a default colour of white for most empty map

			_textures.push_back(new Texture::Texture2d((uint8_t)map_type_index, 1, 1, { (GLubyte**)data_a } ));		// Implement the extra slots
		}
	}

	// Deconstructor
	inline ~Material()
	{
		if (!_textures.empty())		// If container is not empty...
			_textures.clear();	// Delete texture objects
	}

	// Get a texture from vector container
	inline Texture::TextureBase* GetTexture(size_t index)
	{
		if (_textures[index])	// If the texture exists...
			return _textures[index];	// Return the texture object

		std::cout << "Error: Failed to get texture - texture is NULL!\n";	// Print error message
		return NULL;	// Return zero
	}

	// Assign a texture to an already existing texture map slot
	inline void AssignTexture(uint8_t map_type, Texture::TextureBase* texture)
	{
		if (map_type <= MAX_TEXTURES)	// If the index type is less or equal to max amount of textures
		{
			_textures[map_type] = texture;	// Assign new texture to texture slot
			return;		// Return success
		}

		std::cout << "Error: Failed to assign texture - type index is greater than " << MAX_TEXTURES << "!\n";	// Print error message
	}

	// This function will bind our material properties
	inline void Bind()
	{
		for (size_t i = 0; i < MAX_TEXTURES; i++)	// Iterate through each texture type
		{
			glUniform1i(_texture_uniforms[i], (GLint)i);	// Bind the texture map uniforms
			_textures[i]->Render();		// Bind the texture objects
		}
	}
};


#endif