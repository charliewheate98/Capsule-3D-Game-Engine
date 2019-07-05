#ifndef __FBO_H__
#define __FBO_H__

#include <vector>	// Include the vector list
#include <glew.h>	// Include opengl functions


// This struct will contain the data for an fbo attachment
struct FboAttachment
{
	size_t	_width;		// The attachment width
	size_t	_height;	// The attachment height
	GLuint	_texture;	// The texture id
	GLint	_internal_format;	// The interal format
	GLenum	_format;	// The format
	GLenum	_type;	// The attachment value type
	GLenum	_attachment;	// The attachment type
	bool    _mipmapping; // if true then the texture has mipmapping support
	bool    _border_clamping; // if true then this removes any border artifacts (mainly used for shadow artifacts)

	// Initial constructor
	inline FboAttachment(size_t width, size_t height, GLenum internal_format, GLint format, GLenum type, GLenum attachment, bool mipmapping = false, bool border_clamping = false)
	{
		_width = width;		// Assign width
		_height = height;	// Assign height
		_internal_format = internal_format;		// Assign internal format
		_format = format;	// Assign format
		_type = type;	// Assign value type
		_attachment = attachment;	// Assign attachment type
		_mipmapping = mipmapping;
		_border_clamping = border_clamping;
	}

	// Deconstructor
	inline ~FboAttachment()
	{
		glDeleteTextures(1, &_texture);		// Delete the texture map
	}

	// This function will create an fbo attachment
	inline void Create()
	{
		// Generate a texture and sets its data and information
		glGenTextures(1, &_texture);	// Generate the colour texture
		glBindTexture(GL_TEXTURE_2D, _texture);		// Bind the texture map
		glTexImage2D(GL_TEXTURE_2D, 0, _internal_format, _width, _height, 0, _format, _type, 0);	// Store the texture data to a buffer
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Set the linear filter for min
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _mipmapping == true ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);	// Set the linear filter for mag

		/*
		* If border clamping is enabled then set the border colour (mainly used for shadow mapping to remove peter panning)
		*/
		if (_border_clamping)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			GLfloat border[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
		}

		/*
		* If mipmapping enabled then generate mipmaps for this FBO texture. 
		*/
		if (_mipmapping)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0); // set the minimum texture mip level
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4); // set the maximum texture mip level
			glGenerateMipmap(GL_TEXTURE_2D); // generate a mipmap for the shadowmap
		}

		// Send this generated texture to the framebufferobject
		glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, GL_TEXTURE_2D, _texture, 0);		// Assign the texture to the frame buffer as an attachment

		// Check for any problems with the frame buffer object
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error : FBO Could not be created!" << std::endl;
	}
};


// This class will store a frame buffer object
class Fbo
{
private:
	GLuint							_fbo;	// The frame buffer object
	GLuint							_texture;	// The texture for colour attachment
	size_t							_width;		// The frame buffer width
	size_t							_height;	// The frame buffer height
	std::vector<FboAttachment*>		_attachments;	// List of attachments

public:
	// Default constructor
	inline Fbo() = default;

	// Initial constructort
	inline Fbo(size_t width, size_t height, std::vector<FboAttachment*> attachments, bool drawBuffers)
	{
		Create(width, height, attachments, drawBuffers);		// Create the frame buffer object
	}

	// Deconstructor
	inline ~Fbo()
	{
		_attachments.clear();	// Delete all attachments

		glDeleteFramebuffers(1, &_fbo);		// Delete the frame buffer object
	}

	// Get the frame buffer object
	inline GLuint &GetFrameBufferObject()
	{
		return _fbo;	// Return the frmae buffer objet
	}

	// Get the texture colour attachment map
	inline GLuint &GetTexture()
	{
		return _texture;	// Return the texture map
	}

	// Get the width of frame buffer
	inline size_t &GetWidth()
	{
		return _width;	// Return the width of frame buffer
	}

	// Get the height of frame buffer
	inline size_t &GetHeight()
	{
		return _height;	// Return the height of frame buffer
	}

	// Get fbo attachments
	inline std::vector<FboAttachment*>	&GetAttachments()
	{
		return _attachments;	// return the attachments
	}

	// Create the frame buffer object
	inline void Create(size_t width, size_t height, std::vector<FboAttachment*> attachments, bool drawBuffers)
	{
		_width = width;		// Assign width
		_height = height;	// Assign height
		_attachments = attachments;		// Assign attachments

		glGenFramebuffers(1, &_fbo);	// Generate the frame buffer object
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);	// Bind the frame buffer object

		std::vector<GLenum> buffers;	// Temp buffer container
		for (FboAttachment* a : _attachments)	// For each attachment...
		{
			buffers.push_back(a->_attachment);	// Assign attachment type to buffer list
			a->Create();	// Generate the attachment
		}

		if(drawBuffers)
			glDrawBuffers(buffers.size(), &buffers[0]);		// Draw all buffer attachments
	}

	/*
	* Read the fbo and get the data stored in the specfied renderf target
	*/
	inline void Resolve(int read_buffer, Fbo* output_fbo)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, output_fbo->GetFrameBufferObject());
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->GetFrameBufferObject());
		glReadBuffer(read_buffer);
		glBlitFramebuffer(0, 0, _width, _height, 0, 0, output_fbo->GetWidth(), output_fbo->GetHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	inline void ResolveToForward(Fbo* output_fbo)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, output_fbo->GetFrameBufferObject());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBlitFramebuffer(0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// This function binds the frame buffer object
	inline void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);	// Bind the frame buffer object
	}

	inline void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

#endif