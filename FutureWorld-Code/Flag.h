#ifndef _FLAG_H_
#define _FLAG_H_

#include "Fl/gl.h"

#define _USE_MATH_DEFINES

class Flag {
private:
	GLubyte display_list;   // The display list that does all the work.
	GLuint  pole_text_obj;    // The object for the pole texture.
	GLuint  flag_text_obj;    // The object for the flag texture.
	GLuint  base_text_obj;    // The object for the base texture.
	bool    initialized;    // Whether or not we have been initialised.
	float	flag_rot_theta;		// angle of rotation for flag
	float	pole_rot_theta;		// angle of rotation for pole
	bool	flag_rot_direction;	// direction of rotation flag
	bool	pole_rot_direction;	// direction of rotation pole
	float	flag_shear;			// amount of shear for flag
	bool	flag_shear_direction; // direction of shear for flag
	GLfloat shear[16];

public:
	// Constructor. Can't do initialization here because we are
	// created before the OpenGL context is set up.
	Flag(void) { display_list = 0; initialized = false; };

	// Destructor. Frees the display lists and texture object.
	~Flag(void);

	// Initializer. Creates the display list.
	bool    Initialize(void);

	// Does the drawing.
	void    Draw(void);
};


#endif
