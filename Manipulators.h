#ifndef __MANIPULATORS_H__
#define __MANIPULATORS_H__

#include <iostream>
#include "Content.h"
#include "DataIO.h"

class Manipulators
{
private:
	static bool _active;
	static unsigned int _program;
	static glm::vec3 _pos;
public:
	inline Manipulators(unsigned int program)
	{
		Create(program);

	}
	inline ~Manipulators() {}

	inline static void SetActive(bool val) { _active = val; }
	inline static void SetPosition(glm::vec3 pos) { _pos = pos; }

	inline static void Create(unsigned int program)
	{
		_program = program;
		_active = false;

		if (!DataIO::Import::WavefrontObjI(program, "arrow.obj"))	// If the importation failed...
			std::cout << "Error: Failed to import obj file!\n";
		Content::_arrows.push_back(Content::_meshes[Content::_meshes.size() - 1]);
		if (!DataIO::Import::WavefrontObjI(program, "arrow_up.obj"))	// If the importation failed...
			std::cout << "Error: Failed to import obj file!\n";
		Content::_arrows.push_back(Content::_meshes[Content::_meshes.size() - 1]);
		if (!DataIO::Import::WavefrontObjI(program, "arrow_front.obj"))	// If the importation failed...
			std::cout << "Error: Failed to import obj file!\n";
		Content::_arrows.push_back(Content::_meshes[Content::_meshes.size() - 1]);
	}

	inline static void Render()
	{
		glUseProgram(_program);

		glm::mat4 mod = glm::translate(glm::vec3(_pos.x, _pos.y, _pos.z));

		glm::mat4 MVP = Content::_map->GetCamera()->GetProjectionMatrix()
			* Content::_map->GetCamera()->GetViewMatrix() * mod;

		glUniformMatrix4fv(glGetUniformLocation(_program, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

		glDisable(GL_DEPTH_TEST);

		glUniform3f(glGetUniformLocation(_program, "colour_id"), 1.0f, 0.0f, 0.0f);
		Content::_arrows[0]->Render();

		glUniform3f(glGetUniformLocation(_program, "colour_id"), 0.0f, 1.0f, 0.0f);
		Content::_arrows[1]->Render();

		glUniform3f(glGetUniformLocation(_program, "colour_id"), 0.0f, 0.0f, 1.0f);
		Content::_arrows[2]->Render();
	}
};

unsigned int Manipulators::_program;
glm::vec3 Manipulators::_pos;

#endif
