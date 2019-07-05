#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "UI.h"	// Get access to the UI controls
#include "Deferred.h"	// Get access to shader data

// This class will contain all of the components required for operating a fully functional level editor
class Editor
{
public:
	static std::vector<unsigned int>	_selected_actors;	// A list of all the currently selected actors

public:
	// Initialise the entire editor and all it's components
	static void Initialise()
	{
		UI::Initialise();	// Initialise the UI
	}

	// Set selected actor
	static inline void SetSelectedActor(unsigned int id, bool value)
	{
		Content::_map->GetActors()[id]->SetSelected(value);	// Set selected actor bool to value
	}

	// Set all selected actors
	static inline void SetAllSelectedActors(bool value)
	{
		for (unsigned int i = 0; i < _selected_actors.size(); i++)	// Iterate through selected actor id
			Content::_map->GetActors()[_selected_actors[i]]->SetSelected(value);	// Set selected actor bool to value
	}

	// Use a callback function for key / mouse events
	static inline void Callback(UINT &message, WPARAM &wParam)
	{
		ShowCursor(TRUE);

		switch (message)		// Iterate through callback events
		{
		case WM_KEYDOWN:
		{
			Keyboard::SetKeyDown(wParam);	// Set the current keycode set to down
			UI::Callback(message, wParam);		// Update UI callback events

			if (!UI::_controls[0]->active)	// If the console is NOT active...
			{
				Content::_map->KeyDownCallback();	// Update the key down functions in map

				// ----------------------------------------------- GLOBAL VIEW TYPE -----------------------------------------------
				if (GetAsyncKeyState('1') & 0x8000)	// If key 1 was pressed
					Deferred::_current_view_t = VT_REALISTIC;	// Adjust view type
				else if (GetAsyncKeyState('2') & 0x8000)	// If key 2 was pressed
					Deferred::_current_view_t = VT_UNLIT;	// Adjust view type
				else if (GetAsyncKeyState('3') & 0x8000)	// If key 3 was pressed
				{
					Deferred::_current_view_t = VT_WIRE;	// Adjust view type
					((GeometryPass*)Deferred::passes[GEOMETRY_PASS])->SetWireMode(true);
				}
				else if (GetAsyncKeyState('4') & 0x8000)	// If key 4 was pressed
					Deferred::_current_view_t = VT_DETAIL;	// Adjust view type
				else if (GetAsyncKeyState('5') & 0x8000)	// If key 6 was pressed
					Deferred::_current_view_t = VT_SSAO;	// Adjust view type

				// Re-adjust bloom depending on buffer view
				if (Deferred::_current_view_t != VT_REALISTIC)
					static_cast<Bloom*>(Deferred::post_effects[0])->SetBloomIntensity(0.0f);
				else
					static_cast<Bloom*>(Deferred::post_effects[0])->SetBloomIntensity(BLOOM_INTENSITY);

				// Re-adjust polygon mode depending on buffer
				if (Deferred::_current_view_t != VT_WIRE)
					((GeometryPass*)Deferred::passes[GEOMETRY_PASS])->SetWireMode(false);


				// ----------------------------------------------- IMPOSE STATIC MESH TO LEVEL -----------------------------------------------
				if (Keyboard::GetKey('R').down)		// Add our mesh from the content to the world
				{
					Content::_map->GetActors().push_back(Content::_meshes[Content::_meshes.size() - 1]);		// Choose the mesh from our content and add it to the world actor list
				}
				// ----------------------------------------------- SAVE STATIC MESH -----------------------------------------------
				if (Keyboard::GetKey('M').down)		// Save a mesh file
				{
					std::string file_name = "Cube" + static_cast<std::string>(__STATIC_MESH_EXTENSION__);		// Set up the name + extension type
					if (!DataIO::SaveAs::MeshO(Content::_meshes[Content::_meshes.size() - 1], file_name.c_str()))		// If the mesh failed to save to a file
						std::cout << "Filed to save mesh file!\n";
				}

				// ----------------------------------------------- IMPORT STATIC MESH -----------------------------------------------
				if (Keyboard::GetKey('O').down)		// Open a mesh file
				{
					std::string file_name = "Cube" + static_cast<std::string>(__STATIC_MESH_EXTENSION__);		// Set up the name + extension type
					if (!DataIO::Open::MeshI(Deferred::shaders[0]->GetProgram(), file_name.c_str()))		// If the mesh failed to save to a file
						std::cout << "Filed to open mesh file!\n";
				}

				// ----------------------------------------------- ACTIVATE COLLISION TO ALL MESHES -----------------------------------------------
				if (Keyboard::GetKey('C').down)
				{
					//std::vector<CollisionData::TriangleData> td;	// This will contain ALL triangles from ALL geometry

					//for (unsigned int i = 0; i < Content::_map->GetActors().size(); i++)
					//{
					//	for (unsigned int j = 0; j < Content::_map->GetActors()[i]->GetCollisionData().size(); j++)
					//		td.push_back(Content::_map->GetActors()[i]->GetCollisionData()[j]);
					//}

					//Content::_map->SetCollisionData(CollisionData::CompileTriangleData(td));

					Content::_map->GetActors()[Content::_map->GetActors().size() - 1]->SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
					Content::_map->GetActors()[Content::_map->GetActors().size() - 1]->UpdateModel();
				}
			}
			break;
		}

		case WM_KEYUP:
		{
			Keyboard::SetKeyUp(wParam);		// Set the current keycode set to up
			Content::_map->KeyUpCallback();	// Update the key up functions in map
			break;
		}

		case WM_LBUTTONUP: 
		{ 
			Mouse::SetLeftClick(false);

			ShowCursor(TRUE);	// Show cursor
			
			break;
		}
		case WM_LBUTTONDOWN:
		{ 
			Mouse::SetLeftClick(true);
			
			if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
				ShowCursor(FALSE);	// Show cursor

			// Check for selected actors
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)	// If control is down
			{
				unsigned int id = 0; //= Deferred::_cpicker->_selected_id;		// Get reference
				if (id != -1)	// If an actor has been selected
				{
					if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)	// Check for multiple selected actors
					{
						bool replicate(false);	// A bool to check for selection replicate
						for (unsigned int i = 0; i < _selected_actors.size(); i++)	// Iterate through each selected actor
						{
							if (id == _selected_actors[i])	// If the selected id is the same as an already selected id
							{
								replicate = true;	// Set replicate to true
								SetSelectedActor(id, false);		// Set actor to be deselected
								_selected_actors.erase(_selected_actors.begin() + i);	// Erase the already selected actor id

								break;	// Break from loop
							}
						}

						if (!replicate)		// If there is no replicate id
						{
							_selected_actors.push_back(id);		// Add the selected actor id to container
							SetSelectedActor(id, true);		// Set actor to be selected
						}
					}
					else
					{
						if (!_selected_actors.empty())	// If there's already an actor selected
						{
							SetAllSelectedActors(false);	// Set all selected actors to false
							_selected_actors.clear();		// Reset size of container
						}
						
						_selected_actors.push_back(id);		// Assign the selected actor id to container
						SetSelectedActor(id, true);		// Set actor to be selected
					}
				}
				else	// Otherwise if nothing was selected
				{
					SetAllSelectedActors(false);	// Set all selected actors to false
					_selected_actors.clear();	// Clear all selected actors
				}
			}


			break; 
		}
		case WM_RBUTTONUP:
		{ 
			Mouse::SetRightClick(false);	// Set right click to false

			ShowCursor(TRUE);	// Show cursor
			
			break;
		}	
		case WM_RBUTTONDOWN: 
		{ 
			Mouse::SetRightClick(true); 

			ShowCursor(FALSE);	// Show cursor

			break; 
		}

