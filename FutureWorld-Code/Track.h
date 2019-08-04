/*
 * Track.h: Header file for a class that draws the train and its track.
 *
 * (c) 2001: Stephen Chenney, University of Wisconsin at Madison.
 */


#ifndef _TRAINTRACK_H_
#define _TRAINTRACK_H_

#include "Fl/gl.h"
#include "CubicBspline.h"
#include "libtarga.h"
#include <GL/glu.h>

#ifndef __COLOR_LIST__
#define __COLOR_LIST__
enum ColorList { Orange, Red, Yellow, Purple };

struct Vector{
	float x;
	float y;
	float z;
	float rotAngle1;
	float rotAngle2;
};

struct Color{
	float red;
	float blue;
	float green;
	ColorList color;
};
#endif

class Track {
  private:
    GLubyte 	    track_list;	    // The display list for the track.
	GLubyte 	    light_list;	    // The display list for the track.
    GLubyte 	    train_list;	    // The display list for the train.
	GLubyte			sign_list;
	GLubyte			sign_list2;
	GLubyte 	    rod_list;	    // The display list for the train.
	GLuint			texture_obj;    // The object for the train texture.
	GLuint			texture_obj2;    // The object for the train texture.
	GLuint			texture_obj3;    // The object for the train texture.
	ubyte			*texture_data;
	int				texture_height, texture_width;

	ubyte			*image_data;
	ubyte			*image_data2;
	int				image_height, image_width;

    bool    	    initialized;    // Whether or not we have been initialized.
    //CubicBspline    *track;	    // The spline that defines the track.
    //float	    posn_on_track;  // The train's parametric position on the
				    // track.
    float	    speed;	    // The train's speed, in world coordinates
	float tangent[3];

    static const int	TRACK_NUM_CONTROLS;	// Constants about the track.
    static const float 	TRACK_CONTROLS[][3];
    static const float 	TRAIN_ENERGY;

	Vector		cartPos;

	int		    n_refined;
	CubicBspline *   refined;

	Color currColor;
	unsigned int		colorIdx;
	bool	direction;
	int		colorDelay;		// delay between changing colors
	int		colorTimer;		// elapsed time since last color change
	float	rot_theta;
	float	rot_theta2;
	float	rot_theta3;

  public:
    // Constructor
    Track(void) { initialized = false; posn_on_track = 0.0f; speed = 0.0f; };

    // Destructor
    ~Track(void);

    bool    Initialize(void);	// Gets everything set up for drawing.
    void    Update(float);	// Updates the location of the train
    void    Draw(void);		// Draws everything.

	Vector	getLocation();

	void TorusPoint(int,int);

	void next_color();
	void get_color(int colorIdx);
	CubicBspline    *track;	    // The spline that defines the track.
	float	    posn_on_track;  // The train's parametric position on the
};


#endif

