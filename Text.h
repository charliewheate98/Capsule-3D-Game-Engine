#ifndef __TEXT_H__
#define __TEXT_H__

#include <glm\gtc\type_ptr.hpp>		// Get valur_ptr for glm
#include "Control.h"	// Include control for deriving
#include "Font.h"	// Include font struct
#include "Keys.h"	// Get key macro definitions


// A namespace of all text classes
namespace Text
{
	// This function will convert a char index to a normalised vec2 coord
	inline glm::vec2 IndexToCoord(size_t atlas_w, size_t atlas_h, unsigned int i)
	{
		unsigned int i_opt(i - ASCII_OFFSET);	// Optimise the new ascii char table

		size_t length_x = atlas_w / BITMAP_X_COUNT;	// Calculate num chars on each column by dividing width by element size
		size_t length_y = atlas_h / BITMAP_X_COUNT;	// Calculate num chars on each row by dividing height by element size

		double x = i_opt % length_x;	// Get the x index value by calculating modulo offset
		double y = floor(i_opt / length_x);	// Round (i / a_w) down to get num rows

		return glm::vec2(x / length_x, y / length_y);	// Return Normliased x and y for coords
	}

	// This structure will contain all of the basic data for each character
	struct CharData
	{
		float		_kern;	// It's unique spacing by average
		uint32_t	_key_code;	// ASCII key code
		glm::vec2	_position;	// In window-space coords

		// Initial constructor
		inline CharData(uint32_t key_code)
		{
			_key_code = key_code;	// Assign key code
			_kern = 0.0f;	// Initialise default kern
			_position = glm::vec2(0.0f);
		}

		// This function will return a char data struct depending on the key code
		static CharData GetChar(std::vector<CharData> cda, uint32_t kc)
		{
			for (unsigned int i = 0; i < cda.size(); i++)	// Iterate through each character data struct
				if (cda[i]._key_code == kc)	// If the key code matches...
					return cda[i];	// Return this char data object
		}
	};

	// This will store the data for each character
	struct Character
	{
		float		_style;		// The family type (regular, bold ect...)
		float		_opacity;	// Opacity for alpha
		uniform		_u_style;	// Uniform for style
		uniform		_u_offset;	// Uniform for position offset
		uniform		_u_colour;	// Uniform for colour
		uniform		_u_coord;	// Uniform for coordinate in atlas
		glm::vec2	_offset;	// The char offset position in relation to the origin
		glm::vec2	_coord;		// The coordinate in atlas
		glm::vec3	_colour;	// The colour of the character

		// Default constructor
		inline Character() : _offset(glm::vec2(0.0f)), _coord(glm::vec2(0.0f)), _colour(glm::vec3(1.0f)), _opacity(1.0f) {}

		// Initial constructor
		inline Character(uniform shader_program, uint32_t uniform_array_index, glm::vec2 offset, glm::vec2 coord, glm::vec3 colour, float opacity, float style)
		{
			_style = style;		// Assign the style
			_opacity = opacity;		// Assign opacity
			_colour = colour;	// Assign the colour
			_offset = offset;	// Default offset positionchar_spaces
			_coord = coord;		// Default coord

			std::string index = std::to_string(uniform_array_index);	// Convert i to string

			_u_style = glGetUniformLocation(shader_program, ("style[" + index + "]").c_str());	// Get style uniform
			_u_offset = glGetUniformLocation(shader_program, ("offset[" + index + "]").c_str());	// Get offset uniform
			_u_coord = glGetUniformLocation(shader_program, ("coord[" + index + "]").c_str());	// Get coord uniform
			_u_colour = glGetUniformLocation(shader_program, ("colour[" + index + "]").c_str());	// Get colour uniform
		}
	};

	// This class will be used for rendering a line of text
	class TextBlock : public Control
	{
	public:
		float						_spacing;	// The spacing between each character
		float						_char_width;	// The width of each char quad
		float						_current_opacity;	// The current opacity 
		uint16_t					_size;	// The text size
		uint16_t					_current_style;		// The current font style
		uint32_t					_num_spaces;	// For keeping track of length
		glm::vec2					_marker_position;	// The marker position
		glm::vec3					_current_colour;	// Colour brush for each character
		std::string					_text;	// Store each character in container
		std::vector<std::string>	_text_block;	// Store each word into seperate containers
		std::vector<CharData>		_char_data;		// A list of character data
		std::vector<Character>		_chars;		// List of characters
		Font*						_font;	// The current font in memory

