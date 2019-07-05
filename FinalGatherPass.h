#ifndef __FINALGATHER_H__
#define __FINALGATHER_H__

#include "Engine/Pass.h"
#include "Engine/PostProcessing.h"

class FinalGatherPass : public Pass
{
private:
	GLuint _final_textures[4];
	GLuint _bloom_intensity;

	std::vector<PostFX*>			post_effects;
public:
	inline FinalGatherPass(std::vector<PostFX*> _post_effects, std::vector<GLuint> shader_programs) { Create(_post_effects, shader_programs); }
	inline ~FinalGatherPass() {}

	inline void Create(std::vector<PostFX*> _post_effects, std::vector<GLuint> shader_programs)
	{
		_shader_programs = shader_programs;
		post_effects = _post_effects;

		_final_textures[0] = glGetUniformLocation(shader_programs[0], "scene"); // load in the scene texture from the final gather shader
		_final_textures[1] = glGetUniformLocation(shader_programs[0], "bloom"); // load in the bloom texture from the final gather shader
		_final_textures[2] = glGetUniformLocation(shader_programs[0], "lightScattering"); // load in the bloom texture from the final gather shader
		_bloom_intensity   = glGetUniformLocation(shader_programs[0], "bloom_intensity");
	}

	inline void addTexture(unsigned int unit, unsigned int texture) 
	{
		glActiveTexture(GL_TEXTURE + unit); // unit 0
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	inline virtual void Update(double delta) {}
	inline virtual void Render() 
	{
		// ------------------------------ FINAL GATHER -------------------------------
		glUseProgram(_shader_programs[0]);

		glUniform1i(_final_textures[0], 0); // the captured scene texture
		glUniform1i(_final_textures[1], 1); // the bloom texture
		glUniform1i(_final_textures[2], 2); // the light scatter texture

		glUniform1f(_bloom_intensity, static_cast<Bloom*>(post_effects[0])->GetBloomIntensity()); // bloom intensity (higher the value the less bloom)
		glUniform1f(static_cast<ColorCorrection*>(post_effects[1])->_u_contrast, static_cast<ColorCorrection*>(post_effects[1])->getContrast()); // contrast of the final scene
		glUniform1f(static_cast<ColorCorrection*>(post_effects[1])->_u_brightness, static_cast<ColorCorrection*>(post_effects[1])->getBrightness()); // brightness of the final scene

		glActiveTexture(GL_TEXTURE0); // unit 0
		glBindTexture(GL_TEXTURE_2D, static_cast<Bloom*>(post_effects[0])->getFbo()->GetAttachments()[0]->_texture); // bind the scene texture
		glActiveTexture(GL_TEXTURE1); // unit 1
		glBindTexture(GL_TEXTURE_2D, static_cast<Bloom*>(post_effects[0])->_v_blur->GetAttachments()[0]->_texture); // bind the bloom texture
		glActiveTexture(GL_TEXTURE2); // unit 1
		glBindTexture(GL_TEXTURE_2D, static_cast<VolumetricLightPass*>(post_effects[3])->GetLightScatterTexture()); // bind the bloom texture

		_screen_rect->Render(1); // Render to screen rectangle
		// ---------------------------------------------------------------------------- //
	}
};

#endif
