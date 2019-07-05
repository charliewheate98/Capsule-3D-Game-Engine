#ifndef __VERTEX_DATA_H__
#define __VERTEX_DATA_H__

#include <vector>	// Get dynamic array
#include <map>	// Get map variable
#include <glm\glm.hpp>	// Get glm variables

// This struct contains vertex data
struct VertexData
{
	std::vector<glm::vec3> positions;	// Our vertex positions
	std::vector<glm::vec3> texcoords;	// Our vertex texcoords
	std::vector<glm::vec3> normals;		// Our vertex normals
	std::vector<glm::vec3> tangents;	// Our vertex tangents
	std::vector<unsigned int> indices;	// Our index data
};

// This will be our packed version of a vertex
struct PackedVertex
{
	glm::vec3 position;		// This will store our packed position data
	glm::vec3 uv;	// This will store our packed uv data
	glm::vec3 normal;	// This will pack our normal data
	bool operator<(const PackedVertex that) const { return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0; };	// Create an operator for comparing different vertex data
};

// This function will compare each vertex with an existing vertex
static inline bool GetSimilarVertexIndex(PackedVertex &packed, std::map<PackedVertex, unsigned int> &VertexToOutIndex, unsigned int &result)
{
	std::map<PackedVertex, unsigned int>::iterator it = VertexToOutIndex.find(packed);	// Iterate through each vertex to check for duplicates
	if (it == VertexToOutIndex.end())	// If the iterator has finished with no duplicates found...
		return false;	// Return false - no duplicate found
	else	// Otherwise...
	{
		result = it->second;	// Set the result to the indexed vertex
		return true;	// Return true
	}
}

// This function will take obj data and output optimised vertex data for an element buffer object
static inline void IndexVertexData(std::vector<glm::vec3> & in_vertices, std::vector<glm::vec3> & in_uvs, std::vector<glm::vec3> & in_normals, std::vector<unsigned int> & out_indices, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec3> & out_uvs, std::vector<glm::vec3> & out_normals, std::vector<glm::vec3> & out_tangents)
{
	std::map<PackedVertex, unsigned int> VertexToOutIndex;	// This will record any duplicated vertex data for our indices

	for (unsigned int i = 0; i < in_vertices.size(); i++)	// Iterate through the number of vertex positions...
	{
		PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };		// Set a temp vertex struct to our input data

		unsigned int index;		// Use this index variable as the duplicated result
		bool found = GetSimilarVertexIndex(packed, VertexToOutIndex, index);	// Try to find a similar vertex in out_XXXX

		if (found)	// If a similar vertex has been found...
			out_indices.push_back(index);	// A similar vertex already exists - use the existing one instead!
		else	// Otherwise...
		{
			out_vertices.push_back(in_vertices[i]);		// Add the new vertex to our positions list
			out_uvs.push_back(in_uvs[i]);	// Add the new vertex to our uv list
			out_normals.push_back(in_normals[i]);	// Add the new vertex to our normal list
			unsigned int newindex = (unsigned int)out_vertices.size() - 1;	// Set a new index for our out_index list
			out_indices.push_back(newindex);	// Assign the new index
			VertexToOutIndex[packed] = newindex;	// Assign the new index to our packed vertex list
		}
	}

	out_tangents.assign(out_indices.size(), glm::vec3(0.0f));	// Assign empty tangents ready for further calculation
}

// This function will calculate tangent vectors using linear algebra
static inline void CalculateTangents(VertexData &vd)
{
	for (unsigned int i = 0; i < vd.indices.size(); i += 3)	// For each triangle...
	{
		unsigned int i_0 = vd.indices[i];	// Get index offset of 0
		unsigned int i_1 = vd.indices[i + 1];	// Get index offset of 1
		unsigned int i_2 = vd.indices[i + 2];	// Get index offset of 2

		glm::vec3 edge_0 = vd.positions[i_1] - vd.positions[i_0];	// Calculate first edge
		glm::vec3 edge_1 = vd.positions[i_2] - vd.positions[i_0];	// Calculate second edge

		float delta_u_0 = vd.texcoords[i_1].x - vd.texcoords[i_0].x;	// Calculate u 0 with texcoord x
		float delta_v_0 = vd.texcoords[i_1].y - vd.texcoords[i_0].y;	// Calculate v 0 with texcoord y
		float delta_u_1 = vd.texcoords[i_2].x - vd.texcoords[i_0].x;	// Calculate u 1 with texcoord x
		float delta_v_1 = vd.texcoords[i_2].y - vd.texcoords[i_0].y;	// Calculate v 1 with texcoord y

		float f = 1.0f / (delta_u_0 * delta_v_1 - delta_u_1 * delta_v_0);	// Calculate fraction

		glm::vec3 t;	// Temp tangent variable

		t.x = f * (delta_v_1 * edge_0.x - delta_v_0 * edge_1.x);	// Calculate tangent x
		t.y = f * (delta_v_1 * edge_0.y - delta_v_0 * edge_1.y);	// Calculate tangent y
		t.z = f * (delta_v_1 * edge_0.z - delta_v_0 * edge_1.z);	// Calculate tangent z

		vd.tangents[i_0] += glm::normalize(t);	// Assign tangent for point 0 + i
		vd.tangents[i_1] += glm::normalize(t);	// Assign tangent for point 1 + i
		vd.tangents[i_2] += glm::normalize(t);	// Assign tangent for point 2 + i

		vd.tangents[i_0] = glm::normalize(vd.tangents[i_0] - glm::dot(vd.tangents[i_0], vd.normals[i_0]) * vd.normals[i_0]); 	// Calculate tangent with normal 0
		vd.tangents[i_1] = glm::normalize(vd.tangents[i_1] - glm::dot(vd.tangents[i_1], vd.normals[i_1]) * vd.normals[i_1]); 	// Calculate tangent with normal 1
		vd.tangents[i_2] = glm::normalize(vd.tangents[i_2] - glm::dot(vd.tangents[i_2], vd.normals[i_2]) * vd.normals[i_2]); 	// Calculate tangent with normal 2
	}
}

#endif