		// Default constructor
		inline TextBlock() { type = CT_TEXT; active = true; }	// Assign control type

		// Initial constructor
		inline TextBlock(uniform shader_program, std::string text, glm::vec2 pos, Font* font, uint16_t style, float spacing, glm::vec3 colour, float opacity)
		{
			active = true;	// Automatically set active
			type = CT_TEXT;		// Set control type

			Create(shader_program, text, pos, font, style, spacing, colour, opacity);	// Initialise the textblock
		}

		// This function returns the size of the textblock
		inline glm::vec2 GetSize()
		{
			float spacing(0.0f);	// Temp advance kerning variable
			for (unsigned int i = 0; i < _char_data.size(); i++)
			{
				if (_char_data[i]._key_code != K_SPACE)
					spacing = +_char_data[i]._kern;
				else
					spacing = +((0.75f) * _char_width) * _spacing;
			}

			float x = (_char_data.size() * spacing) + (_char_width * 2.0f);	// Calculate length of textblock
			float y = ((float)_size / (float)_vp_height);		// We only need the size of font for y

			return glm::vec2(x, y);		// Return result
		}

		// This function will translate all character offsets with control
		inline void TranslateText(glm::vec2 pos)
		{
			glm::vec2 old_pos = position;	// Get the old position

			TranslateControl(pos);	// Translate control first

			for (unsigned int i = 0; i < _chars.size(); i++)	// Iterate through each char
			{
				glm::vec2 old_offset = _chars[i]._offset - old_pos;		// Localise the offsets
				_chars[i]._offset = position + old_offset;	// Re-translate each character's offset
			}
		}

		// This function sets the opacity value to the entire textblock
		inline void SetTextOpacity(float value)
		{
			_current_opacity = value;	// Assign current opacity to value

			for (unsigned int i = 0; i < _chars.size(); i++)	// Iterate through each char
				_chars[i]._opacity = value;		// Assign new opacity value
		}

		// This function creates the textblock
		inline void Create(uniform shader_program, std::string text, glm::vec2 pos, Font* font, uint16_t style, float spacing, glm::vec3 colour, float opacity)
		{
			TranslateControl(pos);		// Convert window space to NDC
			_marker_position = position;	// Assign optimised translation

			_text = "";	// Initialise text
			_num_spaces = 0;	// Set default space count
			_text_block.push_back("");	// Create first string container
			_font = font;	// Assign font
			_spacing = spacing;		// Assign type spacing
			_current_style = style;		// Assign font stlye
			_current_opacity = opacity;		// Assign opacity
			_current_colour = colour;	// Assign type colour

			if (_font)	// If the font is valid...
			{
				_size = font->_size;	// Assign size of text
				_char_width = (_size / (float)_vp_width) * 2;	// Calc the width of char rect

				AppendText(shader_program, text);	// Append each character to textblock
			}
			else
				std::cout << "Error: Font is null!\n";	// Print out error message as font is not initialised

			GLfloat aspect = (GLfloat)_font->_bitmaps[0]->_height / (GLfloat)_font->_bitmaps[0]->_width;	// Calculate the aspect for atlas map

			rect = new Rect(_size, _size, aspect, true);	// Initialise rectangles
		}

		// This function will extract and store the key code data and append a character to the textblock
		inline void AppendChar(uniform shader_program, uint32_t k)
		{
			if (k < 32 || k > 126)	// If the keycode is NOT a physical symbol
				return;		// Return and don't bother setting up new char

			ExtractCharData(k);		// Extract key data and store to container
			Append(shader_program, _char_data[_char_data.size() - 1], _text.size() <= 0 ? 0 : _char_data[_char_data.size() - 2]);	// Append as first char
			_text.push_back(k);		// Push back character to text container
		}

