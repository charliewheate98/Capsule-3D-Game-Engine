#ifndef __SSAO_PASS_H__
#define __SSAO_PASS_H__

#include "Math.h"	// Get the math class
#include "Pass.h"	// Get abstract class
#include "GBufferData.h"	// Get access to GBuffer data
#include "Rect.h"

GLuint MAX_SSAO_SAMPLE_RESOLUTION = 64;		// Define the default ssao sample resolution

// This pass will support real-time ambient occlusion as a post-effect
class SsaoPass : public Pass
{
private:
	GLuint					_noise_texture;		// The noise texture for sampling ssao
	GLuint					_u_projection;	// Projection matrix
	GLuint					_sample_res;	// Ssao sample resolution
	std::vector<GLuint>		_u_samples;		// Fragment samples
	std::vector<glm::vec3>	_ssao_kernals;	// Ssao kernal data

	std::vector<glm::vec3> ssaoNoise;
	unsigned int noiseTexture;
public:
	Fbo* _colour;
	Fbo* _colour_blur;

	// Constructor
	inline SsaoPass(std::vector<GLuint> shader_programs, size_t width, size_t height, GLuint sample_res = MAX_SSAO_SAMPLE_RESOLUTION)
	{
		Create(shader_programs, width, height, sample_res);		// Create post effect pass
	}

	// Deconstructor
	inline ~SsaoPass()
	{
		delete _colour;
		delete _colour_blur;
		delete _screen_rect;

		_fbos.clear();
		_ssao_kernals.clear();
		_u_samples.clear();

		glDeleteTextures(1, &_noise_texture);	// Delete noise texture
	}

	Rect* _rect;

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	// Initialise the post effect
	inline void Create(std::vector<GLuint> shader_programs, size_t width, size_t height, GLuint sample_res)
	{
		_shader_programs = shader_programs;		// Assign shader pointers
		_sample_res = sample_res;	// Assign sample resolution value

		_rect = new Rect((double)width, (double)height, 1.0f, true);

		// Create two frame buffers, one for ssao colour and another for ssao blur
		_fbos.push_back(new Fbo(width, height, { new FboAttachment(width, height, GL_RED, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, true));
		_fbos.push_back(new Fbo(width, height, { new FboAttachment(width, height, GL_RED, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, true));
		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::uniform_real_distribution<GLfloat> rand_floats(0.0f, 1.0f);	// Generate random floats between 0.0 and 1.0
		std::default_random_engine rand_generator;	// A generator for randomising floats

		// Create temp iterator var
		for (unsigned int i = 0; i < sample_res; ++i)	// Iterate through each sample...
		{
			glm::vec3 sample(rand_floats(rand_generator) * 2.0f - 1.0f, 
							 rand_floats(rand_generator) * 2.0f - 1.0f, 
				             rand_floats(rand_generator)); // the third parameter was wrong on this line
			
			sample = glm::normalize(sample);	// Normalise the sample
			sample *= rand_floats(rand_generator);	// Seed the randomisation
			
			float scale = (float)i / sample_res;	// Get pixel position in NDC about the resolution size

			scale = Math::lerpf(0.1f, 1.0f, scale * scale);		// Interpolate the scale
			sample *= scale;	// Scale the s and t values

			_ssao_kernals.push_back(sample);	// Assign sample to the kernal array
			
			_u_samples.push_back(glGetUniformLocation(shader_programs[0], ("samples[" + std::to_string(i) + "]").c_str()));		// Get each sample uniform location
		}
		
		// generate noise texture
		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(rand_floats(rand_generator) * 2.0 - 1.0, rand_floats(rand_generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
			ssaoNoise.push_back(noise);
		}
		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		glUseProgram(_shader_programs[0]);	// Use the first shader pass
		glUniform1i(glGetUniformLocation(shader_programs[0], "gPositionSS"), 0); // The positions texture in the gbuffer
		glUniform1i(glGetUniformLocation(shader_programs[0], "gNormalSS"), 1); // The normals texture in the gbuffer
		glUniform1i(glGetUniformLocation(shader_programs[0], "texNoise"), 2); // The albedospec texture in the gbuffer
		
		_u_projection = glGetUniformLocation(shader_programs[0], "proj");	// Get projection uniform

		glUseProgram(_shader_programs[1]);	// Use the second shader pass
		glUniform1i(glGetUniformLocation(shader_programs[1], "ssaoInput"), 0); // the positions texture in the gbuffer
	}

	// Virtual functions
	inline virtual void Update(double delta) {}
	inline virtual void Render()
	{
		_fbos[0]->Bind(); // bind ssao texture

		glClear(GL_COLOR_BUFFER_BIT); // clear colour data on the screen

		glUseProgram(_shader_programs[0]); // Use the first shader pass

		for (unsigned int i = 0; i < _sample_res; ++i)	// For each ssao sample...
			glUniform3fv(_u_samples[i], 1, glm::value_ptr(_ssao_kernals[i]));	// Assign kernal uniform data

		glUniformMatrix4fv(_u_projection, 1, GL_FALSE, glm::value_ptr(Content::_map->GetCamera()->GetProjectionMatrix()));	// Assign camera projection uniform data

		glActiveTexture(GL_TEXTURE0);	// Set active texture to index 0
		glBindTexture(GL_TEXTURE_2D, _g_buffer_data->GetAttachments()[6]->_texture);	// Bind positions
		glActiveTexture(GL_TEXTURE1);	// Set active texture to index 1
		glBindTexture(GL_TEXTURE_2D, _g_buffer_data->GetAttachments()[7]->_texture);	// Bind normals
		glActiveTexture(GL_TEXTURE2);	// Set active texture to index 2
		glBindTexture(GL_TEXTURE_2D, noiseTexture);	// Bind the noise texture

		_screen_rect->Render(1);		// Render to screen rectangle
	
		_fbos[0]->Unbind();


		// Blur ssao texture
		_fbos[1]->Bind();

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(_shader_programs[1]);	// Use the second shader pass

		glActiveTexture(GL_TEXTURE0);	// Bind active texture to index 0
		glBindTexture(GL_TEXTURE_2D, _fbos[0]->GetAttachments()[0]->_texture);	// Bind the final colour

		_screen_rect->Render(1);		// Render to screen rectangle
	
		_fbos[1]->Unbind();
	}
};

#endif