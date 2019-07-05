#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__

#include "DataIO.h"		// Get file in and out functions


// This will contain the functions needed to execute editor operations
namespace Commandline
{
	// A list of key words used for the commandline
	enum KeyWords
	{
		KW_IMPORT,
		KW_ASSIGN,
		KW_ADD
	};

	// A syntax struct for assigning colours to key words
	struct Syntax
	{
		std::string		word;	// The key word
		glm::vec3		colour;		// The key colour
	};

	// This data struct will contain all of the commandline data
	struct Data
	{
		static std::vector <Syntax> kw_data;	// Each key word will have a different colour
		
		// Initialise the map data for each key word
		static inline void Initialise()
		{
			// Initialise key words and syntax
			// ----------------------------------------------- INTERNAL FUCNTIONS ----------------------------------------------- <
			kw_data.push_back({ "import", glm::vec3(0.39f, 1.0f, 0.5f) });
			kw_data.push_back({ "assign", glm::vec3(0.39f, 1.0f, 0.5f) });
			kw_data.push_back({ "add", glm::vec3(0.39f, 1.0f, 0.5f) });

			// ----------------------------------------------- GET & SETTERS ----------------------------------------------- <
			kw_data.push_back({ "get", glm::vec3(0.313f, 0.862f, 1.0f) });

			// ----------------------------------------------- VARIABLES ----------------------------------------------- <
			kw_data.push_back({ "position", glm::vec3(0.5f) });
		}
	};

	// This function will return a key word type (unsigned int 16 bit)
	inline static int16_t GetKeyWordIndex(std::string word)
	{
		for (unsigned int i = 0; i < Data::kw_data.size(); i++)	// First iterate through each key word
		{
			if (word == Data::kw_data[i].word)	// If a key word has been found...
				return i;	// return the current index
		}

		return -1;	// Return minus one as no keyword was found
	}

	// This function will return a key word object
	inline static Syntax GetKeyWord(std::string word)
	{
		Syntax zero = {"", glm::vec3(0.0f) };

		for (unsigned int i = 0; i < Data::kw_data.size(); i++)	// First iterate through each key word
		{
			if (word == Data::kw_data[i].word)	// If a key word has been found...
				return Data::kw_data[i];	// return the current index
		}

		return zero;	// Return key word as normal
	}

	// This is the main callback function for executing commandline operations
	inline static void Callback(ShaderProgram *shader_program, std::vector<std::string> line)
	{
		if (line.size() == 3)	// If there are three words on the line
		{
			int16_t kw = GetKeyWordIndex(line[0]);	// Search for key word for first word
	
			switch (kw)	// Check for word type
			{
			case KW_IMPORT:		// Import case
				if (line[1] == "obj")	// If the import type is obj
				{
					std::string f_ext = line[2] + ".obj";	// Add extension to file name
					if (!DataIO::Import::WavefrontObjI(shader_program->GetProgram(), f_ext.c_str()))	// If the importation failed...
						std::cout << "Error: Failed to import obj file!\n";
					else	// Otherwise..
					{				
						return;		// Return success
					}
				}

				if (line[1] == "dds")
				{
					std::string f_ext = line[2];

					Content::_textures.push_back(new Texture2d({ f_ext + "_n.dds" }, NORMAL, GL_REPEAT, GL_LINEAR));	// Push back default texture normal
					Content::_textures.push_back(new Texture2d({ f_ext + "_a.dds" }, ALBEDO, GL_REPEAT, GL_LINEAR));	// Push back default texture albedo
					Content::_textures.push_back(new Texture2d({ f_ext + "_sr.dds" }, SPECROUGH, GL_REPEAT, GL_LINEAR));	// Push back default texture spec
					Content::_textures.push_back(new Texture2d({ f_ext + "_m.dds" }, METALIC, GL_REPEAT, GL_LINEAR));	// Push back default texture metalic
					Content::_textures.push_back(new Texture2d({ f_ext + "_e.dds" }, EMISSIVE, GL_REPEAT, GL_LINEAR));	// Push back default texture metalic
				}
				break;	// Break from switch statement
			case KW_ADD:
				if (line[1] == "mat")
				{
					std::string m_name = line[2];

					Content::_materials.push_back(new Material(shader_program->GetProgram(), line[2] + static_cast<std::string>(MAT_EXTENSION),
						{ Content::_textures[5], Content::_textures[6], Content::_textures[7], Content::_textures[8], Content::_textures[9] }));
				}
				break;
			case KW_ASSIGN:
				if (line[1] == "mat")
				{
					std::string m_name = line[2];

					std::vector<Material*> test;
					test.push_back(Content::_materials[1]);

					for (unsigned int i = 0; i < Content::_map->GetActors().size(); i++)
					{
						((StaticMesh*)Content::_map->GetActors()[i])->SetMaterials(test);
					}
				}
				break;
			}
		}
	}
}

// Define static variables
std::vector <Commandline::Syntax>	Commandline::Data::kw_data;

#endif