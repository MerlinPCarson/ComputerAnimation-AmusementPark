#ifndef _LIGHT_
#define _LIGHT_

#define _USE_MATH_DEFINES

#include "Fl/gl.h"

#ifndef __COLOR_LIST__
#define __COLOR_LIST__
enum ColorList {Orange, Red, Yellow, Purple};

struct Color{
	float red;
	float blue;
	float green;
	ColorList color;
};
#endif

class Light {

private:
	GLubyte display_list;   // The display list that does all the work.
	GLuint  light_text_obj;    // The object for the light texture.
	GLuint  stand_text_obj;    // The object for the light texture.
	bool    initialized;    // Whether or not we have been initialised.
	bool	direction;		// direction of color cycle
	float	distance;		// distance from center	of world

	float	standRadius;	// size of light
	float	lightRadius;	// diameter of cylinder
	float	height;			// height of light/cylinder
	int		numSlices;		
	int		numStacks;
	float	localTheta;		// location of light 
	
	Color	currColor;		// current color of the light
	int		colorDelay;		// delay between changing colors
	int		colorTimer;		// elapsed time since last color change
	float	rotSpeed;		// speed of rotation
	float	rotTheta;

	float	lightLoc[3];			// x, y, z coors for translation
	float	standLoc[3];			// x, y, z coors for translation




public:
	// Constructor. Can't do initialization here because we are
	// created before the OpenGL context is set up.
	Light(void) { display_list = 0; initialized = false; };

	Light(double distance, double lightRadius, double standRadius, double theta, double height, double rotSpeed, ColorList color);
	
	// Destructor. Frees the display lists and texture object.
	~Light(void);

	// Initializer. Creates the display list.
	bool    Initialize(void);

	void next_color();
	void change_lights(int button);


	// Does the drawing.
	void    Draw(void);
};


#endif
