#ifndef __CONTEX_H__
#define __CONTEX_H__

#include <iostream>		// For debugging purposes
#include <glew.h>		// And glew for extensive functions
#include <wglew.h>		// And wglew for setting major and minor context versions
#include <GL/gl.h>		// Include basic gl variables
#include <GL/glu.h>		// Also include glu variables

/* ----- This is the main context class:

This deals with the device as well as the rendering
context for handling OpenGL 3.x - 4.x. It also covers 
window re-size updating the frame ratio, and cleaning up 
any allocated data (called on WM_DESTROY).

-------------------------------------------------*/

class Context
{
private:
	int		_width;		// Width of frame
	int		_height;	// Height of frame

protected:
	HGLRC	_hrc;	// Rendering context
	HDC		_hdc;	// Device context
	HWND	_hwnd;	// Window identifier

public:
	// Clear both device and rendering context data
	inline void Destroy()
	{
		wglMakeCurrent(_hdc, 0);	// Remove the rendering context from our device context
		wglDeleteContext(_hrc);		// Delete our rendering context

		ReleaseDC(_hwnd, _hdc);		// Release the device context from our window
	}

	// Create our OpenGL 3.x - 4.x context
	inline bool Create(HWND& hwnd)	
	{
		_hwnd = hwnd;	// Set the HWND for our window
		_hdc = GetDC(hwnd);		// Get the device context for our window

			// Create a new PIXELFORMATDESCRIPTOR (PFD)
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));		// Clear our PFD
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);	// Set the size of the PFD to the size of the class
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;	// Enable double buffering, OpenGL support and draw to window
		pfd.iPixelType = PFD_TYPE_RGBA;		// Set our application to use RGBA pixels
		pfd.cColorBits = 32;	// Give us 32 bits of colour information (8 bits each channel)
		pfd.cDepthBits = 32;	// Give us 32 bits of depth information
		pfd.iLayerType = PFD_MAIN_PLANE;	// Set the layer of the PFD

		int pixel_format = ChoosePixelFormat(_hdc, &pfd);	// Check if our PFD is valid and get a pixel format back
		if (pixel_format == 0)		// If it fails...
			return false;

		SetPixelFormat(_hdc, pixel_format, &pfd);		// Try and set the pixel format based on our PFD

		HGLRC temp_opengl_context = wglCreateContext(_hdc);		// Create an OpenGL context for our device context
		wglMakeCurrent(_hdc, temp_opengl_context);	// Make this OpenGL context current and active

		GLenum error = glewInit();	// Enable GLEW
		if (error != GLEW_OK)	// If GLEW fails...
			return false;

		int attribs[] = {	// Attributes for defining OpenGL versions and compatibilities
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,	// Set the MAJOR version of OpenGL to 4
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,	// Set the MINOR version of OpenGL to 2
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 0,
			WGL_SAMPLES_ARB, 4 // Set our OpenGL context to be forward compatible
		};

		if (wglewIsSupported("WGL_ARB_create_context") == 1)	// If OpenGL 4.x context creation extension is available...
		{
			_hrc = wglCreateContextAttribsARB(_hdc, NULL, attribs);		// Create an OpenGL 4.x context based on the given attribs
			wglMakeCurrent(NULL, NULL);		// Remove the temporary context from being active
			wglDeleteContext(temp_opengl_context);	// Delete the temporary OpenGL 3.2 context
			wglMakeCurrent(_hdc, _hrc);		// Make our OpenGL 4.x context current
		}

		// ------------------------------ DEBUGGING PURPOSES! -------------------------------------
		int glVersion[2] = { -1, -1 };	// Set some default values for the version
		glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);	// Get back the OpenGL MAJOR version we are using
		glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);	// Get back the OpenGL MINOR version we are using
		std::cout << "Using OpenGL context versions " << glVersion[0] << "." << glVersion[1] << std::endl;
		// ----------------------------------------------------------------------------------------


		return true;	// We have successfully created a context, reutrn true
	}

	// Function to get our frame width and height on resize
	inline void Reshape(int w, int h)
	{
		_width = w;		// Set the frame width
		_height = h;	// Set the frame height

		glViewport(0, 0, _width, _height);	// Set the viewport size to fill the window
	}

	// A swap buffer function for double buffering
	inline void Swap()
	{
		SwapBuffers(_hdc);	// Swap buffers so we can see our rendering
	}
};

#endif