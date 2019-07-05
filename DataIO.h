#ifndef __DATA_IO_H__
#define __DATA_IO_H__

#include "Content.h"	// Get access to the content
#include "ObjLoader.h"	// Get access to our obj wavefront loader functions
#include "DaeLoader.h"	// Get access to our dao loader functions
#include "Asset.h"

// This namespace will manage data and information via input / output
namespace DataIO
{
	unsigned int _num_LOD;

	// This class will handle file importations
	class Import
	{
	public:

		// This function will attempt to import a wavefront: obj file
		static inline Mesh* WavefrontObjI(uniform shader_program, const char* file)
		{
			VertexData vd_opt;	// This will contain our optimised vertex data
			Wavefront::ObjData obj;		// This will contain our native obj data from file
			std::vector<Chunk> chunks_opt;	// This will contain our optimised chunks (in order)
			std::vector<Material*> mats_opt;	// This will contain our temp material data

			std::string s_file = file;	// Convert file name to string for conversion

			if (!Wavefront::Import((static_cast<std::string>(__OBJ_EXTENSION__) + s_file).c_str(), obj))	// Attempt to import our obj file...
			{
				std::cout << "Wavefront Import Error: The obj file failed to import!\n";	// Print error code
			}

			IndexVertexData(obj.v, obj.vt, obj.vn, vd_opt.indices, vd_opt.positions, vd_opt.texcoords, vd_opt.normals, vd_opt.tangents);	// Index our obj data for ebo optimisation
			CalculateTangents(vd_opt);	// Calculate tangents for each triangle

			Vao* vao_opt = Wavefront::CreateVao(vd_opt.positions, vd_opt.texcoords, vd_opt.normals, vd_opt.tangents, vd_opt.indices);	// Initialise a new ebo using our optimised vertex data
			
			chunks_opt.push_back(Chunk(obj.g[0].from, obj.g[0].to, 0));		// Add a chunk for our first main element
			mats_opt.push_back(Content::_materials[0]);		// Assign the default material for our first main element

			if (obj.g.size() > 1)	// If multiple groups were detected...
			{
				for (unsigned int i = obj.g.size() - 1; i != 0; i--)	// Iterate through the rest of the groups backwards
				{
					chunks_opt.push_back(Chunk(sizeof(GLuint) * obj.g[i].from, obj.g[i].to, i));		// Add another chunk for each group detected
					mats_opt.push_back(Content::_materials[0]);		// Add another default material to this chunk
				}
			}

			Mesh* mesh = new StaticMesh(shader_program, obj.o, mats_opt, Content::_cubemaps[0]);

			mesh->SetVao(vao_opt);	// Assign the optimised ebo to our mesh ebo
			mesh->SetVertexData(vd_opt);	// Assign the optimised vertex data
			mesh->SetChunks(chunks_opt);	// Assign the optimised chunk list to our mesh chunk list
			mesh->SetNumIndices(vd_opt.indices.size());	// Assign the number of indices to our mesh

			Content::_meshes.push_back(mesh);

			return mesh;	// Return true as success
		}

		static inline bool ImportAsset(uniform shader_program, const char* file, 
			std::vector<Material*> _material, unsigned int nr_LODs)
		{
			std::string s_file = file;

			std::vector<Mesh*> _temp;
			Asset* asset = new Asset();

			for (unsigned int i = 0; i < nr_LODs; ++i)
			{
				std::string f = s_file + "_l_" + std::to_string(i) + ".obj";

				_temp.push_back(DataIO::Import::WavefrontObjI(shader_program, f.c_str()));
			}

			asset->_meshes = _temp;

			Content::_assets.push_back(asset);
			asset->assignMaterial(_material);

			return true;
		}
	};

	class SaveAs
	{
	public:
		// Save as function will save the current map to a new file
		static inline bool MapO(Map *map, const char* file)
		{
			return true;	// Return true as success
		}

		// Save as function will save the current mesh to a new file
		static inline bool MeshO(Mesh *mesh, const char* file)
		{
			if (mesh == NULL)	// Check if the mesh is initialised, otherwise...
			{
				std::cout << "Mesh Error: Mesh failed to save - mesh is equal to null!\n";	// Print error message
				return false;	// Return false as failed
			}

			std::string d = " ";	// Create our delim
			std::fstream f;		// Create our file stream variable

			f.open(static_cast<std::string>(__STATIC_MESH_URI__) + file, std::ios::out);	// Create a new file
			if (f.is_open())	// If the file opened successfully...
			{
				f << "m" << d << mesh->GetMeshType() << d << mesh->GetName() << "\n\n";	// Save the mesh type

				for (unsigned int i = 0; i < mesh->GetChunks().size(); i++)		// Iterate through each chunk...
					f << "c" << d << mesh->GetChunks()[i]._id << d << mesh->GetChunks()[i]._index_offset << d << mesh->GetChunks()[i]._index_count << d << mesh->GetMaterials()[i]->GetName() << "\n";		// Save the chunk data with each materials assigned
				f << "\n";	// Create a new line for next value types
				for (unsigned int i = 0; i < mesh->GetVertexData().positions.size(); i++)		// Iterate through each vertex for our positions...
					f << "p" << d << mesh->GetVertexData().positions[i].x << d << mesh->GetVertexData().positions[i].y << d << mesh->GetVertexData().positions[i].z << "\n";		// Save the positions
				f << "\n";	// Create a new line for next value types
				for (unsigned int i = 0; i < mesh->GetVertexData().texcoords.size(); i++)		// Iterate through each vertex for our texcoords...
					f << "t" << d << mesh->GetVertexData().texcoords[i].x << d << mesh->GetVertexData().texcoords[i].y << "\n";		// Save the texcoords
				f << "\n";	// Create a new line for next value types
				for (unsigned int i = 0; i < mesh->GetVertexData().normals.size(); i++)		// Iterate through each vertex for our normals...
					f << "n" << d << mesh->GetVertexData().normals[i].x << d << mesh->GetVertexData().normals[i].y  << d << mesh->GetVertexData().normals[i].z << "\n";		// Save the normals
				f << "\n";	// Create a new line for next value types
				for (unsigned int i = 0; i < mesh->GetVertexData().indices.size(); i++)		// Iterate through each index for our vertex indices...
					f << "i" << d << mesh->GetVertexData().indices[i] << "\n";	// Save the indices

				if (mesh->GetMeshType() == M_SKELETAL)	// If the mesh type is skeletal...
				{
					// Add extensive values here...
				}
			}

			return true;	// Return true as success
		}
	};

