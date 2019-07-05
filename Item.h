// marco definitions 
#ifndef __ITEM_H__
#define __ITEM_H__

// Internal Classes
#include "Actor.h"
#include "Rect.h"

// the Item class handles a single item within the inventory
class Item : public Actor
{
private:
	unsigned int _loc_pos;

	glm::vec2	 _dims;
	Rect*		 _rect;
public:
	bool		 _isActive;
	unsigned int _id;
	inline Item(unsigned int shader_program, const std::string &item, unsigned int id) :
		_id(id), 
		_isActive(false)
	{
		Create(shader_program, item, id);
	}

	inline ~Item()
	{
		delete _rect;
	}

	inline void Create(unsigned int shader_program, const std::string &item, unsigned int id)
	{
		// load in the uniforms
		_loc_pos = glGetUniformLocation(shader_program, "item_pos");

		// set the name of the item
		SetName(item);

		// set the dimensions of the item
		_dims = glm::vec2(0.5f);

		// initalise the items quad
		_rect = new Rect((double)32.f, (double)32.f, 1.0f, true);
	}

	inline virtual void Update(double& delta) 
	{
		if (Mouse::GetNormalisedX() >= _trans._pos.x - _dims.x &&
			Mouse::GetNormalisedX() <= _trans._pos.x + _dims.x &&
			Mouse::GetNormalisedY() >= _trans._pos.y - _dims.y &&
			Mouse::GetNormalisedY() <= _trans._pos.y + _dims.y && 
			Mouse::IsLeftClick())
		{
			_isActive = true;
		}	
		else
			_isActive = false;
	}
	inline virtual void Render() 
	{
		glUniform2f(_loc_pos, _trans._pos.x, _trans._pos.y);

		_rect->Render(1);
	}
};

#endif