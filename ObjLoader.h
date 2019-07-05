#ifndef __OBJ_EXTENSION__
#define __OBJ_EXTENSION__ ((char*)"Res/Models/")
#endif

#ifndef __OBJ_LOADER_H__
#define __OBJ_LOADER_H__

#include <iostream>
#include <fstream>
#include <glm\glm.hpp>
#include "Vao.h"


// The wavefront namespace contains global functions for loading .obj format files and utilities for optimising vertex data for buffer objects
namespace Wavefront
{
	int vcount = 0;

	// This will store all the variables required to create an obj element
	typedef struct {
		unsigned int from;	// This is our begin offset
		unsigned int to;	// This is our end offset
		std::vector<unsigned int> v_i;	// A list of position vertex indices
		std::vector<unsigned int> vt_i;		// A list of texcoord vertex indices
		std::vector<unsigned int> vn_i;		// A list of normal vertex indices
	} Group;

	// This will store all of the basic information from an obj wavefront file
	struct ObjData
	{
		std::vector<glm::vec3>		v;	// This will store the vertex positions
		std::vector<glm::vec3>		vt;	// This will store the vertex texcoords	
		std::vector<glm::vec3>		vn;	// This will store the vertex normals
		std::vector<Group>			g;	// This will store each group of vertices
		std::vector<std::string>	u;	// This will store the material names
		std::string					o;	// This will store the name of each object

		// Default constructor
		inline ObjData() : o("") {}
	};

