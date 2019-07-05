#ifndef __CAMERA_H__
#define __CAMERA_H__

#define ORTHO			0x1		// Define an ortho mode
#define PERSPECTIVE		0x2		// Define a perspective mode

#define NUM_STEPS 1000

#include "Actor.h"		// Extend from our abstrct class
#include "Globals.h"	// Get access to width and height of viewport 
#include "Keyboard.h"	// Assign keycodes and states for our keyboard
#include "Collision.h"	// Include global collision functions
#include <glm/gtx/quaternion.hpp>

// This will be our type of camera directions
enum CameraDirections
{
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	FRONT_RIGHT,
	FRONT_LEFT,
	BACK_RIGHT,
	BACK_LEFT
};

// This is our camera class for moving in Cartesian space
class Camera : public Actor
{
private:
	float		_fov;	// Header/Feild of view
	float		_near;	// Near plane
	float		_far;	// Far plane
	float		_ratio;	// Ratio for perspective mode
	float		_pitch;	// Pitch rotation 
	float		_yaw;	// Yaw rotation
	float		_speed;		// Speed of movement

	bool        _locked; // Checks if the camera is locked

	int			_proj_type;		// Projection type (Ortho or Perspective)
	int			_current_look_vector;	// Tell us what look vector we're using
	
	unsigned int _u_view;	// The view matrix uniform
	unsigned int _u_proj;	// The projection matrix uniform

	glm::vec3	_front;		// Front vector
	glm::vec3	_front_right;	// Front right vector
	glm::vec3	_front_left;	// Front right vector
	glm::vec3	_back_right;	// Front right vector
	glm::vec3	_back_left;	// Front right vector
	glm::vec3	_up;		// Up vector
	glm::vec3	_right;		// Right vector 
	glm::vec3	_world_up;	// World orientation
	glm::vec3	_velocity;	// The velocity vector

	glm::mat4	_view;	// The view matrix
	glm::mat4	_proj;	// The projection matrix

public:
	bool		_looked;	// Check if the camera look vector has just rotated

	// Default constructor
	inline Camera() { _t = CAMERA; }

	// Initial constructor for perspective mode
	inline Camera(unsigned int shader_program, glm::vec3 position, float fov, float speed, float sensitivity, float n, float f, float ratio)
	{
		_t = CAMERA;	// Set actor type

		_u_view = glGetUniformLocation(shader_program, "view");		// Locate our view uniform
		_u_proj = glGetUniformLocation(shader_program, "proj");		// Locate our proj uniform

		_front = glm::vec3(0.0f, 0.0f, 1.0f);	// Initialise the front vector
		_up = glm::vec3(0.0f, 1.0f, 0.0f);	// Initialise the up vector
		_world_up = _up;		// Assign world up to the up vector by default
		_velocity = _front;		// Assign our default velocity vector
		_current_look_vector = FRONT;	// By default we set the look vector to front
		_trans._pos = position;		// Assign position
		_fov = fov;		// Assign feild-of-view
		_near = n;	// Assign near plane
		_far = f;		// Assign far plane
		_ratio = ratio;	// Assign ratio
		_yaw = 0.0f;	// Set the yaw value
		_pitch = 0.0f;	// Set the pitch value
		_speed = speed;		// Set the speed value
		_looked = false;
		_locked = false;

		SetProjectionType(PERSPECTIVE);		// Assign proj type
		UpdateLookVectors();	// Update the current look vectors
	}

	// Initial constructor for orthographic mode
	inline Camera(unsigned int shader_program, glm::vec3 position, float speed, float sensitivity)
	{
		_t = CAMERA;	// Set actor type

		_u_view = glGetUniformLocation(shader_program, "view");		// Locate our view uniform
		_u_proj = glGetUniformLocation(shader_program, "proj");		// Locate our proj uniform

		_front = glm::vec3(0.0f, 0.0f, -1.0f);	// Initialise the front vector
		_up = glm::vec3(0.0f, 1.0f, 0.0f);	// Initialise the up vector
		_world_up = _up;		// Assign world up to the up vector by default
		_velocity = _front;		// Assign our default velocity vector
		_current_look_vector = FRONT;	// By default we set the look vector to front
		_trans._pos = position;		// Assign position
		_fov = 0.0f;		// Assign feild-of-view
		_near = 0.0f;	// Assign near plane
		_far = 0.0f;		// Assign far plane
		_ratio = 0.0f;	// Assign ratio
		_yaw = -90.0f;	// Set the yaw value
		_pitch = 0.0f;	// Set the pitch value
		_speed = speed;		// Set the speed value

		SetProjectionType(ORTHO);		// Assign proj type
		UpdateLookVectors();	// Update the current look vectors
	}

