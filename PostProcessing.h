#ifndef __POSTPROCESSING_H__
#define __POSTPROCESSING_H__

#include "Math.h"
#include "Pass.h"
#include "GBufferData.h"
#include "Rect.h" 
#include "Primitives.h"

#define SHADOW_QUALITY 2048

class PostFX
{
protected:
	std::vector<GLuint> _shader_programs;

	Fbo* _fbo;
	Rbo* _rbo;
public:
	inline PostFX() = default;
	inline ~PostFX() {}

	inline Fbo* getFbo() { return _fbo; }

	inline virtual void Update(double delta) = 0;
	inline virtual void Render() = 0;
};

class Bloom : public PostFX
{
private:
	float			_intensity;

	uniform	_u_texturemap_h;
	uniform _u_texturemap_v;
	uniform	_u_blurres_h;
	uniform	_u_blurres_v;
	uniform	_u_texture_map;
	uniform	_u_blur_resolution;
public:
	Fbo* _h_blur;
	Fbo* _v_blur;

	inline Bloom(std::vector<GLuint> shader_programs, size_t width, size_t height, float intensity)
	{
		Create(shader_programs, width, height, intensity);
	}
	inline ~Bloom() {}

	inline void Create(std::vector<GLuint> shader_programs, size_t width, size_t height, float intensity)
	{
		_shader_programs = shader_programs;
		_intensity = intensity;

		_fbo = new Fbo(width, height, { new FboAttachment(width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0),
										new FboAttachment(width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT1) }, true);

		_h_blur = new Fbo(width, height, { new FboAttachment(width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0, false, true) }, false);
		_v_blur = new Fbo(width, height, { new FboAttachment(width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0, false, true) }, false);

		_u_texturemap_h = glGetUniformLocation(shader_programs[0], "textureMap");
		_u_texturemap_v = glGetUniformLocation(shader_programs[1], "textureMap");

		_u_blurres_h = glGetUniformLocation(shader_programs[0], "blur_resolution");
		_u_blurres_v = glGetUniformLocation(shader_programs[1], "blur_resolution");

		_u_texture_map = glGetUniformLocation(_shader_programs[0], "textureMap");
		_u_blur_resolution = glGetUniformLocation(_shader_programs[1], "blur_resolution");
	}

	inline float GetBloomIntensity() { return _intensity; }
	inline void SetBloomIntensity(float value) { _intensity = value; }	// Set bloom intensity

	inline virtual void Update(double delta) {}
	inline virtual void Render()
	{
		_h_blur->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shader_programs[0]);

		glUniform1i(_u_texture_map, 0);
		glUniform1f(_u_blur_resolution, 256.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _fbo->GetAttachments()[1]->_texture);

		_screen_rect->Render(1);


		_v_blur->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shader_programs[1]);

		glUniform1i(_u_texture_map, 0);
		glUniform1f(_u_blur_resolution, 256.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _h_blur->GetAttachments()[0]->_texture);

		_screen_rect->Render(1);

		_v_blur->Unbind();
	}
};

class Sharpen : public PostFX
{
private:
public:
	inline Sharpen() {}
	inline ~Sharpen() {}

	inline virtual void Update(double delta) {}
	inline virtual void Render() 
	{
		//_fbo_final->Bind();

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		//shaders[8]->UseProgram(); // bind the final gather shader (using additive blending to blend all the postFX needed)
		//
		//glUniform1i(_final_textures[0], 0);	// Bind the final scene texture
		//glActiveTexture(GL_TEXTURE0); // unit 1
		//
		//glBindTexture(GL_TEXTURE_2D, static_cast<Bloom*>(post_effects[0])->getFbo()->GetAttachments()[0]->_texture); // bind the scene texture
		//
		//_screen_rect->Render(1); // Render to screen rectangle
	}
};

class ColorCorrection : public PostFX
{
private:
	float _contrast;
	float _brightness;

public:
	uniform _u_contrast;
	uniform _u_brightness;

	inline ColorCorrection(unsigned int program, float contrast, float brightness)
	{
		Create(program, contrast, brightness);
	}
	inline ~ColorCorrection() {}

	inline void Create(unsigned int program, float contrast, float brightness)
	{
		_contrast = contrast;
		_brightness = brightness;

		_u_contrast = glGetUniformLocation(program, "contrast");
		_u_brightness = glGetUniformLocation(program, "brightness");
	}

	inline float getContrast() { return _contrast; }
	inline float getBrightness() { return _brightness; }

