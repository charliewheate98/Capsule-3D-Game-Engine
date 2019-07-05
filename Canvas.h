// start of the class
#ifndef __CANVAS_H__
#define __CANVAS_H__

// includes
#include "Engine/DataIO.h"		
#include "Engine/Content.h"
#include "Hangar.h"
#include <glew.h>

// the canvas class will be used to construct the map
class Canvas
{
private:
	// list of chunks in the map
	enum CHUNKS
	{
		M_HANGAR,
		M_VOID,
	};
	std::vector<float> _lod_distances;
	std::vector<Map*> _maps;

	// current map the player is within
	unsigned int _current_chunk;
public:
	inline Canvas(GLuint shader_program) 
	{
		_maps.push_back(new TestMap(shader_program));

		for (unsigned int i = 0; i < Content::_assets[Content::_assets.size() - 1]->_meshes.size(); ++i)
		{
			float start = 5.0f;
			float range = 20.0f;

			_lod_distances.push_back(start + i * range);
		}
	}
	inline ~Canvas() 
	{
		_lod_distances.clear();
		_maps.clear();
	}

	inline void Update(double delta) 
	{
		float distance;

		for (unsigned int i = 0; i < Content::_assets.size(); ++i)
		{
			distance = glm::distance(Content::_map->GetCamera()->GetPosition(), Content::_assets[i]->_meshes[0]->GetPosition());
			
			for (unsigned int j = 0; j < _lod_distances.size(); ++j)
			{
				if (distance < _lod_distances[0])
					Content::_assets[i]->_current_LOD = 0;	
				else if (distance > _lod_distances[j])
					Content::_assets[i]->_current_LOD = j;
			}
		}
	}
	inline void Render() {}
};

#endif
