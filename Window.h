#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <windows.h>	// Include windows for WinAPI framework
#include "Callback.h"	// Our callback events
#include "Globals.h"	// Access our global variables

bool		_fullscreen;		// Is the window fullscreen mode?
WNDCLASS	_window_class;		// Window class information (icons, cursors ect...)
DWORD		_dw_ex_style;		// Window extended style information
DWORD		_dw_style;			// Window style information
HINSTANCE	_h_instance;		// The HINSTANCE of this application


// Get the screen width in pixels
DWORD GetScreenWidth()
{
	return  GetSystemMetrics(SM_CXSCREEN);	// Get the x resolution of screen
}

// Get the screen height in pixels
DWORD GetScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);	// Get the y resolution of screen
}

// This turns on fullscreen mode for our application
bool SetFullscreen(DWORD width, DWORD height)
{
	DEVMODE newSettings;	// Our settings variable for new window adjustments

	SetWindowRgn(_h_wnd, 0, false);		// Turn off window region without redraw
	EnumDisplaySettings(0, 0, &newSettings);	// Request current screen settings

	newSettings.dmPelsWidth = width;	//  Set desired screen width
	newSettings.dmPelsHeight = height;	//  Set desired screen height
	newSettings.dmBitsPerPel = 32;	// Set desired bits per pixel

	newSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;	// Specify which aspects of the screen settings we wish to change 

	long result = ChangeDisplaySettings(&newSettings, CDS_FULLSCREEN);	// Attempt to apply the new settings 

	if (result != DISP_CHANGE_SUCCESSFUL)	// If fullscreen failed to change...
		return false;	// Return failed
	else	// Otherwise
	{
		
		DWORD dwstyle = GetWindowLong(_h_wnd, GWL_STYLE);	// Switch off the title bar
		dwstyle &= ~WS_CAPTION;		// Remove the bar from our style
		SetWindowLong(_h_wnd, GWL_STYLE, dwstyle);	// Set our new bar style

		SetWindowPos(_h_wnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);		// Move the window to (0,0)
		InvalidateRect(_h_wnd, 0, true);	// Assign the rectangle ratio to the client area

		return true;	// Return success
	}
}

// A function that destroys our window and all deriving context 
static void DestroyWnd()
{
	RendererMaster::Destroy();	// Destroy the render context
	Editor::Destroy();		// Destroy the editor content
	DestroyWindow(_h_wnd);	// Destroy the window itself
}

// A function that creates a style, instance and class for our window
static bool CreateWnd(LPCSTR title, int width, int height, bool fullscreen)
{
	_vp_width = width;	// Assign width
	_vp_height = height;	// Assign height
	_fullscreen = fullscreen;	// Assign fullscreen bool

	_dw_ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// Set the style attribs for our application
	_h_instance = GetModuleHandle(NULL);	// Initialise the instance handler for our application

	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));	// Set a dark gray colour for our background pane

	_window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Set our class style
	_window_class.lpfnWndProc = WndProc;	// Assign the WndProc function to our class
	_window_class.cbClsExtra = 0;	// No need to allocate extra bytes to our window-class structure
	_window_class.cbWndExtra = 0;	// No need to allocate extra bytes to our window instance
	_window_class.hInstance = _h_instance;	// Assign the hinstance 
	_window_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);	// Set the window icon
	_window_class.hCursor = LoadCursor(NULL, IDC_ARROW);	// Set the window cursor
	_window_class.hbrBackground = (HBRUSH)brush;		// Set the background
	_window_class.lpszMenuName = NULL;	// No extra resources for the class menu
	_window_class.lpszClassName = title;	// Set class name

	if (!RegisterClass(&_window_class))		// If the class fails to register...
		return false;

	_h_wnd = CreateWindowEx(_dw_ex_style,	// Set window style
		title,	// Set class title
		title,	// Set window title
		WS_OVERLAPPEDWINDOW,	// Set DW style
		CW_USEDEFAULT,	// Use default X position
		0,	// Set Y positiont to NULL
		width,	// Assign width
		height,	// Assign height
		NULL,	// No window parent
		NULL,	// No menu
		_h_instance,	// Assign instance
		NULL);	// No need to parse anything to the CREATESTRUCT call 

	if (fullscreen)		// If fullscreen mode is set to true...
		SetFullscreen(width, height);	// Apply fullscreen mode

	_opengl_context.Create(_h_wnd);		// Set the opengl context to our main window

	ShowWindow(_h_wnd, SW_MAXIMIZE);	// Show the window as maximised
	UpdateWindow(_h_wnd);	// Update the window parameters

	_precision_cursor = LoadCursor(NULL, IDC_CROSS);	// Assign cursor type

	return true;	// Return as success
}

#endif