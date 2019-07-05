#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Actor.h"		// Extend from our abstrct class

#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

// This is an abstract class and will store all the nessary data 
// for different types of lights
class Light : public Actor
{
protected:
	glm::vec3 _position;
	glm::vec3 _colour;

	unsigned int _light_type; // type of light

	bool		 _cast_shadow; // can this light cast shadows? 

	float		 _intensity; // brightness of the light
public:
	 // Different light types, static for lightmaps, dynamic for shadowmapping
	inline Light() {} // default constructer to avoid errors
	inline ~Light() {} // destructer

	// point
	unsigned int _u_point_positions[5];
	unsigned int _u_point_colours[5];
	
	// spot
	unsigned int _u_spot_positions[5];
	unsigned int _u_spot_directions[5];
	unsigned int _u_spot_colours[5];
	unsigned int _u_spot_cutoff[5];
	unsigned int _u_spot_outercutoff[5];
	unsigned int _u_spot_shadows[5];

	// getters
	inline glm::vec3    GetLightPosition()  { return _position;  }  // get the position of the light
	inline glm::vec3    GetLightColour()    { return _colour;    }  // get the colour of the light
	inline float        GetLightIntensity() { return _intensity; }  // get light intensity (brightness)
	inline unsigned int GetLightType()      { return _light_type; } // get the light type

	// main method
	inline virtual void Update(double& delta) {} // update light
	inline virtual void Render() {}  // render light
};

#endif
