#ifndef __RBO_H__
#define __RBO_H__

#include <glew.h>	// Include opengl functions


// This class will be used for rendering to a frame buffer
class Rbo
{
private:
	size_t	_width;		// Render buffer width
	size_t	_height;		// Render buffer height
	GLuint	_rbo;	// Render buffer object
	GLenum	_internal_format;	// The internal colour format
	GLenum	_attachment;	// The attachment type
	GLuint	_num_samples;	// The number of attachment samples

public:
	// Default constructor
	inline Rbo() {}

	// Initial constructor
	inline Rbo(size_t width, size_t height, GLenum internal_format, GLenum attachment, GLuint num_samples)
	{
		_width = width;		// Assign width
		_height = height;	// Assign height
		_internal_format = internal_format;		// Assign internal format
		_attachment = attachment;	// Assign attachment type
		_num_samples = num_samples;		// Assign num samples

		Create();		// Create render buffer object
	}

	// Deconstructor
	inline ~Rbo()
	{
		glDeleteRenderbuffers(1, &_rbo);	// Delete the render buffer object
	}

	// Get the render buffer object
	inline GLuint &GetRenderBufferObject()
	{
		return _rbo;	// Return the render buffer objcet
	}

	// Get the format of rbo
	inline GLenum &GetFormat()
	{
		return _internal_format;	// Return the format
	}

	// Get the attachment type
	inline GLenum &GetAttachment()
	{
		return _attachment;		// Return the attachment type
	}

	// Get num samples
	inline GLuint &GetNumSamples()
	{
		return _num_samples;	// Return the num samples
	}
	
	// Create the render buffer object
	inline void Create()
	{
		glGenRenderbuffers(1, &_rbo);	// Generate the render buffer object
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo);	// Bind the render buffer object
		
		if (_num_samples > 1)	// If the num samples if greater than one...
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, _num_samples, _internal_format, _width, _height);	// Buffer a multisample
		else	// Otherwise...
			glRenderbufferStorage(GL_RENDERBUFFER, _internal_format, _width, _height);	// Create a singular buffer storage

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, _attachment, GL_RENDERBUFFER, _rbo);	// Render the frame buffer target
	
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)	// If the frame buffer failed to complete
		{
			std::cout << "Error: Frame buffer failed to complete!" << std::endl;	// Print error message
			return;		// Return before unbinding buffer
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Unbind buffer from memory
	}
};

#endif