	inline int GetProjectionType() { return _proj_type; }	// Return the projection matrix type

	inline glm::mat4 GetViewMatrix() { return glm::lookAt(_trans._pos, _trans._pos + _front, _up); }	// Function for getting LookAt matrix
	inline glm::mat4 GetProjectionMatrix() { return _proj; }		// Function for getting projection matrix

	inline glm::vec3 &GetFront() { return _front; }	// Return the front vector
	inline glm::vec3 &GetUp() { return _up; }	// Return the up vector
	inline glm::vec3 &GetRight() { return _right; }	// Return the right vector
	inline glm::vec3 &GetWorldUp() { return _world_up; }		// Return the world up vector
	inline glm::vec3 &GetVelocity() { return _velocity; }	// Return the velocity vector
	inline glm::vec3 GetCurrentLookVectorV()	// Get the currrent look vector
	{
		switch (_current_look_vector)	// Switch through different cases...
		{
		case FRONT:		// If current is FRONT... 
			return _front;	// return front vector
		case BACK:	// If current is BACK
			return -_front;		// return -front vector
		case LEFT:		// If current is LEFT
			return -_right;		// return -right vector
		case RIGHT:		// If current is RIGHT
			return _right;	// return right vector
		case FRONT_RIGHT:	// If current is RIGHT
			return _front_right;	// return front right vector
		case FRONT_LEFT:	// If current is RIGHT
			return _front_left;		// return front right vector
		case BACK_RIGHT:	// If current is RIGHT
			return _back_right;		// return front right vector
		case BACK_LEFT:	// If current is RIGHT
			return _back_left;		// return front right vector
		default:	// If current is something else...	
			break;	// Break from switch statement
		}

		return glm::vec3(-1.0f);	// Return -1 as no look vector can be distinguished
	}

	bool objectVisible(const GLfloat MVPMatrix[16], const GLfloat &xPos, const GLfloat &yPos, const GLfloat &zPos, const GLfloat &radius)
	{
		// The equation for a plane is: Ax + By + Cz + D = 0, where A, B and C define the plane's normal vector, D is the distance from the origin to the plane,
		// and x, y and z are any points on the plane.. You can plug any point into the equation and if the result is 0 then the point lies on the plane. If the
		// result is greater than 0 then the point is in front of the plane, and if it's negative the point is behind the plane.
		enum term { A = 0, B, C, D };

		GLfloat leftPlane[4];
		leftPlane[A] = MVPMatrix[3] + MVPMatrix[0];
		leftPlane[B] = MVPMatrix[7] + MVPMatrix[4];
		leftPlane[C] = MVPMatrix[11] + MVPMatrix[8];
		leftPlane[D] = MVPMatrix[15] + MVPMatrix[12];

		// Normalise the plane
		GLfloat length = sqrtf(leftPlane[A] * leftPlane[A] + leftPlane[B] * leftPlane[B] + leftPlane[C] * leftPlane[C]);
		leftPlane[A] /= length;
		leftPlane[B] /= length;
		leftPlane[C] /= length;
		leftPlane[D] /= length;

		// Check the point's location with respect to the left plane of our viewing frustrum
		GLfloat distance = leftPlane[A] * xPos + leftPlane[B] * yPos + leftPlane[C] * zPos + leftPlane[D];
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the left plane
		}

		// Check the point's location with respect to the right plane of our viewing frustum
		GLfloat rightPlane[4];
		rightPlane[A] = MVPMatrix[3] - MVPMatrix[0];
		rightPlane[B] = MVPMatrix[7] - MVPMatrix[4];
		rightPlane[C] = MVPMatrix[11] - MVPMatrix[8];
		rightPlane[D] = MVPMatrix[15] - MVPMatrix[12];

		// Normalise the plane
		length = sqrtf(rightPlane[A] * rightPlane[A] + rightPlane[B] * rightPlane[B] + rightPlane[C] * rightPlane[C]);
		rightPlane[A] /= length;
		rightPlane[B] /= length;
		rightPlane[C] /= length;
		rightPlane[D] /= length;

