#ifndef __MAP_H__
#define __MAP_H__

#include "Glsl.h"		// Shader data
#include "Mouse.h"		// Get mouse data
#include "Camera.h"		// Get deriving class
#include "Skybox.h"		// Get skybox for exterior
#include "StaticMesh.h"		// Get static mesh class
#include "SkinnedMesh.h"	// Get anim mesh class
#include "Light.h"

// The map class will be our 3D canvas
class Map : public Object
{
private:
	char*						_name;		// The map name

	Camera*						_camera;	// The main camera
	Skybox*						_skybox;	// The skybox
	Light*						_light;		// The lights
	
	std::vector<Actor*>			_actors;	// Our actor list

	CollisionData::VertexData	_collision_vertex_data;		// The map collision vertex data

public:
	// Default constructor
	inline Map() { _t = MAP; }

	// Initial constructor
	inline Map(char* name)
	{
		_t = MAP;	// Assign our actor tpye to map
		_name = name;	// Assign our name variable
	}

	// Deconstructor will clear all allocated memory
	inline ~Map()
	{
		_actors.clear();	// Clear the actor vector list

		if (_camera) delete _camera;	// Delete camera
		if (_skybox) delete _skybox;	// Delete skybox
	}

	// This will be parsed to the Callback function
	inline void KeyDownCallback()
	{
	}

	// This will be parsed to the Callback function
	inline void KeyUpCallback()
	{
	}

	// This will be parsed to the Callback function
	inline void MouseMotionCallback()
	{
		if (Mouse::IsLeftClick() && GetAsyncKeyState(VK_CONTROL) & 0x8000)
			_camera->ProccessMouseRotation(Mouse::GetOffsetX(), Mouse::GetOffsetY(), CAMERA_LOOK_SENSITIVITY);	// Update the camera rotation in respect to the mouse
	}

	// Get the camera object
	inline Camera* GetCamera()
	{
		return _camera;		// Return camera
	}

	// Get the skybox object
	inline Skybox* GetSkybox()
	{
		return _skybox;		// Return skybox
	}

	// Get the list of actors
	inline std::vector<Actor*> &GetActors()
	{
		return _actors;		// Return the list of actors
	}

	// Get the collision vertex data
	inline CollisionData::VertexData &GetCollisionVertexData()
	{
		return _collision_vertex_data;		// Return the collision data
	}

	// Set the collision vertex data
	inline void SetCollisionData(CollisionData::VertexData value)
	{
		_collision_vertex_data = value;		// Assign the collision data
	}

	// Insert actor to vector
	inline void AddActor(Actor* actor, unsigned int ptr_type)
	{
		switch (ptr_type)
		{
		case CAMERA:	// If type camera
			_camera = ((Camera*)actor);		// Assign camera location
			break;
		case SKYBOX:	// If type camera
			_skybox = ((Skybox*)actor);		// Assign camera location
			break;
		case LIGHT:
			_light  = ((Light*)actor);		// Assign light location
			break;
		}			

		_actors.push_back(actor);	// Push back allocated memory to vector list
	}

	inline void SetName(char* name) { _name = name; }

	// The update function will check for logic
	virtual inline void Update(double &delta)
	{		
		_camera->UpdateInterpolation(delta, _collision_vertex_data);	// Update camera interpolation and check for collision

		for (Actor* a : _actors)	// Iterate through our actor list...
			a->Update(delta);		// Update all of the actors
	}

	// This will render all actors in the world
	virtual inline void Render() {}
};

#endif