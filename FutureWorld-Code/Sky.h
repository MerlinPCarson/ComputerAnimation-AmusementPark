#ifndef _SPHERE_
#define _SPHERE_

#include "Fl/gl.h"

class Sphere {
private:
	GLubyte display_list;   // The display list that does all the work.
	GLuint  texture_obj;    // The object for the grass texture.
	bool    initialized;    // Whether or not we have been initialised.
	float	rot_theta;		// angle of rotation for sphere
	float	rev_theta;		// angle of revolution for sphere
	unsigned int chance;	// place holder for random number

public:
	// Constructor. Can't do initialization here because we are
	// created before the OpenGL context is set up.
	Sphere(void) { display_list = 0; initialized = false; };

	// Destructor. Frees the display lists and texture object.
	~Sphere(void);

	// Initializer. Creates the display list.
	bool    Initialize(void);

	// Does the drawing.
	void    Draw(void);
};


#endif