	inline virtual void Update(double delta) {}
	inline virtual void Render() {}
};

// This class handles generating a shadowmap and then using this to generate shadows
class Shadowmapping : public PostFX
{
private:
	size_t _shadowmap_resolution; // resolution of the shadowmap (2048x2048)

	uniform _u_lsm; // lightspacematrix uniform location
	uniform _u_mod; // model matrix uniform location
	uniform _u_texturemap_h; // the horizontal blur shadowmap texture 
	uniform _u_texturemap_v; // the vertical blur shadowmap texture 
	uniform _u_blurres_h; // the horizontal blur shadowmap texture 
	uniform _u_blurres_v; // the vertical blur shadowmap texture 

	glm::mat4 light_projection; // light projection onto the scene
	glm::mat4 light_view; // view of the scene from the lights position (sun)
	glm::mat4 light_space_matrix; // light view * light proj * inverse(cameraView) [VIEWSPACE]

	glm::mat4 shadow_light_projection[3];

	Fbo* _h_blur;
public:
	Fbo* _v_blur;

	float x;
	float y;
	float z;

	inline Shadowmapping(std::vector<GLuint> shader_programs, size_t shadow_resolution)
	{
		// Create the shadowmap
		Create(shader_programs, shadow_resolution);
	}
	inline ~Shadowmapping() {}

	inline glm::mat4 getLightSpaceMatrix() { return light_space_matrix; } // get the lightspacematrix used by the shadowmap

	inline void Create(std::vector<GLuint> shader_programs, size_t shadow_resolution)
	{
		_shader_programs = shader_programs; // initialise shaders 
		_shadowmap_resolution = shadow_resolution; // initialise shadowmap resolution

		// the shadowmap resolution must be above 0
		assert(_shadowmap_resolution > 0);

		/*
		* Initialise the depth map Frame Buffer Object
		*/
		_fbo = new Fbo(shadow_resolution, shadow_resolution, { new FboAttachment(shadow_resolution, shadow_resolution, GL_RG32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0, true, true) }, false);
		_rbo = new Rbo(shadow_resolution, shadow_resolution, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, 1);

		_h_blur = new Fbo(shadow_resolution, shadow_resolution, { new FboAttachment(shadow_resolution, shadow_resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, false);
		_v_blur = new Fbo(shadow_resolution, shadow_resolution, { new FboAttachment(shadow_resolution, shadow_resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, false);

		_u_lsm = glGetUniformLocation(shader_programs[0], "lightSpaceMatrix"); // load in the lightspacematrix from the firstpass shader
		_u_mod = glGetUniformLocation(shader_programs[0], "model"); // load in the model matrix from the firstpass shader

		_u_texturemap_h = glGetUniformLocation(shader_programs[1], "textureMap");
		_u_texturemap_v = glGetUniformLocation(shader_programs[2], "textureMap");
		_u_blurres_h = glGetUniformLocation(shader_programs[1], "blur_resolution");
		_u_blurres_v = glGetUniformLocation(shader_programs[2], "blur_resolution");

		x =  2.0f;
		y = 5.0f;
		z = 3.0f;
	}

	inline void BlurShadowmap()
	{
		_h_blur->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shader_programs[1]);

		glUniform1i(_u_texturemap_h, 0);
		glUniform1f(_u_blurres_h, 1024.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _fbo->GetAttachments()[0]->_texture);

		_screen_rect->Render(1);

		_v_blur->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shader_programs[2]);

		glUniform1i(_u_texturemap_v, 0);
		glUniform1f(_u_blurres_v, 1024.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _h_blur->GetAttachments()[0]->_texture);

		_screen_rect->Render(1);

		_v_blur->Unbind();
	}

	inline virtual void Update(double delta) {}

	inline virtual void Render()
	{
		glDisable(GL_BLEND);	 // Disable blending for opique materials
		glEnable(GL_DEPTH_TEST); // Enable depth test to avoid quads rendering on top of each other that shouldnt
		glDisable(GL_CULL_FACE); // enable cull face

		glm::mat4 model;  // model matrix for all the meshes in the shadowmap		
		{ // DIRECTIONAL SHADOWS
			light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f); // project onto the scene from the position of the light (sun)
			light_view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // position the camera at the lights position
			light_space_matrix = light_projection * light_view * glm::inverse(Content::_map->GetCamera()->GetViewMatrix()); // calculate the lightSpaceMatrix

			glUseProgram(_shader_programs[0]); // bind the first pass shader

			glUniformMatrix4fv(_u_lsm, 1, GL_FALSE, glm::value_ptr(light_space_matrix)); // set the lightSpaceMatrix uniform

			glViewport(0, 0, _shadowmap_resolution, _shadowmap_resolution); // set the viewport size to the resolution of the shadow map
			_fbo->Bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear any depth info

			// loop through all the meshes within the scene
			for (Actor* a : Content::_map->GetActors())
			{
				glUniformMatrix4fv(_u_mod, 1, GL_FALSE, glm::value_ptr(Content::_map->GetCamera()->GetViewMatrix() * a->GetModelMatrix())); // set the viewspace model matrix uniform

				if (a->GetObjectType() == MESH)
				{
					a->Render(); // render all the meshes into the shadowmap
					a->UpdateModel();
				}
			}

			_fbo->Unbind(); // unbind the shadowmap fbo

			BlurShadowmap();
		}

		glViewport(0, 0, 1920, 1080); // reset the viewport

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // glclear the gbuffer before rendering to it
		glEnable(GL_CULL_FACE); // enable cull face
	}
};

class VolumetricLightPass : public PostFX
{
private:
	uniform _u_view; // view matrix uniform location
	uniform _u_proj; // projection matrix uniform location
	uniform _u_mod; // model matrix uniform location
	uniform _u_objtype; // object type (light scatter caster or not) uniform location

