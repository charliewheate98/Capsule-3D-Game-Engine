#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#define CT_POLYGON		1
#define CT_EDGE			2
#define CT_VERTEX		3

#include "Globals.h"	// Get access to default values
#include "Interpolate.h"	// Get access to interpolation functions
#include "Collision.h"	// Get access to our collision detection functions


// This namespace will contain functions for collision response
namespace Response
{
	// A function that checks for collision and responds by adjusting the velocity vector
	inline void CheckCollision(glm::vec3 &in_position, glm::vec3 &in_velocity, glm::vec3 look_vector, float &in_speed, double &delta, CollisionData::VertexData vertex_data)
	{
		unsigned int			collision_arbitary = 0;		// Collision arbitary to check geometry type
		unsigned int			arbitary_index = 0;
		bool					near_collision = false;		// To check if response is worth calculating
		bool					colliding = false;	// Create a bool to check if colliding
		bool					stuck = false;	// To check is it's worth interpolating
		float					new_speed = in_speed;	// Create a new speed for later adjustment
		float					slide_intensity = 0.0f;	// Create slide intensity to check angle between look vector and normal
		glm::vec3				new_look_vector = look_vector;	// Create a new look vector for later adjustment
		glm::vec3				slide_plane = glm::vec3(0.0f);	// Create a slide plane normal for new velocity

		// Iterate through each triangle...
		for (unsigned int i = 0; i < vertex_data.triangles.size(); i++)
		{
			// Check for near by triangles - radius is the value of speed
			if (Collision::NearCollision(in_speed, in_position, vertex_data.triangles[i].points[0], vertex_data.triangles[i].points[1], vertex_data.triangles[i].points[2]))
			{
				near_collision = true;	// Set near collision to true

				// If the triangles orientation is for tangent based collision...
				if (Collision::TangentCollision(vertex_data.triangles[i]))
				{
					// Pre-calculate the dot and intersection point
					bool triangle_hit = Collision::IntersectPointTriangle(in_position, vertex_data.triangles[i].origin, vertex_data.triangles[i].normal, vertex_data.triangles[i].points);

					// If the point has intersected with a triangle...
					if (triangle_hit)
					{
						collision_arbitary = CT_POLYGON;	// Assign collision arbitary type
						colliding = true;	// Set colliding to true!
						arbitary_index = i;
					}
				}
			}
		}

		// If collision is near, then check for edges and points
		if (near_collision)
		{
			// Iterate through each edge...
			for (unsigned int i = 0; i < 3; i++)	// TEMP !!!!
			{
				//nearest_edge = Collision::NearestEdge(in_position, vertex_data.triangles[i]);	// Nearest edge index
				bool edge_hit = Collision::IntersectPointEdge(in_position, look_vector, vertex_data.triangles[0].edges[i]);	// Check if point has hit one of three edges

																															// If the point has intersected with an edge...
				if (edge_hit)
				{
					collision_arbitary = CT_EDGE;	// Assign collision arbitary type
					colliding = true;	// Set colliding to true!
					arbitary_index = i;
				}
			}
		}

		// Check what type of arbitary we're colliding with, and assign slide plane accordingly
		switch (collision_arbitary)
		{
		case CT_POLYGON:
			slide_plane = Collision::CalcPlaneSlideVelocity(in_position, look_vector, in_velocity, vertex_data.triangles[arbitary_index].origin, vertex_data.triangles[arbitary_index].normal);	// Get the slide plane normal
			break;
		case CT_EDGE:
			slide_plane = Collision::CalcEdgeSlideVelocity(in_position, look_vector, in_velocity, vertex_data.triangles[0].edges[arbitary_index]);	// Get the slide edge normal
			break;
		case CT_VERTEX:
			break;
		default:
			break;
		}

		// IF we're colliding...
		if (colliding)
		{
			slide_intensity = glm::abs(glm::dot(look_vector, slide_plane));		// Calculate the angle between look vector and normal

			// If the look vector is not directly parallel to the triangle normal...
			if (slide_plane != glm::vec3(0.0f))
			{
				// Assign our new speed and new velocity variables
				new_speed = in_speed * slide_intensity;	// Adjust speed to the slide intensity
				new_look_vector = slide_plane;	// Assign new look vector to slide plane
			}
			else
				stuck = true;
		}

		// If the the point is not stuck...
		if (!stuck)
			Interpolate::Linestep(in_position, in_velocity, new_look_vector, new_speed, delta);	// Interpolate object
	}

	// This function will check for gravity and ground collision
	inline void CheckGravity()
	{

	}

	// This function checks for world collision and responds via sliding
	inline void CheckWorldCollision(glm::vec3 &in_position, glm::vec3 &in_velocity, glm::vec3 look_vector, float &in_speed, double &delta, CollisionData::VertexData vertex_data)
	{
		CheckCollision(in_position, in_velocity, look_vector, in_speed, delta, vertex_data);	// Check for structural collision
		CheckGravity();		// Check for gravity adjustments for when falling or colliding with the ground
	}
}

#endif