// Marco definitions
#ifndef __TEXTURECACHE_H__
#define __TEXTURECACHE_H__

/*
* Includes
*/
#include <iostream>
#include <assert.h>
#include <map>
#include "Content.h"
#include "dirent.h"

#define MAT_EXTENSION ((char*)"_mat.mat")

/*
* TextureCache class: This class handles loading in textures and generating them 
* only when they are needed to be used (the textures only in the current scene are 
* stored within Ram)
*/
class TextureCache
{
private:

	static int nr_off;

	static std::map<std::string, Material*> _texture_map;
public:
	static std::vector<unsigned int> IDs;
	static std::vector<std::string> _entrys;
	static std::vector<std::string> _textures;

	inline TextureCache() {}

	inline ~TextureCache()
	{
		_texture_map.clear();
	}

	static inline void Initialise()
	{
		nr_off = 0;
		IDs.push_back(0);
	}

	static inline Material GetTexture(unsigned int shader_program, std::string file, unsigned int ids[])
	{
		auto texture_location = _texture_map.find(file);

		if (texture_location == _texture_map.end())
		{
			std::cout << "texture loaded : file" << "\n" << std::endl;

			Content::_textures.push_back(new Texture2d({ file + "_n.dds" }, NORMAL, GL_REPEAT, GL_LINEAR));	// Push back default texture normal
			Content::_textures.push_back(new Texture2d({ file + "_a.dds" }, ALBEDO, GL_REPEAT, GL_LINEAR));	// Push back default texture albedo
			Content::_textures.push_back(new Texture2d({ file + "_sr.dds" }, SPECROUGH, GL_REPEAT, GL_LINEAR));	// Push back default texture spec
			Content::_textures.push_back(new Texture2d({ file + "_m.dds" }, METALIC, GL_REPEAT, GL_LINEAR));	// Push back default texture metalic
			Content::_textures.push_back(new Texture2d({ file + "_e.dds" }, EMISSIVE, GL_REPEAT, GL_LINEAR));	// Push back default texture metalic

			Content::_materials.push_back(new Material(shader_program, file + static_cast<std::string>(MAT_EXTENSION),
				{ Content::_textures[ids[0]], Content::_textures[ids[1]], Content::_textures[ids[2]], Content::_textures[ids[3]], Content::_textures[ids[4]] }));

			_texture_map.insert(std::make_pair(file, Content::_materials[Content::_materials.size() - 1]));

			return *Content::_materials[Content::_materials.size() - 1];
		}

		std::cout << "reused texture : " << file << "\n" << std::endl;

		return *texture_location->second;
	}
};

std::map<std::string, Material*> TextureCache::_texture_map;
std::vector<unsigned int> TextureCache::IDs;
std::vector<std::string> TextureCache::_entrys;
int TextureCache::nr_off;
std::vector<std::string> TextureCache::_textures;

// End of class
#endif

