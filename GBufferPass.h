#ifndef __G_BUFFER_H__ 
#define __G_BUFFER_H__

#include "Pass.h"	// Get access to abstract header
#include "GBufferData.h"	// Get GBuffer data

#define ATTACHMENT_POSITION			0	// Define position attachment index
#define ATTACHMENT_NORMAL			1	// Define normal attachment index
#define ATTACHMENT_ALBEDO			2	// Define albedo attachment index
#define ATTACHMENT_SPECROUGH		3	// Define specular + roughness attachment index
#define ATTACHMENT_METALNESS		4	// Define metalness attachment index
#define ATTACHMENT_EMISSIVE			5	// Define emissive attachment index
#define ATTACHMENT_POSITION_SS		6	// Define normal screen space attachment index
#define ATTACHMENT_NORMAL_SS		7	// Define position screen space attachment index

// This class will store the g buffer pass in screenspace coords
class GBufferPass : public Pass
{
private:
	size_t	_num_attachments;	// The number of attachments
	bool    _enableGI;
public:
	// Constructor
	inline GBufferPass(GLuint shader_program, size_t width, size_t height)
	{
		Create(shader_program, width, height);	// Create the post effect
	}

	// Initialise gbuffer
	inline void Create(GLuint shader_program, size_t width, size_t height)
	{
		_shader_programs.push_back(shader_program);		// Assign shader program

		// Generate frame buffer object and it's attachments
		_g_buffer_data = new Fbo(width, height, {	new FboAttachment(width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0, false, true),	// Position colour attachment
													new FboAttachment(width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT1),	// World Normal colour attachment
													new FboAttachment(width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT2),		// Albedo colour attachment
													new FboAttachment(width, height, GL_RGB, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT3),		// Specular + roughness colour attachment
													new FboAttachment(width, height, GL_RGB, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT4),	// Metalness colour attachment
													new FboAttachment(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT5),	// Emissive colour attachment
													new FboAttachment(width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT6, false, true),	// Position screen space attachment
													new FboAttachment(width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT7) }, true);	// Normal screen space attachment
		
		_rbo = new Rbo(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, 1);	// Generate render buffer object
		
		_fbos.push_back(_g_buffer_data);	// Assign fbo GBuffer data
		
		_num_attachments = _fbos[0]->GetAttachments().size();	// Pre-calculate the number of attachments
	}

	// Virtual functions
	inline virtual void Update(double delta) {}
	inline virtual void Render()
	{
		glUseProgram(_shader_programs[0]);	// Use shader program

		for (size_t i = 0; i < _num_attachments; i++)		// For each attachment...
		{
			glActiveTexture(GL_TEXTURE0 + i);	// Assign active texture
			glBindTexture(GL_TEXTURE_2D, _fbos[0]->GetAttachments()[i]->_texture);	// Bind texture attachments
		}
	}
};

#endif