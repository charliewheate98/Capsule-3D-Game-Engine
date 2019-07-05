#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <iostream>

// A list of object types
enum ObjectTypes {
	OBJECT,
	ACTOR,
	MAP,
	CAMERA,
	LIGHT,
	MESH,
	SKYBOX,
	UI_ELEMENT
};

// This is our abstract object class
class Object
{
private:
	std::string		_name;	// The object's name

protected:
	unsigned int	_t;		// Object type

public:
	// Default constructor - initialise type
	inline Object() : _t(OBJECT), _name("Object") {}

	inline unsigned int GetObjectType() { return _t; }	// A function that returns the object type
	inline std::string GetName() { return _name; }		// Return the name

	inline void SetObjectType(unsigned int value) { _t = value; }	// Return the object type
	inline void SetName(std::string value) { _name = value; }	// Assign our name value
};

#endif