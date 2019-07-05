#ifndef __MESH_H__
#define __MESH_H__

#define COLLISION_TYPE_CUBIC 0x0	// For box-like meshes
#define COLLISION_TYPE_PER_VERTEX 0x1	// For complex meshes

#include "Actor.h"	// Get our deriving class
#include "Chunk.h"	// Get access to the Chunk struct
#include "VertexData.h"		// Get access to the vertex data struct
#include "Material.h"	// Get access to the material class
#include "Vao.h"	// Get access to the ebo class
#include "Cubemap.h"	// Get access to cubemap data
#include "Query.h"

// A list of mesh types
enum MeshTypes
{
	M_STATIC,
	M_SKELETAL
};

// This abstract class will contain the basic information for our Mesh heiarchy
class Mesh : public Actor
{
protected:
	unsigned int			_mt;	// This will define our mesh type
	unsigned int			_ct;	// This will define our collision type
	unsigned int			_num_indices;	// Our index count for vao rendering
	Vao*					_vao;	// Our ebo will create our geometry
	VertexData				_vd;	// This will contain our vertex data
	Cubemap*				_cubemap;	// The cubemap ptr
	std::vector<Chunk>		_chunks;	// This will contain an array of chunks (elements)
	std::vector<Material*>	_mats;	// This will contain our material data

	Query*					_query;

	unsigned int _num_LODs;
	unsigned int _lodgroup;
public:
	bool _vis;

	// Default constructor
	inline Mesh() { _t = MESH; }

	// Deconstructor
	inline ~Mesh() { if (_vao) delete _vao; }

	inline Query *GetQuery() { return _query; }
	inline unsigned int &GetLODGroup() { return _lodgroup; }
	inline unsigned int &GetMeshType() { return _mt; }	// Return our mesh type
	inline unsigned int &GetCollisionType() { return _mt; }	// Return our mesh type
	inline unsigned int &GetNumIndices() { return _num_indices; }	// Return the number of indices
	inline Vao* GetVao() { return _vao; }	// Return our element buffer object
	inline VertexData &GetVertexData() { return _vd; }		// Return our vertex data
	inline Cubemap* GetCubemap() { return _cubemap; }	// Return the cubemap ptr
	inline std::vector<Chunk> &GetChunks() { return _chunks; }	// This returns our chunk list
	inline std::vector<Material*> &GetMaterials() { return _mats; }		// Return our materials

	inline void SetNumLODs(unsigned int nr_lods) { _num_LODs = nr_lods; }
	inline void SetLODGroup(unsigned int group) { _lodgroup = group;  }
	inline void SetMeshType(unsigned int value) { _mt = value;  }	// Assign a value to our mesh type
	inline void SetNumIndices(unsigned int value) { _num_indices = value; }		// Assign a value to our num_indices
	inline void SetVao(Vao* value) { _vao = value; }	// Assign a value to our ebo
	inline void SetVertexData(VertexData value) { _vd = value; }	// Assign a value to our vertex data
	inline void SetCubemap(Cubemap* value) { _cubemap = value; }	// Assign value ptr to cubemap ptr
	inline void SetChunks(std::vector<Chunk> &value) { _chunks = value; }	// Assign a value to our chunks
	inline void SetMaterials(std::vector<Material*> &value) { _mats = value; }	// Assign a value to our materials

	// Set collision type
	inline void SetCollisionType(unsigned int type)
	{
		_ct = type;		// Assign type

		std::vector<glm::vec3> v;	// Our collision vertices
		std::vector <CollisionData::TriangleData> td;	// Our collision triangle data

		if (type == COLLISION_TYPE_PER_VERTEX)	// If the type is per vertex...
		{
			for (unsigned int i = 0; i < _vd.indices.size(); i++)	// For each index...
				v.push_back(_vd.positions[_vd.indices[i]]);	// Assign the correct indices for our vertices
			for (unsigned int i = 0; i != v.size(); i += 3)	// For each vertex...
				td.push_back(CollisionData::TriangleData(v[i], v[i + 1], v[i + 2]));	// Assign per vertex data to the collision data
		}
		else if (type == COLLISION_TYPE_CUBIC)		// Otherwise if the type is cubic...
		{

		}

		SetCollisionData(td);	// Assign the calculated data
	}

	// Virtual voids
	inline virtual void Update(double &delta) {}
	inline virtual void Render() {}
};

#endif