	uniform _u_exposure; // light scatter exposure uniform location
	uniform _u_decay; // light scatter decay uniform location
	uniform _u_density; // light scatter density uniform location
	uniform _u_weight; // light scatter weight uniform location
	uniform _u_lightScreenPos; // light scatter screenspace light position uniform location
	uniform _u_lightScatterTexture; // light scatter texture uniform location
	uniform _u_noise_texture;
	uniform _u_horizontal_texturemap;
	uniform _u_horizontal_blurres;
	uniform _u_vertical_texturemap;
	uniform _u_vertical_blurres;

	GLuint _noise_texture;
	std::vector<glm::vec3>	_noise;

	Fbo* _h_blur;
public:
	Fbo* _fbo1;
	Fbo* _fbo2;
	Fbo* _v_blur;

	glm::vec2 lightScreenPosition;

	// Constructer
	inline VolumetricLightPass(std::vector<GLuint> shader_programs, size_t width, size_t height)
	{
		// Create the volumetric light fbo
		Create(shader_programs, width, height);
	}

	// Destructer
	inline ~VolumetricLightPass()
	{
		// delete the screen quad when not in need
		delete _screen_rect;

		// delete lightScatter texture when not needed
		glDeleteTextures(1, &_u_lightScatterTexture);
	}

	// returns the volumetric light scattering texture
	inline unsigned int GetLightScatterTexture() { return _v_blur->GetAttachments()[0]->_texture; }

