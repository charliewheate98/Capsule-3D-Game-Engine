#ifndef __DAE_LOADER_H__
#define __DAE_LOADER_H__

#include <fstream>	// Include file stream functionality
#include <string>	// Include string for char manipulation
#include <iterator>		// Get iterator functions
#include <sstream>	// Include sstream for string manipulation
#include "VertexData.h"		// Get access to the vertex data struct
#include "AnimData.h"	// Include anim data structs

#define GEOMETRY_VERTEX			0	// Geometry vertex positions, normals and texcoords
#define GEOMETRY_INDEX			1	// Geometry index data for each vertex
#define JOINT_NAME				2	// Joint names (used for specifying ids)
#define INVERSE_BIND_MATRIX		3	// Joint inverse bind matrices (used to calculate in local coords)
#define WEIGHT_VALUE			4	// Weight values (used to determin the intensity of the skinning information)
#define VERTEX_WEIGHT			5	// Vertex weight data (used to point out the amount of weights / joints affecting each vertex)
#define ANIMATION				6	// Animation data (used to determin each matrix position at a given time frame for each joint)
#define HIERARCHY				7	// Hierarchy data (used to determin the joint structure between children and parent information)

#define MAX_WEIGHTS				3	// Each vertex can only be affected by a maximum of up to three weights
#define MAX_JOINTS				32	// Each vao can hold a maximum of up to 32 joints


// This namespace will store all the functions needed to load the key data from a .dae file
namespace DaeLoader
{
	// A struct for containing the weight vertex data
	struct WeightVertexList
	{
		std::vector<glm::ivec3> _joint_indices;	// A list of joint indices
		std::vector<glm::ivec3> _weight_indices;	// A list of weight indices

		// Default constructor
		inline WeightVertexList() {}
	};


