#ifndef _INVENTORY_H_ // starting marco definitions
#define _INVENTORY_H_ // marco definitions

#define NUM_SLOTS 6

// Lib Includes
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <vector>
#include <string>

// Engine Includes
#include "Engine/Uniform.h"
#include "Engine/Rect.h"
#include "GBufferData.h"
#include "BitmapAtlas.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Item.h"

struct Slot
{
	bool isContained;

	unsigned int slotID;

	GLuint _loc_pos;

	glm::vec2 _pos;
	glm::vec2 _dim;

	inline Slot(glm::vec2 pos, glm::vec2 _slot_dims, unsigned int id) :
		_dim(_slot_dims), _pos(pos), isContained(false), slotID(id) {}
	inline ~Slot() {}

	inline void Update(double delta)
	{
		if (Mouse::GetNormalisedX() >= _pos.x - _dim.x &&
			Mouse::GetNormalisedX() <= _pos.x + _dim.x &&
			Mouse::GetNormalisedY() >= _pos.y - _dim.y &&
			Mouse::GetNormalisedY() <= _pos.y + _dim.y)
		{
			isContained = true;
		}
		else
			isContained = false;
	}
};

class Inventory
{
private:
	std::vector<Slot*>        _slots;
	std::vector<unsigned int> _slot_indices;
	std::vector<glm::vec2>    _slots_positions;

	std::vector<Item*>        _items;

	unsigned int _num_slots;
	unsigned int _loc_colour;
	unsigned int _loc_proj;
	unsigned int _loc_instanced;

	glm::vec3 scol;
	glm::mat4 proj;

	Rect* _instance_rect;
public:
	inline Inventory(unsigned int _program, int num_slots)
	{
		_num_slots = num_slots;

		Create(_program);
	}
	~Inventory() 
	{
		_items.clear();
		_slots.clear();

		delete _instance_rect;
	}
	
	inline void Create(unsigned int shader_program)
	{
		_loc_instanced = glGetUniformLocation(shader_program, "instanced");

		for (unsigned int i = 0; i < _num_slots; i++) _slots_positions.push_back(glm::vec2(0.0f + i * 0.1f, 0.0f));
		for (unsigned int i = 0; i < _num_slots; i++)
		{
			_slot_indices.push_back(i);

			std::string	index = std::to_string(i);

			_slots.push_back(new Slot(_slots_positions[i], glm::vec2(0.03f, 0.05f), _slot_indices[i]));
			_slots[i]->_loc_pos = glGetUniformLocation(shader_program, ("elements[" + index + "]._pos").c_str());
		}

		_loc_colour = glGetUniformLocation(shader_program, "_colour");

		addItem(shader_program, "Turret", 0, 0);
		addItem(shader_program, "Gravity Mine", 1, 1);

		_instance_rect = new Rect((double)64.0f, (double)64.0f, 1.0f, true);
	}

	inline void addItem(unsigned int program, const std::string &itemName, unsigned int slot, unsigned int id)
	{
		_items.push_back(new Item(program, itemName, id));
		_items[_items.size() - 1]->SetPosition(glm::vec3(_slots[slot]->_pos.x, _slots[slot]->_pos.y, NULL));
	}

	inline void dropItem(unsigned int index)
	{
		_items.erase(_items.begin() + index);
	}

	inline void Update(double delta) 
	{
		for (unsigned int i = 0; i < _items.size(); i++)
			_items[i]->Update(delta);
	}

	inline void RenderSlots()
	{	
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUniform1i(_loc_instanced, true);

		for (unsigned int i = 0; i < _num_slots; i++) glUniform2f(_slots[i]->_loc_pos, _slots[i]->_pos.x, _slots[i]->_pos.y);

		glUniform3f(_loc_colour, 0.5f, 0.5f, 0.5f);

		_instance_rect->Render(_num_slots);
	}

	inline void RenderItems()
	{
		glUniform1i(_loc_instanced, false);
		glUniform3f(_loc_colour, 1.0f, 1.0f, 1.0f);

		for (unsigned int i = 0; i < _items.size(); i++)
			_items[i]->Render();
	}
};

// End
#endif
