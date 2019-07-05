#ifndef __STATIC_MESH_EXTENSION__
#define __STATIC_MESH_EXTENSION__	((char*)".mesh")
#endif

#ifndef __STATIC_MESH_URI__
#define __STATIC_MESH_URI__			((char*)"Res/Content/StaticMesh/")
#endif

#ifndef __STATIC_MESH_H__
#define __STATIC_MESH_H__

#include "Mesh.h"	// Derive from
#include "ObjLoader.h"

class StaticMesh : public Mesh
{
private:
public:
	GLuint _query_object;

	// Default constructor
	inline StaticMesh() { _mt = M_STATIC; }

	// Initial constructor
	inline StaticMesh(unsigned int shader_program, std::string name, std::vector<Material*> &materials, Cubemap* cubemap)
	{
		_mt = M_STATIC;		// Assign our mesh type

		SetName(name);	// Assign our name data
		SetMaterials(materials);	// Assign our material data
		UpdateModel();	// Initialise our model matrix

		_vis = true;

		_u_sel = glGetUniformLocation(shader_program, "selected");	// Get our model matrix uniform
		_u_mod = glGetUniformLocation(shader_program, "mod");	// Get our model matrix uniform
		_u_rig = glGetUniformLocation(shader_program, "isRigged");

		_query = new Query(GL_SAMPLES_PASSED);
	}

	// Virtual functions
	inline virtual void Update(double &delta) 
	{
	}
	inline virtual void Render()
	{
		glUniform1i(_u_rig, false);	// Bind our selected uniform data
		glUniform1i(_u_sel, _sel);	// Bind our selected uniform data
		glUniformMatrix4fv(_u_mod, 1, GL_FALSE, glm::value_ptr(_trans._mod));	// Bind our uniform data

		_vao->Bind();	// Bind our element buffer object	

		_query->start();

		for (Chunk c : _chunks)		// Iterate through each chunk element...
		{
			_mats[c._id]->Bind();	// Bind our material(s)

			if(_vis)
			{
				glDrawElements(
					GL_TRIANGLES,				// mode
					c._index_count,				// count
					GL_UNSIGNED_INT,			// type
					(void*)(c._index_offset));	// element array buffer offset
			}
		}

		_query->end();

		_query->getResult();
		_query->checkResult();
	}
};

#endif