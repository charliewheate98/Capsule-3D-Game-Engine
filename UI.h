#ifndef __UI_H__
#define __UI_H__

#include "Object.h"
#include "ALConsole.h"	// Get ALConsole controls

class Button : public Object
{
private:
	uniform _loc_pos;
	uniform _loc_ui_element;

	bool _isClicked;
			
	Rect* _rect;

	glm::vec2 _pos;
	glm::vec2 _dims;
public:
	enum ELEMENTS
	{
		RECT,
		LABEL
	};

	inline Button(unsigned int shader_program, unsigned int txt_program, const std::string& name, glm::vec2 pos, glm::vec2 dims) :
		_pos(pos),
		_dims(dims),
		_isClicked(false)
	{
		SetObjectType(UI_ELEMENT);
		SetName(name + "_btn");

		_loc_ui_element = glGetUniformLocation(shader_program, "UIelement");
		_loc_pos = glGetUniformLocation(shader_program, "pos");

		_rect = new Rect((double)dims.x, (double)dims.y, 1.0f, true);
	}

	inline ~Button()
	{
		_rect = nullptr;
		delete _rect;
	}

	inline bool IsClicked() { return _isClicked; }

	inline void Update(double delta)
	{
	}

	inline void Render(unsigned int element)
	{
		switch (element)
		{
		case RECT:
			glUniform1i(_loc_ui_element, true);
			glUniform2f(_loc_pos, _pos.x, _pos.y);

			_rect->Render(1);
			break;
		case LABEL:
			break;
		default:
			break;
		}


	}
};

// This class will statically handle all UI elements to the viewport
class UI
{
public:
	static GLuint _loc_ui_element;
	static GLuint _loc_pos;

	static ShaderProgram*			_text_shader;	// For our text rendering
	static ShaderProgram*			_rect_shader;	// For our rectangle rendering

	static Button* _button[8];

	static std::vector<Control*>	_controls;	// The list of UI controls

	// This will initialise all UI components
	inline static void Initialise()
	{
		_text_shader = new ShaderProgram();		// Initialise the text shader program
		_text_shader->AddShaderAttachment("Text.v", GL_VERTEX_SHADER);	// Add a shader attachment
		_text_shader->AddShaderAttachment("Text.f", GL_FRAGMENT_SHADER);	// Add another a shader attachment
		_text_shader->LinkProgram();	// Link attachments and create shader program

		_rect_shader = new ShaderProgram();		// Initialise the text shader program
		_rect_shader->AddShaderAttachment("Rect.v", GL_VERTEX_SHADER);	// Add a shader attachment
		_rect_shader->AddShaderAttachment("Rect.f", GL_FRAGMENT_SHADER);	// Add another a shader attachment
		_rect_shader->LinkProgram();	// Link attachments and create shader program

		_button[0] = new Button(_rect_shader->GetProgram(), _text_shader->GetProgram(), "PlayerStart", glm::vec2(-0.9f, 0.65f), glm::vec2(64.0f, 64.0f));

		Content::_fonts.push_back(new Font(_text_shader->GetProgram(), { "core_r[16].dds" }, { 256, 96 }));		// Import the engine's default font

		_controls.push_back(new ALConsole(_text_shader, glm::vec2(36.0f, 1048.0f)));	// Initialise the ALConsole to the UI system

		_loc_ui_element = glGetUniformLocation(_rect_shader->GetProgram(), "UIelement");
	}

	inline static void Callback(UINT &message, WPARAM &wParam)
	{
		for (Control* c : _controls)	// Iterate through each control
			c->Callback(message, wParam);	// Update the callback
	}


	// Update the UI
	inline static void Update(double &delta)
	{
		for (Control* c : _controls)	// Iterate through each control
		{
			if (c->active)	// If a control is active...
				c->Update(delta);	// Render the text control
		}

		_button[0]->Update(delta);
	}

	// Render the UI
	inline static void Render()
	{
		// Console
		glUniform1i(_loc_ui_element, false);

		_text_shader->UseProgram();		// Bind the text shader

		for (Control* c : _controls)	// Iterate through each control
			c->Render();	// Render the text control

		// OpenGL 
		glDisable(GL_BLEND);

		// Buttons
		_rect_shader->UseProgram();
		_button[0]->Render(_button[0]->RECT);
	}

	// This will destroy all UI components
	inline static void Destroy()
	{
		_controls.clear();	// Delete all controls
		delete _text_shader;	// Delete text shader
	}
};

// Static definitions
ShaderProgram*			UI::_text_shader;
ShaderProgram*			UI::_rect_shader;

std::vector<Control*>	UI::_controls;

GLuint UI::_loc_ui_element;
GLuint UI::_loc_pos;

Button* UI::_button[8];

#endif