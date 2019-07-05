#ifndef __EDIT_TRANSFORM_H__
#define __EDIT_TRANSFORM_H__

#include <glm/glm.hpp>	// Get glm variables

// This namespace will edit an object's transformation in edit mode
namespace EditTransform
{
	// This function will pan the camera in respect to the mouse motion
	inline static void PanCamera(bool mouse_left, bool mouse_right, double mouse_x, double mouse_y, glm::vec3 &position)
	{
		if (Mouse::IsLeftClick())
		{
			//std::cout << mouse_x << " " << glm::normalize(mouse_y) << "\n";

		}
		else	// Otherwise if nothing is clicked
		{

		}
		
		
		
	}
}

#endif