	// This function loads data from a wavefront: obj file and returns the data
	inline bool Import(const char* file, ObjData &out_obj)
	{
		ObjData obj;	// To store our in / out data

		std::ifstream _in(file);	// Check if the file is valid
		if (!_in)	// If the file is invalid...
		{
			std::cout << "Wavefront Import Error: The file is invalid! Check that the file exists.\n";	// Print out error message
			return false;	// Return false as failed
		}

		char data[256];		// Create a buffer for each line in the file
		std::vector <std::string> line;		// This will contain our text from the buffer
		while (_in.getline(data, 256))	// While each line is being read...
			line.push_back(data);	// Assign the data to our string container
		
		bool mat_id_exists = false;		// This will check if the newly discovered material already exists
		int current_group = 0;	// This will record the number of groups
		unsigned int mat_ids = -1;	// This will store each mat id value for multiple material bindings
		std::string obj_name = "";	// This will store the object's name from the file

		for (unsigned int i = 0; i < line.size(); i++)	// Iterate through each line...
		{
			switch ((line[i])[0])	// Check the value of the first character of each line
			{
			case 'v':	// If the character is a 'v'...
				float x, y, z;	// Create temp variables for vertices
				char chars[128];	// Create temp variable for character data
				if ((line[i])[1] == 'n')	// If the second character is 'n'...
				{
					sscanf_s((&line[i])[0].c_str(), "vn %f %f %f", &x, &y, &z);		// Scan this line and store the three floats to our temp variables
					obj.vn.push_back(glm::vec3(x, y, z));	// Record this data
				}
				if ((line[i])[1] == 't')	// If the second character is 't'...
				{
					sscanf_s((&line[i])[0].c_str(), "vt %f %f", &x, &y);	// Scan this line and store the two floats to our temp variables
					obj.vt.push_back(glm::vec3(x, -y, 0.0f));		// Record this data
				}
				else if ((line[i])[1] == ' ')	// If the second character is nothing...
				{
					sscanf_s((&line[i])[0].c_str(), "v %f %f %f", &x, &y, &z);	// Scan this line and store the three floats to our temp variables
					obj.v.push_back(glm::vec3(x, y, z));	// Record this data

					vcount++;
				}
				
				break;
			case 'o':	// if the character is a 'o'...
				sscanf_s((&line[i])[0].c_str(), "o %s", chars, 128);	// scan this line and store the chars to our temp variables
				out_obj.o = chars;	// Assign the object name to the obj data
				break;
			case 'u':
				sscanf_s((&line[i])[0].c_str(), "usemtl %s", chars, 128);	// scan this line and store the chars to our temp variables
				for (unsigned int j = 0; j < obj.u.size(); j++)	// Iterate through our mat names...
				{
					if (chars == obj.u[j])	// If the found id matches any existing ids...
					{
						mat_id_exists = true;	// Mat id already exists
						current_group = j;	// Set the current group id back to this mat id again
						break;
					}
				}
				if (!mat_id_exists)		// If the mat id doesn't already exist...
				{
					mat_ids++;	// Increment the number of materials being used
					obj.u.push_back(chars);		// Add another mat to the mat string container
					obj.g.push_back(Group());	// Create a new group (obj element)	
					current_group = obj.g.size() - 1;	// Set the current group id to the group size
				}
				mat_id_exists = false;	// Set mat id exists to false ready for the next
				break;
			case 'f':	// If the character is a 'f'...
				unsigned int v_i[3], vn_i[3], vt_i[3];	// Create temp variables for encapsulating face data
				
				sscanf_s((&line[i])[0].c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v_i[0], &vt_i[0], &vn_i[0], &v_i[1], &vt_i[1], &vn_i[1], &v_i[2], &vt_i[2], &vn_i[2]);		// Scan and consume the face data for each line

				for (unsigned k = 0; k < 3; k++)	// For each vertex...
				{
					obj.g[current_group].v_i.push_back(v_i[k]);		// Add a new index for vertex type 0
					obj.g[current_group].vt_i.push_back(vt_i[k]);	// Add a new index for vertex type 1
					obj.g[current_group].vn_i.push_back(vn_i[k]);	// Add a new index for vertex type 2
				}
				
				break;
			}
		}

		unsigned int offset = 0;	// This variable will be used to increment the offsets for each groups indices
		for (unsigned int g = 0; g < obj.g.size(); g++)		// Iterate through the number of groups...
		{
			out_obj.g.push_back(Group());	// Create a new group for our out_obj

			for (unsigned int i = 0; i < obj.g[g].v_i.size(); i++)	// Iterate through the number of vertex indices...
			{
				unsigned int v_i = obj.g[g].v_i[i];		// Assign a temp varaible to each vertex position's index
				unsigned int vt_i = obj.g[g].vt_i[i];	// Assign a temp varaible to each vertex texcoord's index
				unsigned int vn_i = obj.g[g].vn_i[i];	// Assign a temp varaible to each vertex normal's index

				glm::vec3 vertex = obj.v[v_i - 1];	// Get the desired position vertex value using our vertex indices
				glm::vec3 uv = obj.vt[vt_i - 1];	// Get the desired texcoord vertex value using our vertex indices
				glm::vec3 normal = obj.vn[vn_i - 1];	// Get the desired normal vertex value using our vertex indices

				out_obj.v.push_back(vertex);	// Add the indexed version of the vertex position value to our out_obj
				out_obj.vt.push_back(uv);	// Add the indexed version of the vertex texcoord value to our out_obj
				out_obj.vn.push_back(normal);	// Add the indexed version of the normal position value to our out_obj

				out_obj.g[g].v_i.push_back(obj.g[g].v_i[i]);	// Assign the indices from the input to output data
			}
			
			unsigned int g_index_count = out_obj.g[g].v_i.size();	// Get the index count for each group by getting the index size
			out_obj.g[g].from = offset;		// Set the starting index point of each group's offset
			out_obj.g[g].to = offset + g_index_count;	// Set the end index point of each group's offset + index size
			offset += g_index_count;	// Increment the offset by the last index size
		}

		return true;	// Return success
	}

	// This function will convert the given 'ObjData' to a 'VertexData' and return the ebo as output
	inline Vao* CreateVao(std::vector<glm::vec3> &in_positions, std::vector<glm::vec3> &in_texcoords, std::vector<glm::vec3> &in_normals, std::vector<glm::vec3> &in_tangents, std::vector<unsigned int> &in_indices)
	{
		Vao* vao;	// Variable

		std::vector<float*> positions_opt, texcoords_opt, normals_opt, tangents_opt;	// Create three optimised float pointer vector lists for opengl

		for (unsigned int i = 0; i < in_positions.size(); i++)	// Iterate through each vertex
		{
			positions_opt.push_back(glm::value_ptr(in_positions[i]));	// Assign position vertices
			texcoords_opt.push_back(glm::value_ptr(in_texcoords[i]));	// Assign texcoord vertices
			normals_opt.push_back(glm::value_ptr(in_normals[i]));	// Assign normal vertices
			tangents_opt.push_back(glm::value_ptr(in_tangents[i]));		// Assign tangent vertices
		}
		
		vao = new Vao({ new Vbo(positions_opt, 3, 0), new Vbo(texcoords_opt, 3, 1), new Vbo(normals_opt, 3, 2), new Vbo(tangents_opt, 3, 3) }, new Ebo(in_indices));	// Initialise vao

		return vao;		// Return result
	}
};

#endif