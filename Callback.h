#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "Globals.h"
#include "RendererMaster.h"	// Set callbacks to input events
#include "resource.h"
#include <crtdbg.h>

HWND	_h_wnd;			// The main window identifier
HWND	_child_h_wnd;		// The child window identifier
HCURSOR _precision_cursor;	// The window cursor

// The main WndProc for hadnling message callbacks
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)		// Standard window callback
{
	switch (message)	// Iterate through native callback events
	{
	case WM_CREATE:
	{
		HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		_ASSERTE(hIcon != 0);
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		return 0;
	}
	case WM_SIZE:	// If our window is resizing
	{
		_opengl_context.Reshape(_vp_width, _vp_height);	// Assign the width and height to our OepnGL context
		break;
	}

	case WM_SETCURSOR:
	{
		break;
	}

	case WM_DESTROY:	// If our window is being destroyed
	{
		PostQuitMessage(0);		// Quit the application
		break;
	}

	case WM_SETFOCUS:	// If our window is being focused
	{
		_focused = true;	// Set focus to true
		break;
	}

	case WM_KILLFOCUS:	// If our window is being not being focused
	{
		_focused = false;	// Set focus to false
		break;
	}
	}

	Editor::Callback(message, wParam);	// Update our Editor component callbacks

	return DefWindowProc(hWnd, message, wParam, lParam);	// Return the defined WndProc
}

#endif