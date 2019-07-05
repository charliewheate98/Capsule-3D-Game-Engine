#ifndef __AL_CONSOLE_H__
#define __AL_CONSOLE_H__

#include "Text.h"	// Get text controls
#include "Commandline.h"	// Get access to the DataIO functions and content
#include "Control.h"

#define MAX_LINES	3	// Maximum line count


// This class handles the console user interface
class ALConsole : public Control
{
private:
	int								_current_line;	// Keep track of current line due to pointer rotation
	glm::vec3						_default_colour;	// A default colour for the console
	ShaderProgram*					_temp_shader_prog;	// Temporary shader program 
	std::vector<Text::TextBlock*>	_lines;	// The text blocks for each line in the console

public:
	// Initial constructor
	inline ALConsole(ShaderProgram *shader_program, glm::vec2 pos)
	{
		type = CT_CUSTOM;	// Define control type
		position = pos;		// Assign position

		_current_line = -1;	// Initialise current line
		_default_colour = glm::vec3(1.0f, 1.0f, 1.0f);
		_temp_shader_prog = shader_program;		// Assign temp shader program

		NewLine();	// Create a new line ready for first command

		Commandline::Data::Initialise();	// Initialise keyword data for commandline
	}

	// Deconstructor
	inline ~ALConsole()
	{
		_lines.clear();		// Delete all text blocks
	}

	// Update the callback events
	inline virtual void Callback(UINT &message, WPARAM &wParam)
	{
		switch (message)		// Iterate through callback events
		{
		case WM_KEYDOWN:	// If a key is pressed
			if (Keyboard::GetKey(VK_TAB).down)		// If TAB is pressed
				active = !active;	// Toggle active on / off

			if (active)		// If the console is active...
			{
				if (GetAsyncKeyState(VK_SPACE))		// Apply syntax colour change after space has been pressed
				{
					std::string	word = _lines[0]->_text_block[_lines[0]->_text_block.size() - 1];	// First extract word from textblock

					if (!HasDuplicates(word))	// If there are no word duplicates on the same line...
						SetSyntax(word, 1);		// Set syntax colour to word IF it is key
				}

				if (GetAsyncKeyState(VK_RETURN))	// If ENTER is pressed
				{
					_lines[0]->_text_block.erase(_lines[0]->_text_block.begin() + 0);	// Remove the first word
					Commandline::Callback(Deferred::shaders[0], _lines[0]->_text_block);	// Compute the callback result
					NewLine();	// Create new line
				}

				else if (Keyboard::GetKey(VK_BACK).down)	// if BACKSPACE is pressed
				{
					bool prev_word = false;		// This is used for reseting text to default colour IF a space and syntax is detected
					if (_lines[0]->_text[_lines[0]->_text.size()-1] == VK_SPACE)	// If the current keycode is a white space...
						prev_word = true;	// Word detected is set to true

					_lines[0]->RemoveLast();	// Remove last key in current textblock	// Delete last char

					if (prev_word && _lines[0]->_text_block[_lines[0]->_text_block.size()-1].size() > 1)	// If a previous word has been detected and the number of words is greater than one...
					{
						std::string	word = _lines[0]->_text_block[_lines[0]->_text_block.size() - 1];	// First extract word from textblock
						if (!HasDuplicates(word))	// If there are no word duplicates on the same line...
							SetSyntax(word, 0);		// Reset syntax back to default colour
					}
				}
				else	// Otherwise
				{
					char c = MapVirtualKey(wParam, MAPVK_VK_TO_CHAR);	// Convert wParam to virtual keycode
					_lines[0]->AppendChar(_temp_shader_prog->GetProgram(), tolower(c));		// Append character
				}
			}
			break;
		}
	}

	// Update the ALConsole
	inline virtual void Update(double &delta) {}

	// Render the ALConsole
	inline virtual void Render()
	{
		if (active)		// If the console is active...
		{
			for (Text::TextBlock* t : _lines)	// Iterate through each line
				t->Render();	// Render the line(s)
		}
	}

private:
	// This function will shift a line up, ready for a new line
	inline void Shift()
	{
		for (unsigned int i = 1; i < _lines.size() + 1; i++)	// Iterate through each line
		{
			_lines[i - 1]->TranslateText(glm::vec2(this->position.x, this->position.y - (i * 20.0f)));	// Move lines up

			if (i > 0)	// If i is greater than zero
				_lines[i - 1]->SetTextOpacity(0.15f);	// Set opacity to old lines
		}
	}

	// Push back a new textblock and delete any that exceed the limited number of lines
	inline void NewLine()
	{
		if (_lines.size() >= MAX_LINES)	// Remove exceeding line
			_lines.pop_back();	// Clear last line

		Shift();	// Shift lines up
		_lines.insert(_lines.begin(), 1, new Text::TextBlock(_temp_shader_prog->GetProgram(), "> ", position, Content::_fonts[0], FT_REGULAR, 1.1f, _default_colour, 0.95f));	// Add new textblock to console
	}

	// This function takes a word, and toggle for choosing syntax keyword colour, or default colour
	inline void SetSyntax(std::string word, uint8_t toggle)
	{
		if (Commandline::GetKeyWordIndex(word) != -1)	// If this word is a key word...
		{
			uint32_t offset = _lines[0]->_text.find(word) - _lines[0]->_num_spaces;	// Get the offset char position in text
			Commandline::Syntax key_word = Commandline::GetKeyWord(word);	// Get key word object from commandline data
			for (unsigned int i = 0; i < word.size(); i++)	// Iterate through each character in word
				_lines[0]->_chars[offset + i]._colour = (toggle == 1 ? key_word.colour : _default_colour);	// Set default colour to each character
		}
	}

	// This function checks for duplicated words in a line of text
	inline bool HasDuplicates(std::string word)
	{
		uint8_t matches = 0;	// The number of duplicate matches
		for (unsigned int i = 0; i < _lines[0]->_text_block.size(); i++)	// Iterate through each word on line
		{
			if (word == _lines[0]->_text_block[i])	// If a similar word has been found...
				matches++;	// Increment the number of matches
			if (matches > 1)	// If a duplicate key word has been detected...
				return true;	// Return true as there is a duplicate
		}

		return false;	// Return false by default
	}
};

#endif