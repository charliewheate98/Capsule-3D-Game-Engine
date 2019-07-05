#ifndef __CONTENT_H__
#define __CONTENT_H__

#include "Map.h"	// Get access to our map class
#include "Font.h"	// Get font class
#include "Mesh.h"	// Get mesh abstract
#include "Cubemap.h"	// Get cubemap data
#include "Asset.h"

using namespace Texture;	// Get namespace for texture objects

// This class will contain all of our content
class Content
{
public:
	static Map*							_map;		// This will contain our map  content
	static std::vector<Font*>			_fonts;		// This will contain our list of fonts
	static std::vector<TextureBase*>	_textures;	// Store our textures
	static std::vector<Material*>		_materials;	// Store our materials
	static std::vector<Cubemap*>		_cubemaps;	// Store cubemap data
	static std::vector<Mesh*>			_meshes;	// This will contain our mesh content
	static std::vector<Asset*>			_assets;
	static std::vector<Actor*>			_arrows;

	// A function that deletes all allocated memory
	inline static void Destroy()
	{
		_textures.clear();	// Clear texture data
		_materials.clear();		// Clear material data
		_cubemaps.clear();	// Clear cubemap data
		_meshes.clear();	// Clear mesh data
		_fonts.clear();		// Clear font data

		if (_map) delete _map;	// Delete map
	}

	// A function that gets the desired material via name
	inline static Material* GetMaterial(std::string name)
	{
		for (unsigned int i = 0; i < _materials.size(); i++)	// Iterate through each material...
		{
			if (_materials[i]->GetName() == name)	// If the material name is found...
				return _materials[i];	// Return this material
		}

		return NULL;	// Return null by default
	}
};

// Static definitions
Map*						Content::_map;
std::vector<Material*>		Content::_materials;
std::vector<TextureBase*>	Content::_textures;
std::vector<Cubemap*>		Content::_cubemaps;
std::vector<Mesh*>			Content::_meshes;
std::vector<Asset*>			Content::_assets;
std::vector<Font*>			Content::_fonts;
std::vector<Actor*>			Content::_arrows;

#endif 