#ifndef __COLLISION_H__
#define __COLLISION_H__

#define ELIPSOID_SPACE	1.0f

#include "CollisionData.h"	// Get access to our collision data structs


// This namespace will handle collision functionality
namespace Collision
{
	// This function checks if the triangle is orientated enough to block
	inline bool TangentCollision(CollisionData::TriangleData triangle_data)
	{
		return (triangle_data.normal.y <= abs(0.5f));
	}


	// -----------------------------------------------------------------------------------------  TRIANGLE ----------------------------------------------------------------------------------------- //

	// This function will check if a collision is near
	inline bool NearCollision(float n, glm::vec3 position, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
	{
		float a = glm::distance(v0, position);		// Get distance from vertex a
		float b = glm::distance(v1, position);		// Get distance from vertex b
		float c = glm::distance(v2, position);		// Get distance from vertex c

		return ((a < n) || (b < n) || (c < n));	// Return true if one of the points is close to position
	}

	// Return the barrymetric coord between a point and triangle area
	inline glm::vec3 BarymetricCoord(glm::vec3 point_origin, glm::vec3 point_direction, glm::vec3 triangle_origin, glm::vec3 triangle_normal, glm::vec3 triangle_vertices[3])
	{
		glm::vec3 v0 = triangle_vertices[0] - point_origin;		// Get ray from vertex 0 to point
		glm::vec3 v1 = triangle_vertices[1] - point_origin;		// Get ray from vertex 1 to point
		glm::vec3 v2 = triangle_vertices[2] - point_origin;		// Get ray from vertex 2 to point

		glm::vec3 v01 = glm::cross(v0, v1);	// Get perpendicular value between v0 and v1
		glm::vec3 v12 = glm::cross(v1, v2);	// Get perpendicular value between v1 and v2
		glm::vec3 v20 = glm::cross(v2, v0);	// Get perpendicular value between v2 and v0

		float v01d = glm::dot(v01, point_direction);		// Get the dot between perpendicular values of v0 and v1, and the ray velocity
		float v12d = glm::dot(v12, point_direction);		// Get the dot between perpendicular values of v1 and v2, and the ray velocity
		float v20d = glm::dot(v20, point_direction);		// Get the dot between perpendicular values of v2 and v0, and the ray velocity

		return glm::vec3(v01d, v12d, v20d);		// Return final Barymetric result
	}

	// Return the distance between ray to plane intersection
	inline float IntersectRayPlane(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 plane_origin, glm::vec3 plane_normal)
	{
		float plane_dot = glm::dot(plane_normal, plane_origin - ray_origin);	// Get the dot between the plane normal and the object origin
		float point_dot = glm::dot(ray_direction, plane_normal);	// Get the dot between the object normal and the plane normal
		float dist = plane_dot / point_dot;	// // Divide the plane dot and the object dot to get the distance scalar

		if (dist < 0)	// If the ray is behind the plane normal...
			return -1;	// Return negative value

		return	dist;	// Otherwise return the distance scalar
	}

	// Return the true if ray to triangle intersects
	inline bool IntersectRayTriangle(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 triangle_origin, glm::vec3 triangle_normal, glm::vec3 triangle_vertices[3])
	{
		if (IntersectRayPlane(ray_origin, ray_direction, triangle_origin, triangle_normal) != -1)	// If ray direction is facing and perpendicular to plane normal...
		{
			glm::vec3 bary_coord = BarymetricCoord(ray_origin, ray_direction, triangle_origin, triangle_normal, triangle_vertices);		// Calculate the barymetric coord
			
			if (glm::all(glm::lessThan(bary_coord, glm::vec3(0.0f))))	// If the ray intersects triangle...
				return true;	// Return true as there is intersection
		}

		return false;	// Return false as there is no intersection
	}

	// Return true if the point is being blocked by triangle
	inline bool IntersectPointTriangle(glm::vec3 point_origin, glm::vec3 triangle_origin, glm::vec3 triangle_normal, glm::vec3 triangle_vertices[3])
	{
		if (IntersectRayTriangle(point_origin, -triangle_normal, triangle_origin, triangle_normal, triangle_vertices))	// If the point intersects with triangle projection...
		{
			if (IntersectRayPlane(point_origin, -triangle_normal, triangle_origin, triangle_normal) <= ELIPSOID_SPACE)		// If the distance between ray and triangle is less or equal to 0...
				return true;	// Return true as point collides with triangle!
		}

		return false;	// Return false as nothing was blocked
	}

	// Return a new velocity vector along an intersected plane
	inline glm::vec3 CalcPlaneSlideVelocity(glm::vec3 point_origin, glm::vec3 point_direction, glm::vec3 point_velocity, glm::vec3 plane_origin, glm::vec3 plane_normal)
	{
		glm::vec3 intersect_point = point_origin;	// Get current point as intersection point
		glm::vec3 destination = point_origin + glm::length(glm::normalize(point_velocity)) * point_direction;	// Get the current destination point

		float dest_angle = glm::clamp(glm::dot(plane_normal, (plane_origin + ELIPSOID_SPACE * plane_normal) - destination), 0.0f, 1.0f);	// Get the dot between the plane normal and the object origin

		if (dest_angle >= 1.0f)		// If the look angle is almost parallel to normal...
			return glm::vec3(0.0f);		// Return 0

		glm::vec3 new_destination = destination + dest_angle * plane_normal;	// Get new destination point
		glm::vec3 slide_plane_normal = glm::normalize(new_destination - intersect_point);	// Create a direction velocity vector using new destination minus intersection point

		return slide_plane_normal;	// Return the new velocity vector
	}


	// -----------------------------------------------------------------------------------------  EDGE ----------------------------------------------------------------------------------------- //

	// This function will return true if point is within the perpendicular range of edge
	inline bool PointInEdgeParam(glm::vec3 point_origin, CollisionData::Edge edge)
	{
		glm::vec3 ab = edge.p[1] - edge.p[0];	// Get the ab values from B to A (the edge size)

		float ab_len = glm::length(ab);	// Get the magnitude of ab |/ 2
		float inner_product = glm::dot(point_origin - edge.p[0], ab);	// Get the dot product between p - a and ab

		return (inner_product >= 0 && inner_product <= ab_len * ab_len);	// Return true if inner product is greater than a but less than b
	}

	// Return true if a point has intersected with an edge
	inline bool IntersectPointEdge(glm::vec3 point_origin, glm::vec3 point_velocity, CollisionData::Edge edge)
	{
		if (IntersectRayPlane(point_origin, point_velocity, edge.o, edge.tbn[2]) != -1)	// If point is in front of plane and velocity direction is perpendicular to plane normal...
		{
			if (PointInEdgeParam(point_origin, edge))	// If the point is within the perpendicular zone of edge...
			{
				glm::vec3 alignment = glm::cross(edge.tbn[0], point_origin - edge.p[0]); 	// Get the cross between b - a and c - a for alignment
				float dist = glm::length(alignment);	// Get the magnitude of cross product

				if (dist <= ELIPSOID_SPACE)		// If the distance between point and edge is less or equal to elipsoid space...
					return true;	// Return true as point as intersected with edge!
			}
		}

		return false; 	// Otherwise, return false as p has not intersected
	}

	// Returns the index of closest edge from point
	//inline CollisionData::Edge NearestEdge(glm::vec3 point_origin, CollisionData::TriangleData &triangle)
	//{
	//	CollisionData::Edge a, b, c;	// Create our temp edges

	//	a = triangle.edges[0];	// Assign a to edge 0
	//	b = triangle.edges[1];	// Assign b to edge 1
	//	c = triangle.edges[2];	// Assign c to edge 2

	//	float a_len = glm::length(a.o - point_origin);	// Get the dist between point to edge a
	//	float b_len = glm::length(b.o - point_origin);	// Get the dist between point to edge b
	//	float c_len = glm::length(c.o - point_origin);	// Get the dist between point to edge c

	//	CollisionData::Edge r = a;	// Create a vairable for the result

	//	if (a_len < b_len)	// If a is less than b...
	//	{
	//		r = a;	// Assign r to a

	//		if (c_len < a_len)	// If c is less than a...
	//			r = c;	// Assign r to c
	//	}
	//	else	// Otherwise, if b is less than a...
	//	{
	//		r = b;	// Assign r to b

	//		if (c_len < b_len)	// If c is less than b...
	//			r = c;	// Assign r to c
	//	}

	//	return r;	// Return the final result
	//}

	// This function will return the nearest point on edge, perpendicular to the origin
	inline glm::vec3 NearestPointToEdge(glm::vec3 point_origin, CollisionData::Edge edge)
	{
		glm::vec3 ab = edge.p[1] - edge.p[0];	// Calculate the area from point A to point B

		float edge_len = glm::length(ab);	// Calculate the magnitude of ab (length of the edge)
		float ap_dot = glm::clamp(glm::dot(edge.tbn[0], point_origin - edge.p[0]), 0.0f, edge_len);	// Calculate magnitude between edge tangent and (player - A), clamped from 0.0f to length of edge

		return edge.p[0] + edge.tbn[0] * ap_dot;	// Return A plus tangent (direction vector) times by the magnitude between edge tangent and (player - A), clamped from 0.0f to length of edge
	}

	// Return a new velocity vector along an intersected edge
	inline glm::vec3 CalcEdgeSlideVelocity(glm::vec3 point_origin, glm::vec3 point_direction, glm::vec3 point_velocity, CollisionData::Edge edge)
	{
		glm::vec3 intersect_point = point_origin;	// Get current point as intersection point
		glm::vec3 destination = point_origin + glm::length(glm::normalize(point_velocity)) * point_direction;	// Get the current destination point
		
		glm::vec3 nearest_intersection = NearestPointToEdge(point_origin, edge);	// Get the nearest point to edge from point origin
		glm::vec3 edge_normal = glm::normalize(intersect_point - nearest_intersection);	// Calculate an edge normal

		float dest_angle = glm::clamp(glm::dot(edge_normal, (nearest_intersection + ELIPSOID_SPACE * edge_normal) - destination), 0.0f, 1.0f);	// Get the dot between the plane normal and the object origin

		if (dest_angle >= 1.0f)		// If the look angle is almost parallel to normal...
			return glm::vec3(0.0f);		// Return 0

		glm::vec3 new_destination = destination + dest_angle * edge_normal;	// Get new destination point
		glm::vec3 slide_edge_normal = glm::normalize(new_destination - intersect_point);	// Create a direction velocity vector using new destination minus intersection point

		return slide_edge_normal;	// Return the new velocity vector
	}


	// -----------------------------------------------------------------------------------------  POINT ----------------------------------------------------------------------------------------- //
}

#endif