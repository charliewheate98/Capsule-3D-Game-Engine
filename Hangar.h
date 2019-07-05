#ifndef __TESTMAP_H__
#define __TESTMAP_H__

#include "Engine/DataIO.h"	
#include "Engine/Map.h"

class TestMap : public Map
{
private:
public:
	inline TestMap() = default;

	inline TestMap(GLuint shader_program) { Create(shader_program); }
	inline ~TestMap() {}

	inline void Create(GLuint shader_program)
	{		
		// name of the map
		SetName("CM_Default");

		// materials for this map
		std::vector<Material*> hangar_materials;
		hangar_materials.push_back(Content::_materials[0]);

		// assets
		DataIO::Import::ImportAsset(shader_program, "rock", hangar_materials, 3);

		{ // add assets to the vectorlist
			Content::_map->GetActors().push_back(Content::_assets[Content::_assets.size() - 1]);
		}
	}

	// The update function will check for logic
	inline virtual void Update(double &delta) {}

	// This will render all actors in the world
	inline virtual void Render() {}
};

#endif