		distance = rightPlane[A] * xPos + rightPlane[B] * yPos + rightPlane[C] * zPos + rightPlane[D];
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the right plane
		}

		// Check the point's location with respect to the bottom plane of our viewing frustum
		GLfloat bottomPlane[4];
		bottomPlane[A] = MVPMatrix[3] + MVPMatrix[1];
		bottomPlane[B] = MVPMatrix[7] + MVPMatrix[5];
		bottomPlane[C] = MVPMatrix[11] + MVPMatrix[9];
		bottomPlane[D] = MVPMatrix[15] + MVPMatrix[13];

		// Normalise the plane
		length = sqrtf(bottomPlane[A] * bottomPlane[A] + bottomPlane[B] * bottomPlane[B] + bottomPlane[C] * bottomPlane[C]);
		bottomPlane[A] /= length;
		bottomPlane[B] /= length;
		bottomPlane[C] /= length;
		bottomPlane[D] /= length;

		distance = bottomPlane[A] * xPos + bottomPlane[B] * yPos + bottomPlane[C] * zPos + bottomPlane[D];
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the bottom plane
		}

		// Check the point's location with respect to the top plane of our viewing frustrum
		GLfloat topPlane[4];
		topPlane[A] = MVPMatrix[3] - MVPMatrix[1];
		topPlane[B] = MVPMatrix[7] - MVPMatrix[5];
		topPlane[C] = MVPMatrix[11] - MVPMatrix[9];
		topPlane[D] = MVPMatrix[15] - MVPMatrix[13];

		// Normalise the plane
		length = sqrtf(topPlane[A] * topPlane[A] + topPlane[B] * topPlane[B] + topPlane[C] * topPlane[C]);
		topPlane[A] /= length;
		topPlane[B] /= length;
		topPlane[C] /= length;
		topPlane[D] /= length;

		distance = topPlane[A] * xPos + topPlane[B] * yPos + topPlane[C] * zPos + topPlane[D];
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the top plane
		}

		// Check the point's location with respect to the near plane of our viewing frustum
		GLfloat nearPlane[4];
		nearPlane[A] = MVPMatrix[3] + MVPMatrix[2];
		nearPlane[B] = MVPMatrix[7] + MVPMatrix[6];
		nearPlane[C] = MVPMatrix[11] + MVPMatrix[10];
		nearPlane[D] = MVPMatrix[15] + MVPMatrix[14];

		// Normalise the plane
		length = sqrtf(nearPlane[A] * nearPlane[A] + nearPlane[B] * nearPlane[B] + nearPlane[C] * nearPlane[C]);
		nearPlane[A] /= length;
		nearPlane[B] /= length;
		nearPlane[C] /= length;
		nearPlane[D] /= length;

		distance = nearPlane[A] * xPos + nearPlane[B] * yPos + nearPlane[C] * zPos + nearPlane[D];
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the near plane
		}

		// Check the point's location with respect to the far plane of our viewing frustum
		GLfloat farPlane[4];
		farPlane[A] = MVPMatrix[3] - MVPMatrix[2];
		farPlane[B] = MVPMatrix[7] - MVPMatrix[6];
		farPlane[C] = MVPMatrix[11] - MVPMatrix[10];
		farPlane[D] = MVPMatrix[15] - MVPMatrix[14];

		// Normalise the plane
		length = sqrtf(farPlane[A] * farPlane[A] + farPlane[B] * farPlane[B] + farPlane[C] * farPlane[C]);
		farPlane[A] /= length;
		farPlane[B] /= length;
		farPlane[C] /= length;
		farPlane[D] /= length;

		distance = farPlane[A] * xPos + farPlane[B] * yPos + farPlane[C] * zPos + farPlane[D];
		if (distance <= -radius)
		{
			return false; // Bounding sphere is completely outside the far plane
		}

		// If we got here, then the bounding sphere is within at least all six sides of the view frustum, so it's visible and we should draw it!
		return true;
	}

	// Return the current direction at which the camera is moving
	inline int GetCurrentLookVector()
	{
		if (!_locked)
		{
			if (Keyboard::GetKey('W').down)		// Check front vector
			{
				if (Keyboard::GetKey('D').down)		// Check front right vector
				{
					_current_look_vector = FRONT_RIGHT;
				}
				else if (Keyboard::GetKey('A').down)	// Check front left vector
				{
					_current_look_vector = FRONT_LEFT;
				}
				else
					_current_look_vector = FRONT;
			}
			else if (Keyboard::GetKey('S').down)	// Check back vector
			{
				if (Keyboard::GetKey('D').down)		// Check back right vector
				{
					_current_look_vector = BACK_RIGHT;
				}
				else if (Keyboard::GetKey('A').down)	// Check back left vector
				{
					_current_look_vector = BACK_LEFT;
				}
				else
					_current_look_vector = BACK;
			}
			else if (Keyboard::GetKey('A').down)	// Check left vector
				_current_look_vector = LEFT;
			else if (Keyboard::GetKey('D').down)	// Check right vector
				_current_look_vector = RIGHT;
			else
				return -1;	// Otherwise return -1 for no input
		}

		return _current_look_vector;	// Return the final result
	}

	inline bool IsMoving() { return (GetCurrentLookVector() != -1); }			// Check if key input has been pressed for moving camera

	inline float &GetFov() { return _fov; }	// Get our fov value
	inline float &GetNear() { return _near; }	// Get our near value
	inline float &GetFar() { return _far; }	// Get our far value
	inline float &GetRatio() { return _ratio; }	// Get our ratio value
	inline float &GetYaw() { return _yaw; }	// Get our _yaw value
	inline float &GetPitch() { return _pitch; }	// Get our _pitch value
	inline float &GetSpeed() { return _speed; }	// Get speed value

	inline void SetFov(float value) { _fov = value; }	// Assign a value to our fov
	inline void SetNear(float value) { _near = value; }		// Assign a value to our near
	inline void SetFar(float value) { _far = value; }	// Assign a value to our far
	inline void SetRatio(float value) { _ratio = value; }	// Assign a value to our ratio
	inline void SetYaw(float value) { _yaw = value; }	// Assign a value to our _yaw
	inline void SetPitch(float value) { _pitch = value; }	// Assign a value to our _pitch
	inline void SetSpeed(float value) { _speed = value; }	// Assign speed value
	inline void SetFront(glm::vec3 value) { _front = value; }	// Assign a value to front
	inline void SetUp(glm::vec3 value) { _up = value; }	// Assign a value to up
	inline void SetRight(glm::vec3 value) { _right = value; }	// Assign a value to right
	inline void SetVelocity(glm::vec3 value) { _velocity = value; }		// Assign the velocity vector
	inline void SetCurrentLookVector(int value) { _current_look_vector = value; }	// Assign the current look vector

	inline void IncrementYaw(float value) { _yaw += value; }	// We can use this to interpolate the yaw value
	inline void IncrementPitch(float value) { _pitch += value; }	// We can use this to interpolate the pitch value
	inline void SetProjection(glm::mat4 value) { _proj = value; }		// Assign a value to the projection matrix

	// A function that can switch the projection type
	inline void SetProjectionType(unsigned int projection_type)
	{
		_proj_type = projection_type;	// Assign proj type

		if (projection_type == ORTHO)	// If the projection type is set to ORTHO...
			SetProjection(glm::ortho(0.0f, (float)_vp_width, 0.0f, (float)_vp_height));	// Assign the proj to an ortho projection type matrix
		else	// If the projection type is set to PROJECTION or otherwise...
			SetProjection(glm::perspective(_fov, _ratio, _near, _far));	// Assign the proj to a perspective projection type matrix
	}

	// A function that updates a camera's orientation vectors
	inline void UpdateLookVectors()
	{
		glm::vec3 front;	// This will be our normalised front vector

		front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));	//	Set the X value in front vector for cosine yaw and pitch
		front.y = sin(glm::radians(_pitch));	// Set the Y value in front vector for sine pitch
		front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));	// Set the Y value in front vector for sine yaw and cosine pitch

		_front = glm::normalize(front);		// Normalise the _front vector 
		_right = glm::normalize(glm::cross(_front, _world_up));		// Now use this normalised front vector to cross with the world_up vector in order to calculate the right vector
		_up = glm::normalize(glm::cross(_right, _front));	// Apply the same formula to get the up vector

		_front_right = (_front + _right) / 2.0f;	// Calculate front right vector
		_front_left = (_front - _right) / 2.0f;	// Calculate front left vector
		_back_right = (-_front + _right) / 2.0f;	// Calculate back right vector
		_back_left = (-_front - _right) / 2.0f;	// Calculate back left vector
	}

	// Proccess the mouse input for camera look rotation
	inline void ProccessMouseRotation(double x_offset, double y_offset, float sensitivity, bool constrain_pitch = true)
	{
		if(!_locked)
		{
			x_offset *= (sensitivity * MOUSE_SENSITIVITY);	// Attenuate the x_offset with our sensitivity value
			y_offset *= (sensitivity * MOUSE_SENSITIVITY);	// Attenuate the y_offset with our sensitivity value
	
			_yaw += (float)x_offset;	// Increment our camera _yaw with the mouse x_offset
			_pitch += (float)y_offset;	// Increment our camera _pitch with the mouse y_offset

			if (_yaw > 360.0f)	_yaw = 0.0f;	// Restrain yaw to 360 degree spin
			if (_yaw < 0.0f)	_yaw = 360.0f;	// Restrain yaw to 360 degree spin

			if (constrain_pitch)		// if pitch is constrained...
			{
				if (_pitch > 89.0f)		_pitch = 89.0f;		// Constrain up pitch
				if (_pitch < -89.0f)	_pitch = -89.0f;	// Constrain down pitch
			}

			UpdateLookVectors();	// Update our look vectors
		}
	}

	// Update any changes made to one of the projection parameters
	inline void UpdateProjectionMatrix()
	{
		SetProjectionType(_proj_type);	// Set the projection to it's specific type
	}

	// Create a new input function for assigning different look directions
	inline void UpdateInterpolation(double &delta, CollisionData::VertexData &vertex_data)
	{
		if (IsMoving())		// If the camera is actively moving...
			Response::CheckWorldCollision(_trans._pos, _velocity, GetCurrentLookVectorV(), _speed, delta, vertex_data);		// Check for collision whilst moving
	}

	// Override virtual functions
	inline virtual void Update(double &delta)
	{
		// if camera is locked then dont equal to lookAt. 
		// Else if it is not locked, then use quaternion interpolation

		if (!_locked)
			_view = lookAt(_trans._pos, _trans._pos + _front, _up);		// Update the camera view position
		else
		{
			//if (current_step != _steps.size() - 1)
			//{
			//	// ROTATION ------------------------------------------------------------------------------------------
			//	glm::vec3 cameraRot(_pitch, _yaw, 0.0f);

			//	float rot_step_len_x = glm::length(_rot_steps[current_rot_step_x + 1].x - _rot_steps[current_rot_step_x].x);
			//	float rot_step_len_y = glm::length(_rot_steps[current_rot_step_y + 1].y - _rot_steps[current_rot_step_y].y);
	
			//	cameraRot.x = _rot_steps[current_rot_step_x].x;
			//	cameraRot.y = _rot_steps[current_rot_step_y].y;

			//	float time_x = glm::length((cameraRot.x - _rot_steps[current_rot_step_x + 1].x) / rot_step_len_x);
			//	float time_y = glm::length((cameraRot.y - _rot_steps[current_rot_step_y + 1].y) / rot_step_len_y);
	
			//	if (time_x >= 1.0f)
			//		current_rot_step_x++;
			//	if (time_y >= 1.0f)
			//		current_rot_step_y++;
			//	// -------------------------------------------------------------------------------------------------------
			//	
			//	// TRANSLATION ------------------------------------------------------------------------------------------
			//	glm::vec3 step_dir = glm::normalize(_steps[current_step + 1] - _steps[current_step]);
			//	float step_len = glm::length(_steps[current_step + 1] - _steps[current_step]);

			//	_trans._pos += step_len * step_dir * ((float)delta * 200.0f);

			//	float t = glm::length((_trans._pos - _steps[current_step + 1]) / step_len);

			//	if (t >= 1.0f)
			//		current_step++;
			//	// -------------------------------------------------------------------------------------------------------


			//	_view = glm::rotate(cameraRot.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(cameraRot.y, glm::vec3(0.0f, 1.0f, 0.0f))
			//		* glm::translate(-_steps[current_step]);
			//}
		}
	}

	inline virtual void Render()
	{
		glUniformMatrix4fv(_u_view, 1, GL_FALSE, value_ptr(_view));		// Parse the view uniform data
		glUniformMatrix4fv(_u_proj, 1, GL_FALSE, value_ptr(_proj));		// Parse the projection uniform data
	}
};

#endif