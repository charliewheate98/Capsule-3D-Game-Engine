#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <sstream>
#include <vector>
#include <glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

// Dynamic - parsed to uniform data
class Instance
{
private:
	unsigned int				_num_instances;
	unsigned int				_geometry_vao;
	unsigned int				_geometry_vbo;

	unsigned int quadVAO;
	unsigned int quadVBO;
public:
	inline Instance(unsigned int num_instances)
	{
		_num_instances = num_instances;

		GLfloat vertex_positions[] = { -0.01f,  0.015f, 0.0f,
										0.01f, -0.015f, 0.0f,
									   -0.01f, -0.015f, 0.0f,

									   -0.01f,  0.015f, 0.0f,
										0.01f, -0.015f, 0.0f,
										0.01f,  0.015f, 0.0f };

		// Generate geometry vao
		glGenVertexArrays(1, &_geometry_vao);
		glBindVertexArray(_geometry_vao);

		// Store vbo data to an array buffer
		glGenBuffers(1, &_geometry_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _geometry_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), &vertex_positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	void renderQuad()
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _num_instances);
		glBindVertexArray(0);
	}

	inline ~Instance()
	{
		glDeleteBuffers(1, &_geometry_vbo);
		glDeleteVertexArrays(1, &_geometry_vao);
	}

	inline void render()
	{
		//glBindVertexArray(_geometry_vao);
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _num_instances);
		//glBindVertexArray(0);

		renderQuad();
	}
};


// Static - parsed to vertex attribute data
class InstanceArray
{
private:
	unsigned int			_num_instances;
	unsigned int			_instance_vbo;
	unsigned int			_geometry_vao;
	unsigned int			_geometry_vbo;

public:
	inline InstanceArray(unsigned int num_instances, std::vector<glm::vec2> positions)
	{
		_num_instances = num_instances;

		GLfloat vertex_positions[] = { -0.05f,  0.05f,
										0.05f, -0.05f,
									   -0.05f, -0.05f,

									   -0.05f,  0.05f,
										0.05f, -0.05f,
										0.05f,  0.05f };

		// Store instance data in an array buffer
		glGenBuffers(1, &_instance_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _instance_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * num_instances, &positions[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Generate geometry vao
		glGenVertexArrays(1, &_geometry_vao);
		glBindVertexArray(_geometry_vao);

		// Store vbo data to an array buffer
		glGenBuffers(1, &_geometry_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _geometry_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), &vertex_positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Set instance data
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, _instance_vbo);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(1, 1);
	}

	inline ~InstanceArray()
	{
		glDeleteBuffers(1, &_geometry_vbo);
		glDeleteBuffers(1, &_instance_vbo);
		glDeleteVertexArrays(1, &_geometry_vao);
	}

	inline void render()
	{
		glBindVertexArray(_geometry_vao);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _num_instances);
		glBindVertexArray(0);
	}
};


typedef InstanceArray	t_instance_array;
typedef Instance		t_instance;

#endif