		case WM_MBUTTONUP: { Mouse::SetMiddleClick(false); break; }	// Set middle click to false
		case WM_MBUTTONDOWN: { Mouse::SetMiddleClick(true); break; }	// Set middle click to true

		case WM_LBUTTONDBLCLK:
		{
			//Mouse::SetToggleClick(true);	// Toggle the mouse click on		

			break;
		}

		case WM_MOUSEMOVE:
		{
			if (!UI::_controls[0]->active)	// If the console is NOT active...
			{
				Mouse::UpdateMousePosition();	// Update the mouse position

				Mouse::SetOffsetX(Mouse::GetPointX() - Mouse::GetLastX());
				Mouse::SetOffsetY(Mouse::GetLastY() - Mouse::GetPointY());

				Mouse::SetLastX(Mouse::GetPointX());
				Mouse::SetLastY(Mouse::GetPointY());

				Content::_map->MouseMotionCallback();	// Update the mouse look-rotation in map
			}

			//if (Mouse::IsLeftClick())	// If left mouse button is down
			//{
			//	float sensitivity = CAMERA_PAN_SENSITIVITY * MOUSE_SENSITIVITY;		// Calculate camera pan sensitivity

			//	if (Mouse::IsRightClick())	// If right mouse button is also down
			//	{
			//		Content::_map->GetCamera()->SetCurrentLookVector(RIGHT);	// Set current look vector to front by default
			//		glm::vec3 cam_look_x = Content::_map->GetCamera()->GetCurrentLookVectorV();	// Get temp camera variable data
			//		glm::vec3 cam_look_y = Content::_map->GetCamera()->GetWorldUp();	// Get the world up vector
			//		Content::_map->GetCamera()->GetPosition() += cam_look_x * (sensitivity * (float)Mouse::GetOffsetX());	// Pan camera position in direction of mouse motion
			//		Content::_map->GetCamera()->GetPosition() += cam_look_y * (sensitivity * (float)Mouse::GetOffsetY());	// Pan camera position in direction of mouse motion
			//	}
			//	else	// Otherwise, if left mouse button is down ONLY
			//	{
			//		Content::_map->GetCamera()->SetCurrentLookVector(FRONT);	// Set current look vector to front by default
			//		glm::vec3 cam_look_z = Content::_map->GetCamera()->GetCurrentLookVectorV();	// Get temp camera variable data
			//		Content::_map->GetCamera()->GetPosition() += cam_look_z * (sensitivity * (float)Mouse::GetOffsetY());	// Pan camera position in direction of mouse motion
			//	}
			//}

			// Reset mouse position to prevent camera rotation lock
			if (Mouse::GetPointX() <= 0)
			{
				Mouse::SetCursorPosition(1918, Mouse::GetLastY());		// Rotate +x
			}
			if (Mouse::GetPointX() >= 1919)
			{
				Mouse::SetCursorPosition(1, Mouse::GetLastY());		// Rotate -x
			}
			if (Mouse::GetPointY() <= 0)
			{
				Mouse::SetCursorPosition(Mouse::GetLastX(), 1078);		// Rotate +y
			}
			if (Mouse::GetPointY() >= 1079)
			{
				Mouse::SetCursorPosition(Mouse::GetLastX(), 1);		// Rotate -y
			}
						
			break;
		}

		}
	}

	// Update the current map
	static void Update(double &delta)
	{
		UI::Update(delta);	// Update the UI
	}

	// Render the current map
	static void Render()
	{
		glEnable(GL_BLEND);		// Enable blending
		glDisable(GL_DEPTH_TEST);	// Disable depth test

		UI::Render();	// Render the UI
	}

	// Destroy the Editor components
	static void Destroy()
	{
		UI::Destroy();	// Delete all UI elements
		Content::Destroy();		// Delete all content
	}
};

std::vector<unsigned int>	Editor::_selected_actors;

#endif