	inline void Create(std::vector<GLuint> shader_programs, size_t width, size_t height)
	{
		_shader_programs = shader_programs;	// initialise local shader list

		_fbo1 = new Fbo(1920, 1080, { new FboAttachment(1920, 1080, GL_RGBA, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, false);
		_fbo2 = new Fbo(1920, 1080, { new FboAttachment(1920, 1080, GL_RGBA, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, false);

		_h_blur = new Fbo(1920, 1080, { new FboAttachment(1920, 1080, GL_RGBA, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, false);
		_v_blur = new Fbo(1920, 1080, { new FboAttachment(1920, 1080, GL_RGBA, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0) }, false);

		{
			_u_view = glGetUniformLocation(shader_programs[0], "view"); // load in the view matrix uniform from the light scatter shader
			_u_proj = glGetUniformLocation(shader_programs[0], "proj"); // load in the projection matrix uniform from the light scatter shader
			_u_mod = glGetUniformLocation(shader_programs[0], "model"); // load in the model matrix uniform from the light scatter shader
			_u_objtype = glGetUniformLocation(shader_programs[0], "obj_type"); // load in the object type uniform from the light scatter shader
		}

		{
			_u_exposure = glGetUniformLocation(shader_programs[1], "exposure"); // load in the exposure uniform from the radialblur shader (brightness of the volumetric light)
			_u_decay = glGetUniformLocation(shader_programs[1], "decay"); // load in the decay uniform from the radialblur shader (amount of light scattering)
			_u_density = glGetUniformLocation(shader_programs[1], "density"); // load in the density uniform from the radialblur shader
			_u_weight = glGetUniformLocation(shader_programs[1], "weight"); // load in the weight uniform from the radialblur shader
			_u_lightScreenPos = glGetUniformLocation(shader_programs[1], "lightPositionOnScreen"); // load in the lightPosOnScreen uniform from the radialblur shader (light position in screenspace)
			_u_lightScatterTexture = glGetUniformLocation(shader_programs[1], "lightScatterTexture"); // load in the sampler2D light scatter texture from the radialblur shader
		}

		_u_horizontal_texturemap = glGetUniformLocation(shader_programs[2], "textureMap");
		_u_horizontal_blurres = glGetUniformLocation(shader_programs[2], "blur_resolution");
		_u_vertical_texturemap = glGetUniformLocation(shader_programs[3], "textureMap");
		_u_vertical_blurres = glGetUniformLocation(shader_programs[3], "blur_resolution");
	}

	inline void Blur()
	{
		_h_blur->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shader_programs[2]);

		glUniform1i(_u_horizontal_texturemap, 0);
		glUniform1f(_u_horizontal_blurres, 256.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _fbo2->GetAttachments()[0]->_texture);

		_screen_rect->Render(1);

		_v_blur->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shader_programs[3]);

		glUniform1i(_u_vertical_texturemap, 0);
		glUniform1f(_u_vertical_blurres, 256.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _h_blur->GetAttachments()[0]->_texture);

		_screen_rect->Render(1);

		_v_blur->Unbind();
	}

	inline virtual void Update(double delta) {}
	inline virtual void Render()
	{
		glEnable(GL_CULL_FACE); // enable cull facing

		glm::mat4 model; // model matrix for each model in the light scatter texture
		glm::mat4 projection = Content::_map->GetCamera()->GetProjectionMatrix(); // camera projection
		glm::mat4 view = Content::_map->GetCamera()->GetViewMatrix(); // camera view

		_fbo1->Bind();  // bind the light scatter fbo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear any data on the screen 

		// set up the camera matrices
		glUseProgram(_shader_programs[0]); // use the gbuffer shader

		glUniformMatrix4fv(_u_view, 1, GL_FALSE, glm::value_ptr(view)); // set the model matrix uniform
		glUniformMatrix4fv(_u_proj, 1, GL_FALSE, glm::value_ptr(projection)); // set the model matrix uniform

		// render the light source
		model = glm::translate(glm::vec3(15.0f, 10.0f, 6.0f)) * glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(_u_mod, 1, GL_FALSE, glm::value_ptr(model)); // set the model matrix uniform
		glUniform3f(_u_objtype, 1.0f, 0.8f, 0.7f);
		
		Primitives::sphere();

		// render the models
		for (Actor* a : Content::_map->GetActors())
		{
			glUniformMatrix4fv(_u_mod, 1, GL_FALSE, glm::value_ptr(view * a->GetModelMatrix())); // set the model matrix uniform	
			glUniform3f(_u_objtype, 0.0f, 0.0f, 0.0f);

			if (a->GetObjectType() == MESH) // If object type is type mesh
				a->Render();
		}

		_fbo1->Unbind(); // unbind the fbo

		// blur pass
		_fbo2->Bind(); // bind the gbuffer fbo to start the geometry pass
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // glclear the gbuffer before rendering to it

		// bind the radial blur shader
		glUseProgram(_shader_programs[1]);

		// get the light position in screenspace
		glm::mat4 viewProjectionMatrix = projection * view;
		glm::vec4 lightNDCPosition = viewProjectionMatrix * glm::vec4(0.0f, 4.0f, 0.3f, 1);
		lightNDCPosition /= lightNDCPosition.w;
		lightScreenPosition = glm::vec2((lightNDCPosition.x + 1) * 0.5, (lightNDCPosition.y + 1) * 0.5);

		// set the uniforms for the light scattering
		glUniform1f(_u_exposure, 0.5050f);
		glUniform1f(_u_decay, 0.9f);
		glUniform1f(_u_density, 0.9f);
		glUniform1f(_u_weight, 0.9f);
		glUniform2f(_u_lightScreenPos, lightScreenPosition.x, lightScreenPosition.y); // TODO: Load this in within the constructer!	

		glUniform1i(_u_lightScatterTexture, 0); // TODO: Load this in within the constructer!

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _fbo1->GetAttachments()[0]->_texture);

		// render the quad
		_screen_rect->Render(1);

		_fbo2->Unbind(); // unbind the gbuffer, we dont need it anymore

		Blur();
	}
};

#endif