	// This will load all of the vertex data for the geometry
	static inline void Import(const char* uri, VertexData& out_vertex_data, AnimData& out_anim_data)
	{
		FILE*			file;	// The file object
		std::string		line;	// String for current line

		if (fopen_s(&file, uri, "r") != 0)		// If file failed to open...
		{
			std::cout << "Error: Failed to open file!\n";	// Print out error message
			return;		// Return as normal
		}

		bool						begin_scan = false;	// A bool for checking when to start extracting data
		int							current_scan = -1;		// A count for each line extraction
		int							current_anim = -1;	// Current joint anim
		unsigned int				state = 0;	// A state system for checking key data
		unsigned int				count = 0;		// Num variables
		unsigned int				stride = 0;		// Stride for each attribute
		unsigned int				count_div_stride = 0;	// The total lines for extraction
		unsigned int				vertex_state = 0;	// State of vertex extraction
		unsigned int				i_0 = 0;	// Start point before buffer
		unsigned int				i_1 = 0; 	// End point before buffer
		unsigned int				len = 0;	// Length of buffer
		VertexData					vd = VertexData();		// A temp variable for our vertex data
		AnimData					ad = AnimData();		// A temp variable for our anim data
		std::ifstream				file_stream(uri);	// Create an ifstream for reading file

		std::vector<std::string>	temp_values;	// For conversion purposes

		std::vector<glm::mat4x4>	inverse_bind_matrices;		// The joint inverse bind matrices in order
		std::vector<glm::mat4x4>	joint_matrices;		// The joint inverse bind matrices in order
		std::vector<std::string>	joint_names;	// Temp container to get each joint name
		std::vector<float>			weights;	// The weight values
		std::vector<unsigned int>	vcount;	// The weight index data for each vertex
		
		Joint						root_joint;		// The joint data for skeletal heirarchy
		WeightVertexList			weight_vertices;	// The weight vertex list data for each position
		std::vector<JointAnim>		joint_anims;	// The animation data for each joint

		while (std::getline(file_stream, line))		// Iterate through each line
		{
			if (begin_scan)	// If this line is ready for extraction...
			{
				current_scan++;	// Increment the current line index

				if (state == GEOMETRY_VERTEX)	// If we're in the state of extracting vertex data...
				{
					glm::vec3 v = glm::vec3(0.0f);	// Temp vertex container

					if (static_cast<unsigned int>(current_scan) < (count / stride))	// If the current scan index is less than the N of vertices divided by stride...
					{
						if (vertex_state == 0)	// If vertex type POSITIONS
						{
							sscanf_s(line.c_str(), "%f %f %f", &v.x, &v.y, &v.z);	// Get line buffer and store data to temp variables
							vd.positions.push_back(glm::vec3(v.x, v.y, v.z));	// Add a position vertex using temp variable values
						}
						else if (vertex_state == 1)	// If vertex type NORMALS
						{
							sscanf_s(line.c_str(), "%f %f %f", &v.x, &v.y, &v.z);	// Get line buffer and store data to temp variables
							vd.normals.push_back(glm::vec3(v.x, v.y, v.z));	// Add a position vertex using temp variable values
						}
						else if (vertex_state == 2)	// If vertex type TEXCOORDS
						{
							sscanf_s(line.c_str(), "%f %f", &v.x, &v.y);	// Get line buffer and store data to temp variables
							vd.texcoords.push_back(glm::vec3(v.x, v.y, 0.0f));	// Add a position vertex using temp variable values
						}
					}
					else	// If the current scan exceeds the N of vertics...
					{
						current_scan = -1;	// Reset the current scan count
						begin_scan = false;	// Stop extraction

						if (vertex_state >= 2)	// If the vertex state has finished...
							state++;	// Go to next state
						else	// If vertex state has NOT finished...
							vertex_state++;		// Skip to next vertex state
					}
				}
				else if (state == JOINT_NAME)	// Extract joint name data
				{
					std::stringstream ss(line);	// Create string stream for maniplulation
					std::copy(std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>(), std::back_inserter(joint_names));	// Iterate through each string usign stream and store in vector list

					unsigned int i;
					for (i = 0; i < 13; i++)	// Get the last 13 characters we don't need
						joint_names[joint_names.size() - 1].pop_back();		// Delete the unwanted chars

					current_scan = -1;	// Reset the current scan count
					begin_scan = false;	// Start extraction
					state++;	// Go to next state
				}
				else if (state == INVERSE_BIND_MATRIX)	// Extract joint inverse bind matrix data
				{
					if (static_cast<unsigned int>(current_scan) < (count / stride) + 1)	// If the current scan index is less than the N of vertices divided by stride...
					{
						if (current_scan > 0)	// Skip the empty line before scanning
						{
							glm::mat4x4	m = glm::mat4x4(0.0f);		// A temp inverse matrix
							sscanf_s(line.c_str(), "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",	&m[0][0], &m[1][0], &m[2][0], &m[3][0],
																										&m[0][1], &m[1][1], &m[2][1], &m[3][1],
																										&m[0][2], &m[1][2], &m[2][2], &m[3][2],
																										&m[0][3], &m[1][3], &m[2][3], &m[3][3]);	// Get line buffer and store data to temp variables

							inverse_bind_matrices.push_back(m);	// Add an inverse bind matrix using temp variable values
						}
					}
					else
					{
						current_scan = -1;	// Reset the current scan count
						begin_scan = false;	// Start extraction
						state++;	// Go to next state
					}
				}
				else if (state == WEIGHT_VALUE)	// Extract weight data
				{
					if (current_scan > 0)	// Skip the empty line
					{
						if (static_cast<unsigned int>(current_scan) < count + 1)	// If the current scan is greater than count plus 1 offset...
						{
							std::stringstream ss(line);	// Create string stream for maniplulation

							for (unsigned int i = 0; i < stride; i++)	// Iterate through 16 times for each line
							{
								float temp_value = 0.0f;	// Create a temp weight var
								ss >> temp_value;	// Get next index in stream
								weights.push_back(temp_value);	// Add a weight to the weight container
							}
						}
						else
						{
							unsigned int num_chars = 15;	// The amount of unwanted chars
							for (unsigned int i = 0; i < num_chars; i++)	// Iterate 15 times
								weights.pop_back();		// Remove unwanted "</float_array>" 

							current_scan = -1;	// Reset the current scan count
							begin_scan = false;	// Start extraction
							state++;	// Go to next state
						}
					}
				}
				else if (state == ANIMATION)	// Extract weight data
				{
					if (current_scan > 0)	// Skip the empty line
					{
						if (joint_anims[current_anim]._time_stamps.size() != count)		// IF we haven't finished getting input array...
						{
							if (static_cast<unsigned int>(current_scan) < count_div_stride + 1)	// If the current scan is greater than count plus 1 offset...
							{
								std::stringstream ss(line);	// Create string stream for maniplulation

								for (unsigned int i = 0; i < stride; i++)	// Iterate through 16 times for each line
								{
									float temp_value = 0.0f;	// Create a temp weight var
									ss >> temp_value;	// Get next index in stream
									joint_anims[current_anim]._time_stamps.push_back(temp_value);	// Add a weight to the weight container
								}
							}
							else
							{
								unsigned int i = 0;		// A temp iterator index
								unsigned int zeros = 0;		// A temp variable for checking the amount of zeros
								for (i = 0; i < joint_anims[current_anim]._time_stamps.size(); i++)	// Iterate 15 times
								{
									if (joint_anims[current_anim]._time_stamps[i] == 0.0f)	// If we found an unwanted zero value...
										zeros++;	// Count it
								}

								for (i = 0; i < zeros; i++)		// For the amount of zeros found...
									joint_anims[current_anim]._time_stamps.pop_back();	// Remove them

								current_scan = 0;	// Reset the current scan count
							}
						}
						else	// Otherwise get the output array
						{
							
							if (static_cast<unsigned int>(current_scan) < count + 1)	// If the current scan is greater than count plus 1 offset...
							{
								std::string temp_float;		// Create temp float value
								std::stringstream ss(line);	// Create string stream for maniplulation		
								for (unsigned int i = 0; i < stride; i++)	// Iterate through each stride
								{
									ss >> temp_float;	// Get current float to temp value
									temp_values.push_back(temp_float);	// Store value to temp_values container
								}									
							}
							else	// When finished scanning...
							{
								unsigned int i;		// Index iterator
								unsigned int num_chars = 41;	// The amount of unwanted chars
								for (i = 0; i < num_chars; i++)	// Iterate through the number of unwanted chars
									temp_values[temp_values.size() - 1].pop_back();		// Remove the unwanted chars in last temp_value

								for (i = 0; i != temp_values.size(); i += stride)	// Iterate through each stride of temp float values
								{
									// Convert the temp values and store the matrix data for the current join anim
									joint_anims[current_anim]._matrices.push_back(glm::mat4x4(	std::stof(temp_values[i]), std::stof(temp_values[i+1]), std::stof(temp_values[i+2]), std::stof(temp_values[i+3]), 
																								std::stof(temp_values[i+4]), std::stof(temp_values[i+5]), std::stof(temp_values[i+6]), std::stof(temp_values[i+7]),
																								std::stof(temp_values[i+8]), std::stof(temp_values[i+9]), std::stof(temp_values[i+10]), std::stof(temp_values[i+11]),
																								std::stof(temp_values[i+12]), std::stof(temp_values[+13]), std::stof(temp_values[i+14]), std::stof(temp_values[i+15])));
								}
								
								temp_values.resize(0);	// Reset the temp char container
								current_scan = -1;	// Reset the current scan count
								begin_scan = false;	// Start extraction
							}
						}
					}
				}
			}


			switch (state)	// Check for each search state
			{
			case GEOMETRY_VERTEX:	// Get VERTEX data
				if ((vertex_state == 0) && (std::string::npos != line.find("-POSITION-array")))	// Find the start of POSITIONS
				{
					stride = 3;		// Set stride for POSITION attribute
					len = line.find(">") - line.find("t=") - 4;	// Calculate the buffer size of count value
					count = std::stoi(line.substr(line.find("t=") + 3, len));	// Convert count to int

					if (std::string::npos != line.find(">"))	// Find the end of the line
						begin_scan = true;	// Get ready to extract the next line of data
				}

				if ((vertex_state == 1) && (std::string::npos != line.find("-Normal0-array")))	// Find the start of NORMALS
				{
					stride = 3;		// Set stride for NORMAL attribute
					len = line.find(">") - line.find("t=") - 4;	// Calculate the buffer size of count value
					count = std::stoi(line.substr(line.find("t=") + 3, len));	// Convert count to int

					if (std::string::npos != line.find(">"))	// Find the end of the line
						begin_scan = true;	// Get ready to extract the next line of data
				}

				if ((vertex_state == 2) && (std::string::npos != line.find("-UV0-array")))	// Find the start of TEXCOORDS
				{
					stride = 2;		// Set stride for TEXCOORD attribute
					len = line.find(">") - line.find("t=") - 4;	// Calculate the buffer size of count value
					count = std::stoi(line.substr(line.find("t=") + 3, len));	// Convert count to int

					if (std::string::npos != line.find(">"))	// Find the end of the line
						begin_scan = true;	// Get ready to extract the next line of data
				}

				break;
			case GEOMETRY_INDEX:	// Get INDEX data
				if (std::string::npos != line.find("<triangles"))
				{
					i_0 = line.find("t=") + 3;	// Get the first index offset for count
					i_1 = line.find("m") - 2;	// Get the last index offset for count
					len = i_1 - i_0;	// Calculate the buffer size

					count = std::stoi(line.substr(i_0, len)) * 9;		// Convert the count value to int

					i_0 = line.find("<p>") + 4;		// Get the first index offset for count
					i_1 = line.find("</p>");	// Get the last index offset for count
					len = i_1 - i_0;	// Calculate the buffer size

					std::string indices_str = line.substr(i_0, len);	// Get index data to string
					std::stringstream indices_strs(indices_str);	// Create string stream for maniplulation

					unsigned int i;		// A temp index variable
					for (i = 0; i < count; i++)	// Iterate through each index
					{
						unsigned int index = 0;		// Create a temp value
						indices_strs >> index;	// Get next index in stream
						vd.indices.push_back(index);	// Add a new index
					}

					for (i = 0; i != vd.indices.size(); i += 3)		// Iterate through each index with an offset of three
					{
						unsigned int p_i = vd.indices[i];		// Position index
						unsigned int n_i = vd.indices[i + 1];	// Normal index
						unsigned int t_i = vd.indices[i + 2];	// Texcoord index

						glm::vec3 position = vd.positions[p_i];		// Get the index of position
						glm::vec3 normal = vd.normals[n_i];		// Get the index of normal
						glm::vec3 texcoord = vd.texcoords[t_i];		// Get the index of texcoord

						out_vertex_data.positions.push_back(position);	// Assign position data
						out_vertex_data.normals.push_back(normal);	// Assign normal data
						out_vertex_data.texcoords.push_back(texcoord);	// Assign texcoord data
					}

					out_vertex_data.indices = vd.indices;	// Assign indices


					// NEEDS FIXING!!! // ------------------------------
					std::cout << "Vertex indices: " << vd.indices.size() << "\n";
					std::cout << "Vertex positions: " << vd.positions.size() << "\n";
					std::cout << "Vertex normals: " << vd.normals.size() << "\n";
					std::cout << "Vertex texcoords: " << vd.texcoords.size() << "\n";
					// NEEDS FIXING!!! // ------------------------------



					state++;	// Go to next state!
				}
				break;
			//case JOINT_NAME:	// Get JOINT NAME data
			//	if (std::string::npos != line.find("<Name_array"))	// If joint_id names have been discovered...
			//		begin_scan = true;	// Start extraction
			//	break;
			//case INVERSE_BIND_MATRIX:	// Get JOINT INVERSE BIND MATRIX data
			//	if (std::string::npos != line.find("<float_array"))	// If joint_inverse_bind matrices have been discovered...
			//	{
			//		i_0 = line.find("t=") + 3;	// Get the first index offset for count
			//		i_1 = line.find(">") - 1;	// Get the last index offset for count
			//		len = i_1 - i_0;	// Calculate the buffer size

			//		count = std::stoi(line.substr(i_0, len));		// Convert the count value to int
			//		stride = 16;	// Set stride to 4x4 matrix

			//		begin_scan = true;	// Start extraction
			//	}
			//	break;
			//case WEIGHT_VALUE:	// Get the WEIGHT data
			//	if (std::string::npos != line.find("<float_array"))	// If weight data have been discovered...
			//	{
			//		i_0 = line.find("t=") + 3;	// Get the first index offset for count
			//		i_1 = line.find(">") - 1;	// Get the last index offset for count
			//		len = i_1 - i_0;	// Calculate the buffer size

			//		stride = 16;	// Set stride to 4x4 matrix
			//		count = (std::stoi(line.substr(i_0, len)) + stride - 1) / stride;		// Convert the count value to int
			//		
			//		begin_scan = true;	// Start extraction
			//	}
			//	break;
			//case VERTEX_WEIGHT:		// Get the VERTEX WEIGHT data
			//	if (std::string::npos != line.find("<vcount"))	// If weight vertices have been discovered...
			//	{
			//		std::string l = line.substr(line.find(">") + 1, line.length());		// Get the start of array
			//		std::stringstream ss(l);	// Create string stream for maniplulation
			//		std::copy(std::istream_iterator<unsigned int>(ss), std::istream_iterator<unsigned int>(), std::back_inserter(vcount));	// Iterate through each string usign stream and store in vector list
			//		
			//		current_scan = -1;	// Reset the current scan count
			//	}
			//	else if (std::string::npos != line.find("<v>"))	// If weight vertices have been discovered...
			//	{
			//		std::string l = line.substr(line.find(">") + 1, line.length());		// Get the start of array
			//		std::stringstream ss(l);	// Create string stream for maniplulation
			//		std::vector<unsigned int> wj_indices;	// Create a temp container for the weight and joint index data
			//		std::copy(std::istream_iterator<unsigned int>(ss), std::istream_iterator<unsigned int>(), std::back_inserter(wj_indices));	// Iterate through each string usign stream and store in vector list

			//		unsigned int i, j;		// Iterator index
			//		std::vector<glm::ivec2> pairs;	// Temp pair container

			//		for (i = 0; i != wj_indices.size(); i += 2)		// Iterate through each weight vertex
			//			pairs.push_back(glm::ivec2(wj_indices[i], wj_indices[i + 1]));	// Add a pair for every two offsets
			//		
			//		unsigned int offset_count = 0;	// Create a temp offset counter for each vertex size
			//		for (i = 0; i < vcount.size(); i++)		// Iterate through each index
			//		{
			//			glm::ivec3 joint_indices;	// Create a temp joint index container
			//			glm::ivec3 weight_indices;	// Create a temp weight index container

			//			for (j = 0; j != vcount[i]; j++)	// Iterate through each local offset
			//			{
			//				joint_indices[j] = pairs[offset_count + j].x;	// Assign temp joint index variables
			//				weight_indices[j] = pairs[offset_count + j].y;	// Assign temp weight index variables
			//			}
			//			
			//			weight_vertices._joint_indices.push_back(joint_indices);	// Add new joint index array
			//			weight_vertices._weight_indices.push_back(weight_indices);	// Add new weight index array

			//			offset_count += vcount[i];	// Increment to next vertex offset
			//		}

			//		current_scan = -1;	// Reset the current scan count
			//		state++;	// Go to next state!
			//	}
			//	break;
			//case ANIMATION:		// Get the ANIMATION data
			//	if (std::string::npos != line.find("<animation id"))	// If we've come across an animation...
			//	{
			//		current_anim++;		// Incremenet the curent_anim ready to store new joint anim data

			//		i_0 = line.find("e=") + 3;	// Get the first index offset for count
			//		i_1 = line.find("><") - 1;		// Get the last index offset for count
			//		len = i_1 - i_0;	// Calculate the buffer size

			//		joint_anims.push_back(line.substr(i_0, len));	// Add a new joint animation

			//		i_0 = line.find("t=") + 3;	// Get the first index offset for count
			//		i_1 = line.find(">") - 1;	// Get the last index offset for count
			//		len = i_1 - i_0;	// Calculate the buffer size

			//		stride = 16;	// Set stride to 4x4 matrix
			//		count = std::stoi(line.substr(i_0, len));		// Convert the count value to int
			//		count_div_stride = (count + stride - 1) / stride;	// Calculate the number of total lines to count

			//		current_scan = -1;	// Reset the current scan count
			//		begin_scan = true;	// Start extracting data
			//	}
			//	else if (std::string::npos != line.find("</library_animations>"))	// If we've finished reading the anim library...
			//		state++;	// Go to next state!
			//	break;
			//case HIERARCHY:
			//	state++;
			//	break;
			default:
				break;
			}
		}
	}
}

#endif