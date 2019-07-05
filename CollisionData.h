#ifndef __COLLISION_DATA_H__
#define __COLLISION_DATA_H__

#define EDGE_TYPE_RIGHT		0x0
#define EDGE_TYPE_OPTUSE	0x1
#define EDGE_TYPE_ACUTE		0x2


#include "Math.h"	// Include our math header


// A namespace to hold all structure types
namespace CollisionData
{
	// A class for containing edge data
	struct Edge
	{
		glm::vec3 o; 		// Store the origin of the edge
		glm::vec3 p[2]; 	// Store the point values
		glm::vec3 e;		// Store the edge value
		glm::vec3 tbn[3]; 	// Store the tangent and bitangent values

							// Default constructor
		inline Edge() {}

		// Initial constructor
		inline Edge(glm::vec3 a, glm::vec3 b, glm::vec3 normal)
		{
			o = a + b / 2.0f; 	// Get the mid point of edge

			p[0] = a; 	// Assign point a
			p[1] = b; 	// Assign point b

			e = p[1] - p[0];	// Calculate the edge

			tbn[0] = glm::normalize(e); 	// Calculate the tangent
			tbn[1] = glm::cross(tbn[0], normal); 	// Calculate the bitangent
			tbn[2] = normal; 	// Assign the normal
		}
	};

	// A structure for containing the triangle vertices and other important values for collision
	struct TriangleData
	{
		glm::vec3	points[3];	// A container for our 3 points
		glm::vec3	origin;		// The triangle origin
		glm::vec3	normal;		// The triangle normal
		Edge		edges[3];	// Edge data

		// Default constructor
		inline TriangleData() {}

		// Initial constructor
		inline TriangleData(glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			points[0] = a;	// Assign vertex a
			points[1] = b;	// Assign vertex b
			points[2] = c;	// Assign vertex c

			origin = Math::AverageTriangle(points);	// Calculate the origin
			normal = Math::CalcNormal(points);	// Calculate the normal

			edges[0] = Edge(a, b, normal);	// Assign edge ab
			edges[1] = Edge(b, c, normal);	// Assign edge ba
			edges[2] = Edge(c, a, normal);	// Assign edge ca
		}
	};

	// A structure that contains key data for an optimised collision object
	struct VertexData
	{
		std::vector<TriangleData> triangles;	// A dynamic container for our triangle data

		// Default constructor
		inline VertexData() {}

		// Initial constructor
		inline VertexData(std::vector<glm::vec3> vertices)
		{
			for (unsigned int i = 0; i != vertices.size(); i += 3)	// For each three vertices...
				triangles.push_back(TriangleData(vertices[i], vertices[i + 1], vertices[i + 2]));	// Create a triangle
		}
	};

	// This function will compile a list of vertex data and return an optimised version of vertex data
	inline CollisionData::VertexData CompileTriangleData(std::vector<CollisionData::TriangleData> in_triangle_data)
	{
		CollisionData::VertexData vd_opt;	// Create a vertex data object ready for optimising

		for (unsigned int i = 0; i < in_triangle_data.size(); i++)	// Iterate through each triangle...
			vd_opt.triangles.push_back(in_triangle_data[i]);	// Assign all triangles to one vertex data

		return vd_opt;	// Return the optimised vertex data
	}
}


#endif