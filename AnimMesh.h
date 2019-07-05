#ifndef __ANIM_MESH_H__
#define __ANIM_MESH_H__

#include "Mesh.h"	// Derive from
#include "SkinnedMesh.h"
#include "Timer.h"

class AnimMesh : public Mesh
{
private:
	GLuint    _loc_bones[100];
	
	Timer* _timer;

	unsigned int prog;

	SkinnedMesh	_riggedMesh;
public:
	inline AnimMesh(unsigned int shader_program, std::string name, const std::string& filename)
	{
		Create(shader_program, name, filename);
	}
	inline ~AnimMesh() 
	{
		_riggedMesh.Clear();
	}

	inline void Create(unsigned int shader_program, std::string name, const std::string& filename)
	{
		prog = shader_program;

		SetName(name);
		//SetMaterials();
		UpdateModel();

		_timer = new Timer(1.0f);

		_vis = true;

		_u_mod = glGetUniformLocation(shader_program, "mod");	// Get our model matrix uniform
		_u_rig = glGetUniformLocation(shader_program, "isRigged");

		_riggedMesh.LoadAnimatedMesh(filename);
	}

	// Virtual functions
	inline virtual void Update(double &delta)
	{
	}
	inline virtual void Render()
	{
		_trans._sca = glm::vec3(85.0f, 85.0f, 85.0f);

		UpdateModel();

		glUniform1i(_u_rig, true);	// Bind our selected uniform data
		glUniformMatrix4fv(_u_mod, 1, GL_FALSE, glm::value_ptr(_trans._mod));	// Bind our uniform data
		
		std::vector<glm::mat4> _bone_transforms;
		_riggedMesh.BoneTransform(_timer->seconds(), _bone_transforms);

		for (unsigned int i = 0; i < _bone_transforms.size(); i++)
			glUniformMatrix4fv(glGetUniformLocation(prog, ("gBones[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(_bone_transforms[i]));

		Content::_materials[0]->Bind();

		_riggedMesh.Render();
	}
};

#endif