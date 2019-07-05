// start of class
#ifndef __IBL_H__ 
#define __IBL_H__

// c++ classes and libs
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// other classes needed for the PBR
#include "Primitives.h"
#include "Rbo.h"
#include "Content.h"
#include "GBufferData.h"

// namespace which stores all the classes and data for calculating PBR
namespace PBR
{
	// the HDR cubemap capture buffer objects
	unsigned int capture_fbo;
	unsigned int capture_rbo;

	// initialise the capture projection matrix
	glm::mat4 _capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	// initialise the different lookat matrices, to capture each face
	// to store within a cubemap
	glm::mat4 _capture_views[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// This struct handles generating a cubemap out of the loaded in HDR image and capture matrices
	struct EquirectangularMap
	{
		// Default Constructer
		inline EquirectangularMap() = default;

		// Constructer : the constructer generates a cubemap out of the projection matrices specfied above
		inline EquirectangularMap(GLuint shader_program, unsigned int env_map, unsigned int c_fbo, unsigned int c_rbo, 
			Texture::TextureHDR* _texture_hdr)
		{
			// Use the Equirectangular to cubemap shader
			glUseProgram(shader_program);

			// Uniforms
			glUniform1i(glGetUniformLocation(shader_program, "equirectangularMap"), 0);
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "proj"), 1, GL_FALSE, glm::value_ptr(_capture_projection));

			// bind the hdr map
			_texture_hdr->Render();

			// downsample the resoulation down to 512
			glViewport(0, 0, 512, 512);

			// bind the capture fbo to capture all the faces
			glBindFramebuffer(GL_FRAMEBUFFER, c_fbo); 

			// loop through all 6 views, storing each in the environment map
			for (unsigned int i = 0; i < 6; ++i)
			{
				// view matrices
				glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(_capture_views[i]));
				// this function stores the texture in the fbo
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env_map, 0);
				// clear the screen
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// render the cube
				Primitives::cube();
			}

			// unbind the fbo
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
			glBindTexture(GL_TEXTURE_CUBE_MAP, env_map);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		// generate the capture fbo and rbo
		inline void CreateCaptureFBO()
		{
			glGenFramebuffers(1, &capture_fbo);
			glGenRenderbuffers(1, &capture_rbo);

			glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
			glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_rbo);
		}
	};
	
	// Solve diffuse integral by convolution to create an irradiance (cube)map.
	struct IrradianceMap
	{
		// the irradianceMap texture ID
		unsigned int irradianceMap;

		// texture cubemap class in order to generate a empty cubemap
		Texture::TextureCubemap* _env_map_hdr;

		// Default constructer
		inline IrradianceMap() = default;

		// Constructer : within this constructer we generate a empty cubemap,  
		inline IrradianceMap(GLuint shader_program, unsigned int env_map)
		{
			// Generate an empty cubemap
			irradianceMap = _env_map_hdr->GenerateEmptyCubemap(32, 32);

			// bind the capture fbo and the rbo
			glBindFramebuffer(GL_FRAMEBUFFER, PBR::capture_fbo);
			glBindRenderbuffer(GL_RENDERBUFFER, PBR::capture_rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

			// start the irradiance shader
			glUseProgram(shader_program);

			// uniforms
			glUniform1i(glGetUniformLocation(shader_program, "environmentMap"), 0);
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "proj"), 1, GL_FALSE, glm::value_ptr(_capture_projection));
			
			// bind the environment map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, env_map);

			// downsample the resolution to 32x32
			glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.

			// Bind the capture fbo
			glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
			for (unsigned int i = 0; i < 6; ++i)
			{
				// set the view matrices
				glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(_capture_views[i]));
				// send the irradiance map to the fbo
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
				// clear the screen
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// render cube
				Primitives::cube();
			}

			// unbind the capture fbo
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Get the irradiance texture
		unsigned int GetIrradianceMap() { return irradianceMap; }
	};
	
	// Prefilter the environment map using mip mapping
	struct PrefilterMap
	{
		// prefilter map texture ID
		unsigned int prefilterMap;

		// texture cubemap class in order to generate a empty cubemap
		Texture::TextureCubemap* _env_map_hdr;

		// default Constructer
		inline PrefilterMap() = default;

		// Constructer uses the prefilter shader to make the prefilter map usig mip mapping
		inline PrefilterMap(GLuint shader_program, unsigned int env_map)
		{
			// generate an empty cubemap
			prefilterMap = _env_map_hdr->GenerateEmptyCubemap(128, 128, true);

			// use the prefilter shader
			glUseProgram(shader_program);
			
			// uniforms
			glUniform1i(glGetUniformLocation(shader_program, "environmentMap"), 0);
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "proj"), 1, GL_FALSE, glm::value_ptr(_capture_projection));
			
			// bind the environment map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, env_map);

			// bind the capture fbo
			glBindFramebuffer(GL_FRAMEBUFFER, PBR::capture_fbo);

			// use mip mapping to do the prefiltering
			unsigned int maxMipLevels = 5;
			for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
			{
				// reisze framebuffer according to mip-level size.
				double mipWidth = 128 * std::pow(0.5, mip);
				double mipHeight = 128 * std::pow(0.5, mip);
				glBindRenderbuffer(GL_RENDERBUFFER, PBR::capture_rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (GLsizei)mipWidth, (GLsizei)mipHeight);
				glViewport(0, 0, (GLsizei)mipWidth, (GLsizei)mipHeight);

				float roughness = (float)mip / (float)(maxMipLevels - 1);
				glUniform1f(glGetUniformLocation(shader_program, "roughness"), roughness);
				for (unsigned int i = 0; i < 6; ++i)
				{
					// set the view uniform matrix
					glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(_capture_views[i]));

					// send the prefilter map to the fbo
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

					// clear screen
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					// render cube
					Primitives::cube();
				}
			}
			
			// unbind the fbo
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// get the prefilter map
		unsigned int GetPrefilterMap() { return prefilterMap; }
	};
	
	// create a BRDF texture which stores how light is reflected on the surface
	struct BRDF
	{
		// brdf texture ID
		unsigned int brdfLUTTexture;

		// Default constructer
		inline BRDF() = default;

		// generate the brdf 2D texture and render to the screen using a quad
		inline BRDF(GLuint shader_program)
		{
			// generate texture
			glGenTextures(1, &brdfLUTTexture);

			// pre-allocate enough memory for the LUT texture.
			glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

			// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// bind the the fbo and rbo and store the brdf texture within the fbo
			glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
			glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

			// render the brdf texture to the screen
			glViewport(0, 0, 512, 512);
			glUseProgram(shader_program);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_screen_rect->Render(1);

			// unbind the fbo
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// reset the viewport dimensions
			glViewport(0, 0, 1920, 1080);
		}

		// get the brdf texture
		unsigned int GetBRDFTexture() { return brdfLUTTexture; }
	};

	// The IBL class compiles all the code from the above structs to finalise the PBR lighting 
	class IBL
	{
	private:
		// shaders programs used within this class
		std::vector<GLuint> _shader_programs;

		// environment map texture ID
		unsigned int _env_map;

		// hdr image file 
		Texture::TextureHDR*		_texture_hdr;

		// environment cubemap
		Texture::TextureCubemap* _env_map_hdr;
		
		// the nessasary PBR maps 
		PBR::EquirectangularMap* _equi_map;
		PBR::IrradianceMap* _irr_map;
		PBR::PrefilterMap* _prefilter_map;
		PBR::BRDF* _brdf;
	public:
		// Default constructer
		inline IBL() = default;

		// initialise all the nessasary PBR maps 
		inline IBL(const char* hdr_file, std::vector<GLuint> shader_programs) { Create(hdr_file, shader_programs); }

		// delete any unneeded allocated memory
		~IBL()
		{
			// delete environment map
			glDeleteTextures(1, &_env_map);

			// clear shader vector list
			_shader_programs.clear();

			// delete pointers
			delete _texture_hdr;
			delete _env_map_hdr;
			delete _equi_map;
			delete _irr_map;
			delete _prefilter_map;
			delete _brdf;
		}

		// get the environment map
		inline unsigned int GetEnvironmentMap() { return _env_map; }

		// initialise all the nessasary PBR maps 
		inline void Create(const char* hdr_file, std::vector<GLuint> shader_programs)
		{
			// initialise the shader programs
			shader_programs = _shader_programs;

			// create the capturing fbo
			_equi_map->CreateCaptureFBO();

			// load in the hdr image
			_texture_hdr   = new Texture::TextureHDR(hdr_file);

			// generate the environment map
			_env_map       = _env_map_hdr->GenerateEmptyCubemap(512, 512);
			
			// initialise the maps
			_equi_map      = new PBR::EquirectangularMap(shader_programs[0], _env_map, PBR::capture_fbo, PBR::capture_rbo, _texture_hdr);
			_irr_map       = new PBR::IrradianceMap(shader_programs[1], _env_map);
			_prefilter_map = new PBR::PrefilterMap(shader_programs[2], _env_map);
			_brdf		   = new PBR::BRDF(shader_programs[3]);
		}

		// bind the pbr maps to the screen
		inline void Render()
		{
			// irradiance map
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _irr_map->GetIrradianceMap());

			// prefilter map
			glActiveTexture(GL_TEXTURE11);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _prefilter_map->GetPrefilterMap());

			// brdf texture 
			glActiveTexture(GL_TEXTURE12);
			glBindTexture(GL_TEXTURE_2D, _brdf->GetBRDFTexture());
		}
	};
};

// end of class
#endif	
