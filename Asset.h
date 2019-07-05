#ifndef __ASSET_H__
#define __ASSET_H__

#include "Mesh.h"
#include "Content.h"

class Asset : public Mesh
{
private:
	std::vector<float> _lod_distances;
public:
	unsigned int _current_LOD;
	std::vector<Mesh*> _meshes;

	inline Asset() 
	{
	}

	inline ~Asset() 
	{
		_meshes.clear();
	}

	inline glm::vec3 getPosition() { return _meshes[0]->GetPosition(); }

	inline void assignMaterial(std::vector<Material*> _material)
	{
		for (unsigned int i = 0; i < _meshes.size(); ++i)
		{
			_meshes[i]->SetMaterials(_material);
		}
	}

	inline virtual void Update(double& delta)
	{ 
		for(unsigned int i = 0; i < _meshes.size(); ++i)
			_meshes[i]->Update(delta);
	}

	inline virtual void Render()
	{
		_meshes[_current_LOD]->Render();
	}
};

#endif