	class  Save
	{
	public:
	};

	class New
	{
	public:
	};

	class Open
	{
	public:

		// Open a mesh file
		static inline bool MeshI(unsigned int shader_program, const char* file)
		{
			std::ifstream _in(static_cast<std::string>(__STATIC_MESH_URI__) + file);	// Check if the file is valid
			if (!_in)	// If the file is invalid...
			{
				std::cout << "Mesh Error: The file is invalid! Check that the file exists.\n";	// Print out error message
				return false;	// Return false as failed
			}

			char data[256];		// Create a buffer for each line in the file
			std::vector <std::string> line;		// This will contain our text from the buffer
			while (_in.getline(data, 256))	// While each line is being read...
				line.push_back(data);	// Assign the data to our string container

			unsigned int t = 0;		// This will record the mesh type
			int cd[3];	// Store chunk data
			float x, y, z;	// Create temp variables for vertex data
			char chars[128];	// Create temp variable for character data
			std::string n;	// This will store the meshes name
			Vao* vao;	// Our vertex buffer object for storing vertex arrays
			VertexData vd;	// Our vertex data for parsing to our ebo
			std::vector<Chunk> c;	// Our chunk data for assigning to our mesh
			std::vector<Material*> m;	// Our material data for assigning to our mesh chunks

			for (unsigned int i = 0; i < line.size(); i++)	// Iterate through each line...
			{
				switch ((line[i])[0])	// Check the value of the first character of each line
				{
				case 'm':	// If the character is a 'm'...
					sscanf_s((&line[i])[0].c_str(), "m %d %s", &t, &chars, 128);		// Scan this line
					n = chars;	// Record name data
					break;

				case 'c':	// If the character is a 'c'...
					sscanf_s((&line[i])[0].c_str(), "c %d %d %d %s", &cd[0], &cd[1], &cd[2], &chars, 128);		// Scan this line

					c.push_back(Chunk(cd[1], cd[2], cd[0]));	// Record chunk data
					m.push_back(Content::GetMaterial(chars));	// Record material data
					break;

				case 'p':	// If the character is a 'p'...
					sscanf_s((&line[i])[0].c_str(), "p %f %f %f", &x, &y, &z);		// Scan this line
					vd.positions.push_back(glm::vec3(x, y, z));	// Record attrib data
					break;

				case 't':	// If the character is a 't'...
					sscanf_s((&line[i])[0].c_str(), "t %f %f", &x, &y);		// Scan this line
					vd.texcoords.push_back(glm::vec3(x, y, 0.0f));	// Record attrib data
					break;

				case 'n':	// If the character is a 'n'...
					sscanf_s((&line[i])[0].c_str(), "n %f %f %f", &x, &y, &z);		// Scan this line
					vd.normals.push_back(glm::vec3(x, y, z));	// Record attrib data
					break;

				case 'i':	// If the character is a 'i'...
					sscanf_s((&line[i])[0].c_str(), "i %d", &cd[0]);		// Scan this line
					vd.indices.push_back(cd[0]);	// Record attrib data
					break;
				}
			}

			vao = Wavefront::CreateVao(vd.positions, vd.texcoords, vd.normals, vd.tangents, vd.indices);		// Create our vao
			
			Mesh* mesh = new StaticMesh(shader_program, n, m, Content::_cubemaps[0]);		// Create our temp variable for allocating a mesh
			
			mesh->SetVao(vao);	// Assign the optimised ebo to our mesh ebo
			mesh->SetVertexData(vd);	// Assign the optimised vertex data
			mesh->SetChunks(c);	// Assign the optimised chunk list to our mesh chunk list
			mesh->SetNumIndices(vd.indices.size());	// Assign the number of indices to our mesh



			// TEMP !!! Set up per-vertex collision ----------------------------------
			mesh->SetCollisionType(COLLISION_TYPE_PER_VERTEX);
			// TEMP !!! --------------------------------------------------------------

			Content::_meshes.push_back(mesh);	// Add the mesh to our content
			
			return true;	// Return true as success
		}


	};
};

#endif