		// This function will push back an array of chars into textblock
		inline void AppendText(uniform shader_program, std::string text)
		{
			for (unsigned int i = 0; i < text.size(); i++)	// Iterate through each character in string
				AppendChar(shader_program, text[i]);	// Append each character to textblock
		}

		// Delete the last character in textblock
		inline void RemoveLast()
		{
			if (_char_data.size() >= 3)	// Cap the minimal number of chars to two for console
			{
				if (_char_data.size() == 3)		// If this is the first char in console
				{
					_marker_position.x -= ((_char_data[_char_data.size() - 1]._kern) * _char_width / 2.0f) * _spacing;	// Increment marker by one multiplied by spacing, multiplied by size
					_chars.pop_back();	// Remove the last physical character
					_text_block[_text_block.size() - 1].pop_back();		// Remove single char from textblock container
				}
				else if (_char_data[_char_data.size() - 1]._key_code == K_SPACE && _char_data.size() != 3)	// If the previous keycode is a white space...
				{
					_marker_position.x -= ((0.4f + _char_data[_char_data.size() - 2]._kern / 2.0f) * _char_width) * _spacing;	// Decrement marker by 0.4, multiplied by spacing, multiplied by size
					_text_block.pop_back();		// Remove empty text block container
					_num_spaces--;	// Decrement the number of white spaces
				}
				else	// Otherwise, if the previous key is NOT a white space...
				{
					_marker_position.x -= ((_char_data[_char_data.size() - 1]._kern + _char_data[_char_data.size() - 2]._kern / 2.0f) * _char_width / 2.0f) * _spacing;	// Increment marker by one multiplied by spacing, multiplied by size
					_chars.pop_back();	// Remove the last physical character
					_text_block[_text_block.size() - 1].pop_back();		// Remove single char from textblock container
				}

				
				_text.pop_back();	// Remove last character from string
				_char_data.pop_back();	// Finally, remove temp backspace char data from container
			}
		}

		// Update the callback events
		inline virtual void Callback(UINT &message, WPARAM &wParam) {}

		// Update the text
		inline virtual void Update() {}

		// Render the text
		inline virtual void Render()
		{
			for (unsigned int i = 0; i < _chars.size(); i++)	// Iterate through each character
			{
				_font->Bind(_chars[i]._style);	// Bind the font

				glUniform1f(_chars[i]._u_style, _chars[i]._style);	// Bind each character's style
				glUniform2fv(_chars[i]._u_offset, 1, glm::value_ptr(_chars[i]._offset));	// Bind each character's offset
				glUniform2fv(_chars[i]._u_coord, 1, glm::value_ptr(_chars[i]._coord));	// Bind each character's coord
				glUniform4fv(_chars[i]._u_colour, 1, glm::value_ptr(glm::vec4(_chars[i]._colour, _chars[i]._opacity)));	// Bind each character's colour
			}

			rect->Render(_chars.size());	// Render rectangles
		}

	private:
		// Append another character to text block
		inline void Append(uniform shader_program, CharData key, CharData last_key)
		{
			glm::vec2 coord = IndexToCoord((size_t)_font->_bitmaps[FT_REGULAR]->_width, (size_t)_font->_bitmaps[FT_REGULAR]->_height, key._key_code);	// Pre-calculate the atlas coord for this key code

			if (last_key._key_code == 0 || last_key._key_code == K_SPACE)	// Check if this is first char in textblock or after space
			{
				_marker_position.x += ((0.4f + key._kern / 2.0f) * _char_width / 2.0f) * _spacing;	// Increment marker by one multiplied by spacing, multiplied by size
				_chars.push_back(Character(shader_program, _chars.size(), glm::vec2(_marker_position.x, position.y), coord, _current_colour, _current_opacity, _current_style));	// Add a character with extra position offset
				_text_block[_text_block.size() - 1].push_back(key._key_code);	// Add new char to the current word

				return;		// Push back first char in textblock and return
			}

			else if (key._key_code != K_SPACE)	// If this is not a white space...
			{
				_marker_position.x += ((key._kern + last_key._kern / 2.0f) * _char_width / 2.0f) * _spacing;	// Increment marker by one multiplied by spacing, multiplied by size
				_chars.push_back(Character(shader_program, _chars.size(), glm::vec2(_marker_position.x, position.y), coord, _current_colour, _current_opacity, _current_style));	// Add a character with extra position offset
				_text_block[_text_block.size() - 1].push_back(key._key_code);	// Add new char to the current word
			}
			else	// Otherwise...
			{
				_marker_position.x += ((0.4f + last_key._kern / 2.0f) * _char_width) * _spacing;	// Increment marker by one multiplied by spacing, multiplied by size
				_text_block.push_back("");	// Add a new word to textblock

				_num_spaces++;	// Increment the space count
			}
		}

