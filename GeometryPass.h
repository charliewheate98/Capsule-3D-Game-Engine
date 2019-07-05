#ifndef __GEOMETRY_PASS_H__
#define __GEOMETRY_PASS_H__

#include <glm/gtx/quaternion.hpp>

#include "Canvas.h"		
#include "Pass.h"	// Include abstract class
#include "TextureCache.h"
#include "SoundMaster.h"
#include "Animator.h"
#include "Canvas.h"
#include "Timer.h"
#include "AnimMesh.h"

using namespace Animator;

/*
* Geometry pass class: This class will store the geometry pass for each vertex.
* The geometry pass stores a range of data in the form of textures including,
* world positions, normals, albedo and specularity
*/
class GeometryPass : public Pass
{
private:
	AnimMesh	*_anim_mesh;

	Timer *timer;

	GLuint	_u_camera_pos;
	GLuint	_u_selection_colour;
	GLuint	_u_wire_mode;
	GLuint query;

	Canvas *_world;

	Mover* _mover;
	std::vector<glm::vec3> keyframes;
	std::vector<glm::vec3> keyframes2;
	std::vector<glm::vec3> rotations;

	unsigned int _LOD_Group;

	bool	_wire_mode;	// Polygon mode variable
	unsigned int cubemap;
public:
	unsigned int skyboxVAO, skyboxVBO;

	inline GeometryPass(std::vector<GLuint> shader_programs)
	{
		Create(shader_programs);		// Create the post effect
	}
	// Initialise geometry pass
	inline void Create(std::vector<GLuint> shader_programs)
	{
		_shader_programs = shader_programs;
		_wire_mode = false;	// Set default polygon mode

		timer = new Timer(1.0f);

		glUseProgram(_shader_programs[0]);	// Use shader program

		_u_camera_pos = glGetUniformLocation(_shader_programs[0], "camera_pos");
		_u_selection_colour = glGetUniformLocation(_shader_programs[0], "selection_colour");
		_u_wire_mode = glGetUniformLocation(_shader_programs[0], "wire_mode");

		// Initialise default map content
		Content::_map = new Map("Scene_01");
		Content::_map->AddActor(new Camera(_shader_programs[0], glm::vec3(0.0f, 5.0f, 0.0f),
			CAMERA_FOV, CAMERA_SPEED, CAMERA_LOOK_SENSITIVITY, CAMERA_NEAR, CAMERA_FAR, GetUpdatedAspectRatio()), CAMERA);

		_anim_mesh = new AnimMesh(_shader_programs[0], "Bob_Idle", "walking");

		TextureCache::Initialise();

		unsigned int ids[] = { 0, 1, 2, 3, 4 };
		TextureCache::GetTexture(_shader_programs[0], "barrel", ids);

		//_world = new Canvas(_shader_programs[0]);

		////// Mover
		//_mover = new Mover();
		//keyframes.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		//keyframes.push_back(glm::vec3(5.0f, 15.0f, 10.0f));
		//keyframes.push_back(glm::vec3(2.0f, 2.0f, 15.0f));
		//keyframes.push_back(glm::vec3(5.0f, 15.0f, 5.0f));
		//keyframes2.push_back(glm::vec3(5.0f, 15.0f, 5.0f));
		//keyframes2.push_back(glm::vec3(5.0f, 2.0f, 10.0f));
		//keyframes2.push_back(glm::vec3(2.0f, 12.0f, 15.0f));
		//keyframes2.push_back(glm::vec3(5.0f, 15.0f, 25.0f));
		//rotations.push_back(glm::vec3(0.0f, glm::radians(25.0f), glm::radians(15.0f)));
		//rotations.push_back(glm::vec3(0.0f, glm::radians(45.0f), 0.0f));
		//rotations.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		//rotations.push_back(glm::vec3(0.0f, glm::radians(25.0f), glm::radians(15.0f)));
		//_mover->addPath(keyframes, rotations);
		//_mover->addPath(keyframes2, rotations);
	}

	inline bool &IsWireMode() { return _wire_mode; }	// Return the current polygon mode
	inline void SetWireMode(bool value) { _wire_mode = value; }		// Set the current polygon mode

	// Virtual functions
	inline virtual void Update(double delta)
	{
		Content::_map->Update(delta);

		//_world->Update(delta);

		//if (!Content::_meshes.empty())
		//{
		//	//_mover->Play(delta, Content::_meshes[0]);
		//	//std::cout << Content::_meshes[0]->_trans._pos.z << std::endl;
		//}

		Content::_map->GetCamera()->UpdateLookVectors();
	}
	inline virtual void Render()
	{
		glDisable(GL_BLEND);		// Disable blending for opique materials
		glDisable(GL_CULL_FACE);

		glUseProgram(_shader_programs[0]);	// Use shader program

		glUniform1i(_u_wire_mode, _wire_mode);	// Send polygon mode to shader
		glUniform3fv(_u_camera_pos, 1, glm::value_ptr(Content::_map->GetCamera()->GetPosition()));		// Bind the camera position uniform location

		Content::_map->GetCamera()->Render();	// Render the camera

		for (Actor* a : Content::_map->GetActors())		// Iterate through each actor in map
		{
			if (a->GetObjectType() == MESH) // If object type is type mesh
			{
				glm::mat4 MVP = Content::_map->GetCamera()->GetProjectionMatrix()
					* Content::_map->GetCamera()->GetViewMatrix() * a->GetModelMatrix();

				if (a->IsSelected()) // If wire mode is toggled
				{
					glDisable(GL_CULL_FACE);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Assign current polygon mode

					a->Render();
				}

				bool obj_visible = Content::_map->GetCamera()->objectVisible(glm::value_ptr(MVP),
					a->GetPosition().x, a->GetPosition().y, a->GetPosition().z, 1.0f);

				a->UpdateModel();

				
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Assign current polygon mode
				/*if (obj_visible)*/ a->Render();

				glEnable(GL_CULL_FACE);
				glEnable(GL_DEPTH_TEST);	// Enable depth tests
			}
		}
	}
};

#endif