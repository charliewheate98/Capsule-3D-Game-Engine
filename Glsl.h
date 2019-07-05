#ifndef __GLSL_H__
#define __GLSL_H__

#define MAX_SHADER_ATTACHMENTS	8	// Define a maximum amount of shader attachments for each shader program

#include <glew.h>	// Include the GLEW extensions
#include <iostream>	// IOstream for debugging
#include <vector>	// Vector for dynamic arays
#include <fstream>	// FStream for opening shader files
#include <sstream>	// SStream for converting file contents to a string


// This class will contain the key data for creating a shader attachment
class ShaderAttachment
{
protected:
	unsigned int		_type;		// Represents the shader type
	GLuint				_shader;	// This is the shader identifier
	GLint				_result;	// Our result for compiling
	std::string			_glsl_code;	// Our glsl context from file

public:
	// Default constructor
	inline ShaderAttachment() {}

	// Initial constructor
	inline ShaderAttachment(GLuint program, const GLchar* file, GLuint type) 
	{
		std::string file_uri = "Res/Shaders/";	// Pre-calculate uri directory
		Create(program, (file_uri + file).c_str(), type);	// Create a shader by extracting information from a file
	}

	inline unsigned int GetType() { return _type; }		// Return the shader type
	inline GLuint GetShader() { return _shader; }	// Return the shader handle

	// A create function to form a shader
	inline void Create(GLuint &program, const GLchar* file, GLuint type)
	{
		_type = type;	// Assign the shader type

		std::ifstream		_file;		// Our glsl file
		std::stringstream	_stream;	// For assigning our file contents

		_file.exceptions(std::ifstream::badbit);	// If the file successfully opened...
		try		// Attempt...
		{
			_file.open(file);	// Open the shader file
			_stream << _file.rdbuf();	// Read the file's buffer contents into a stream
			_file.close();	// Close the file now that we've consumed the data
			_glsl_code = _stream.str();		// Convert the stream into a string
		}
		catch (std::ifstream::failure e)	// Otherwise...
		{
			std::cout << "Shader Error: File was unsuccessfully read!\n"; // Call an error message
			return;		// Return out of this function
		}

		const GLchar* final_glsl_code = _glsl_code.c_str();	// Convert string into OpenGL const char format

		GLchar log[512];	// Create a buffer for debugging glsl error information

		_shader = glCreateShader(type);		// Create our OpenGL shader as VERTEX
		glShaderSource(_shader, 1, &final_glsl_code, NULL);		// Feed OpenGL our glsl source code
		glCompileShader(_shader);	// Compile our shader

		glGetShaderiv(_shader, GL_COMPILE_STATUS, &_result);	// Check compile status for any errors
		if (!_result)	// If it failed to compile...
		{
			glGetShaderInfoLog(_shader, 512, NULL, log);	// Get the error information from OpenGL end
			std::cout << "Shader Error: Shader attachment failed to compile!\n" << log << std::endl;	// Print our the error information
			return;		// Return out of this function
		}
	}
};


// This class will contain an array of shader attachments and act as a main shader program
class ShaderProgram
{
private:
	GLint							_result;	// Our result for linking
	GLuint							_program;	// Our shader program handler
	std::vector<ShaderAttachment>	_attachments;	// This list will contain our shader attachments

public:
	// Our default constructor
	inline ShaderProgram() {}

	// Our initial constructor
	inline ShaderProgram(std::vector<ShaderAttachment> attachments) 
	{
		_attachments = attachments;		// Assign our attachments

		if (!LinkProgram())		// If linking  the attachmens has failed...
			std::cout << "Shader Program Error: Failed to link program!\n";		// Print out an error message
	}

	// The deconstructor will delete our shader program
	inline ~ShaderProgram()
	{
		glDeleteProgram(_program);	// Delete our shader program to save memory leakage
	}

	inline GLuint GetProgram() { return _program; }	// Return program identifier
	inline std::vector<ShaderAttachment> GetAttachments() { return _attachments; }	// Return our shader attachments

	// A function to add an attachment to our vectorlist
	inline bool AddShaderAttachment(const GLchar* file, GLuint type)
	{
		if (_attachments.size() > MAX_SHADER_ATTACHMENTS)	// If the amount of shader attachments exceed the limit
		{
			std::cout << "Shader Program Error: Program attachments exceeds the maximum of " << (int)MAX_SHADER_ATTACHMENTS << "!\n";	// Print message
			return false;	// Return false as an error
		}

		_attachments.push_back(ShaderAttachment(_program, file, type));	// Assign the new shader attachment

		return true;	// We can return true
	}

	// This function will link all shader attachments for our program
	inline bool LinkProgram()
	{
		_program = glCreateProgram();	// Create our shader program

		for (unsigned int i = 0; i < _attachments.size(); i++)	// Iterate through all of our attachments...
			glAttachShader(_program, _attachments[i].GetShader());	// Attach all shaders to the program

		glLinkProgram(_program);	// Link the shader program

		GLchar log[256];	// Create a buffer for debugging link error information

		glGetProgramiv(this->_program, GL_LINK_STATUS, &_result);	// Get the linking status from OpenGL
		if (!_result)	// If the result returns invalid...
		{
			glGetProgramInfoLog(this->_program, 512, NULL, log);	// Gather the error information
			std::cout << "Shader Program Error: Failed to attach shaders!\n" << log << std::endl;	// Print error message
			return false;	// Return false as an error
		}

		for (unsigned int i = 0; i < _attachments.size(); i++)	// Iterate through all of our attachments...
			glDeleteShader(_attachments[i].GetShader());	// Delete shader context

		return true;
	}

	inline void UseProgram() { glUseProgram(_program); }	// Bind our program (ATTACHMENTS MUST BE LINKED!)
	inline void DetachProgram() { glUseProgram(0); }	// Unbind our program
};

#endif
