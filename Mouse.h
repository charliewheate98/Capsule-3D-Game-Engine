#ifndef __MOUSE_H__
#define __MOUSE_H__

#include <Windows.h>


// Our mouse class for point position and buttons
class Mouse
{
private:
	static POINT _p;	// Our point variable for encapsulating the cursor position
	static POINT _old_p;	// Our old point variable for scalar calculation
	static double _x_offset;		// The offset x value for positioning the 
	static double _y_offset;		// The offset y value for positioning the camera view
	static double _last_x;		// The last offset x value for positioning the camera view
	static double _last_y;		// The last offset y value for positioning the camera view
	static bool _first_mouse;	// To check if the mouse has been moved for the first time
	static bool _left_click;	// To check if left mouse button clicked
	static bool _right_click;	// To check if left mouse button clicked
	static bool _middle_click;	// To check if left mouse button clicked
	static bool _toggle_clicked;	// Check a toggle click on / off
	static double _click_drag_len;	// The scalar of drag from mouse click

public:
	// Update the mouse position
	inline static void UpdateMousePosition()
	{
		if (GetCursorPos(&_p)) {}	// Get the cursor position and assign the point values to _p
	}

	// Get the POINT value of the mouse position
	inline static POINT GetPoint()
	{
		return _p;	// Return the POINT variable
	}

	// Get the old POINT value of the mouse position
	inline static POINT GetOldPoint()
	{
		return _old_p;	// Return the old POINT variable
	}

	// Get the X value of POINT _p
	inline static double GetPointX()
	{
		return (int)_p.x;	// Return X position
	}

	// Get the Y value of POINT _p
	inline static double GetPointY()
	{
		return (int)_p.y;	// Return Y position
	}

	// Get the X value of POINT _old_p
	inline static double GetOldPointX()
	{
		return (int)_old_p.x;	// Return X position
	}

	// Get the Y value of POINT _old_p
	inline static double GetOldPointY()
	{
		return (int)_old_p.y;	// Return Y position
	}

	// Get the X offset value of X
	inline static double GetOffsetX()
	{
		return _x_offset;	// Return X offset value
	}

	// Get the X offset value of X
	inline static double GetOffsetY()
	{
		return _y_offset;	// Return Y offset value
	}

	// Get the last X offset value of X
	inline static double GetLastX()
	{
		return _last_x;		// Return last X offset value of X
	}

	// Get the last Y offset value of Y
	inline static double GetLastY()
	{
		return _last_y;		// Return last Y offset value of Y
	}

	// Get the normalised X position
	inline static double GetNormalisedX()
	{
		return ((_last_x / 1920.0f) * 2) - 1;	// Normalise x mouse position
	}

	// Get the normalised Y position
	inline static double GetNormalisedY()
	{
		return -(((_last_y / 1080.0f) * 2) - 1);	// Normalise y mouse position
	}

	// Get the click drag scalar value
	inline static double GetClickDragLength()
	{
		return _click_drag_len;		// Return the click drag value
	}

	// Get the first mouse bool
	inline static bool IsFirstMouse()
	{
		return _first_mouse;	// Return the first mouse state
	}

	// Get the left mouse click bool
	inline static bool &IsLeftClick()
	{
		return _left_click;		// Return the left click bool
	}

	// Get the right mouse click bool
	inline static bool IsRightClick()
	{
		return _right_click;		// Return the right click bool
	}

	// Get the middle mouse click bool
	inline static bool IsMiddleClick()
	{
		return _middle_click;		// Return the middle click bool
	}

	// Get the toggle mouse click bool
	inline static bool IsToggleClick()
	{
		return _toggle_clicked;		// Return the toggle mouse click bool
	}

	// Set the old point x value
	inline static void SetOldPosX(double value)
	{
		_old_p.x = (int)value;	// Set X old pos value
	}

	// Set the old point y value
	inline static void SetOldPosY(double value)
	{
		_old_p.y = (int)value;	// Set Y old pos value
	}

	// Set the old point value
	inline static void SetOldPos(double x, double y)
	{
		SetOldPosX(x);	// Set old x pos
		SetOldPosY(y);	// Set old y pos
	}

	// Set the X offset value of X
	inline static void SetOffsetX(double value)
	{
		_x_offset = value;	// Set X offset value
	}

	// Set the X offset value of X
	inline static void SetOffsetY(double value)
	{
		_y_offset = value;	// Set Y offset value
	}

	// Set the last X offset value of X
	inline static void SetLastX(double value)
	{
		_last_x = value;		// Set last X offset value of X
	}

	// Set the last Y offset value of Y
	inline static void SetLastY(double value)
	{
		_last_y = value;		// Set last Y offset value of Y
	}

	// Set the click drag scalar
	inline static void SetClickDragLength(double value)
	{
		_click_drag_len = value;	// Set the click drag scalar result to value
	}

	// Set the first mouse bool
	inline static void SetFirstMouse(bool value)
	{
		_first_mouse = value;	// Set the first mouse state
	}

	// Set the left mouse click bool
	inline static void SetLeftClick(bool value)
	{
		_left_click = value;	// Set left click to value
	}

	// Set the right mouse click bool
	inline static void SetRightClick(bool value)
	{
		_right_click = value;	// Set right click to value
	}

	// Set the middle mouse click bool
	inline static void SetMiddleClick(bool value)
	{
		_middle_click = value;	// Set middle click to value
	}

	// Set the toggle mouse click bool
	inline static void SetToggleClick(bool value)
	{
		_toggle_clicked = value;	// Set middle click to value
	}

	// Set cursor pos
	inline static void SetCursorPosition(double x, double y)
	{
		SetCursorPos((int)x, (int)y);	// Center our mouse position

		SetLastX(x);		// Initialise our last mouse x position
		SetLastY(y);		// Initialise our last mouse y position
	}
};

// Define our static variables
POINT Mouse::_p;
POINT Mouse::_old_p;
double Mouse::_x_offset = 0.0f;
double Mouse::_y_offset = 0.0f;
double Mouse::_last_x = 0.0f;
double Mouse::_last_y = 0.0f;
double Mouse::_click_drag_len = -0;
bool Mouse::_first_mouse = true;
bool Mouse::_left_click = false;	
bool Mouse::_right_click = false;
bool Mouse::_middle_click = false;
bool Mouse::_toggle_clicked = false;

#endif