		// This function will extract ascii data from each character and store into a container
		inline void ExtractCharData(uint32_t k)
		{
			uint32_t a(0);	// Create temp advance value for character kern
			CharData c(k);		// Create temp char data variable

			// ------------------------------------------ FUNCTIONS ------------------------------------------ <
			if (k == K_SPACE)	// If a space or backspace has been found...
			{
				_char_data.push_back(c);	// Add a new char data object to container
				return;		// Return function and don't add char
			}

			// ------------------------------------------ SYMBOLS ------------------------------------------ <
			else if (k == K_l ||	// 6 pixels
				k == K_i ||
				k == K_APOSTROPHE ||
				k == K_COMMA ||
				k == K_FULLSTOP ||
				k == K_COLON ||
				k == K_SEMICOLON ||
				k == K_PUSH ||
				k == K_IN_CURLY ||
				k == K_OR ||
				k == K_OUT_CURLY ||
				k == K_EXCLAMATION)
				a = 6;	// Set advance value

			else if (k == K_j ||	// 8 pixels	
				k == K_IN_BRACKET ||
				k == K_OUT_BRACKET ||
				k == K_ASTERIX ||
				k == K_DASH ||
				k == K_I ||
				k == K_J)
				a = 8;	// Set advance value

			else if (k == K_c ||	// 10 pixels
				k == K_f ||
				k == K_r ||
				k == K_s ||
				k == K_t ||
				k == K_z ||
				k == K_DOLLAR ||
				k == K_SLASH ||
				k == K_QUESTION ||
				k == K_F ||
				k == K_L ||
				k == K_IN_ARRAY ||
				k == K_BACKSLASH ||
				k == K_OUT_ARRAY ||
				k == K_QUOTE)
				a = 10;	// Set advance value

			else if (k == K_a ||	// 12 pixels
				k == K_b ||
				k == K_d ||
				k == K_e ||
				k == K_g ||
				k == K_h ||
				k == K_k ||
				k == K_n ||
				k == K_o ||
				k == K_p ||
				k == K_q ||
				k == K_u ||
				k == K_v ||
				k == K_x ||
				k == K_y ||
				k == K_GREATER ||
				k == K_LESS ||
				k == K_HASH ||
				k == K_PLUS ||
				k == K_0 ||
				k == K_1 ||
				k == K_2 ||
				k == K_3 ||
				k == K_5 ||
				k == K_6 ||
				k == K_7 ||
				k == K_8 ||
				k == K_9 ||
				k == K_EQUAL ||
				k == K_A ||
				k == K_B ||
				k == K_E ||
				k == K_K ||
				k == K_P ||
				k == K_R ||
				k == K_S ||
				k == K_T ||
				k == K_CAP ||
				k == K_UNDERSCORE)
				a = 12;	// Set advance value

			else if (k == K_4 ||	// 14 pixels
				k == K_C ||
				k == K_D ||
				k == K_G ||
				k == K_H ||
				k == K_N ||
				k == K_U ||
				k == K_V ||
				k == K_X ||
				k == K_Y ||
				k == K_Z ||
				k == K_TILDE)
				a = 14;		// Set advance value

			else if (k == K_m ||	// 16 pixels
				k == K_w ||
				k == K_PERCENT ||
				k == K_AND ||
				k == K_AT ||
				k == K_M ||
				k == K_O ||
				k == K_Q ||
				k == K_W)
				a = 16;		// Set advance value


			c._kern = static_cast<float>(a) / static_cast<float>(_size);		// Normalise advance spacing
			_char_data.push_back(c);	// Add a new char data object to container
		}
	};
};

#endif