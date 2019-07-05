#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__


// Define a Key structure
typedef struct {
	unsigned int id;
	char sym;
	bool down;
} Key;

// The keyboard class will handle each key code and key state
class Keyboard
{
private:
	static Key				_keys[256];	// Make an array of keycodes
	static unsigned int		_buffer_size;	// The amount of keys being iterated

public:
	// Reset key codes for default ASCII format
	inline static void ResetKeyCodes()
	{
		for (unsigned int i = 0; i < _buffer_size; i++)	// Initialise each key code
			_keys[i] = Key({ i, (char)i, false });	// Assign each keycode to the incrementing index
	}

	// Unbind a specific keycode
	inline static void UnbindKey(unsigned int keycode)
	{
		_keys[keycode] = Key({ 0, 0, false });	// Set this keycode to NULL
	}

	// Bind a specific keycode and unbind the opposing value
	inline static void BindKey(unsigned int keycode, unsigned int value)
	{
		_keys[keycode] = _keys[value];	// Assign the current keycode to the new one
		UnbindKey(value);	// Unbind the value of the old keycode
	}

	// Set up our keys via the ASCII format
	inline static void Initialise()
	{
		ResetKeyCodes();	// Set the default keycodes
	}

	// Set any keycode currently down via wParam
	inline static void SetKeyDown(unsigned int keycode)
	{
		for (unsigned int i = 0; i < 255; i++)	// Iterate through our _keys array...
		{
			if (keycode == _keys[i].id)	// If the character matches a keycode...
				_keys[i].down = true;		// Set this key press as true
		}
	}

	// Set any keycode currently up via wParam
	inline static void SetKeyUp(unsigned int keycode)
	{
		for (unsigned int i = 0; i < 255; i++)	// Iterate through our _keys array...
		{
			if (keycode == _keys[i].id)	// If the character matches a keycode...
				_keys[i].down = false;		// Set this key press as false
		}
	}

	// Return a key in respect to the key sym
	inline static Key GetKey(unsigned int keycode)
	{
		return _keys[keycode];	// Return the specific key element
	}
};

Key				Keyboard::_keys[256];		// Define our static variable
unsigned int	Keyboard::_buffer_size = 128;	// Define a default buffer size

#endif