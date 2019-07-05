#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <glm\glm.hpp>	// Get glm vars
#include "Keyboard.h"	// For checking if control is active
#include "Mouse.h"	// For checking if control is active
#include "Rect.h"	// Get the rect class for instancing

#define CT_TEXT		0
#define CT_PANE		1
#define CT_BUTTON	2
#define CT_CUSTOM	3


// Abstract class for UI constrols
class Control
{
protected:
	Rect* rect;		// Each control will have a rectangle if need be

public:
	bool		active;		// Check if the control is active
	float		opacity;	// Opacity of control
	uint16_t	type;	// The type of control
	glm::vec2	position;		// The native position of the control

	inline Control() : position(glm::vec2(0.0f)), active(false) {}		// Default constructor
	inline ~Control() { if (rect) delete rect; }	// Delete the rectangle if it's been initialised

	inline void SetOpacity(float value) { opacity = value; }	// Set opacity of control

	inline virtual void Callback(UINT &message, WPARAM &wParam) {}
	inline virtual void Update(double &delta) {}	// Virtual function to update control
	inline virtual void Render() {}		// Virtual function to render control

protected:
	// Translate the control from WSC to NDC
	inline void TranslateControl(glm::vec2 pos)
	{
		position.x = (pos.x / (float)_vp_width * 2.0f) - 1.0f;
		position.y = -((pos.y / ((float)_vp_height) * 2.0f) - 1.0f);
	}
};

#endif