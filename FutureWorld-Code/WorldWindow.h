/*
 * CS559 Maze Project
 *
 * Class header file for the WorldWindow class. The WorldWindow is
 * the window in which the viewer's view of the world is displayed.
 *
 * (c) Stephen Chenney, University of Wisconsin at Madison, 2001-2002
 *
 */

#ifndef _WORLDWINDOW_H_
#define _WORLDWINDOW_H_

#include "Fl/Fl.h"
#include "Fl/Fl_Gl_Window.h"
#include "Ground.h"
#include "Track.h"
#include "Flag.h"
#include "Sky.h"
#include "Light.h"

struct Camera{
	float x;
	float y;
	float z;
	float anglePos;
	float angleTilt;
};


// Subclass the Fl_Gl_Window because we want to draw OpenGL in here.
class WorldWindow : public Fl_Gl_Window {
    public:
	// Constructor takes window position and dimensions, the title.
	WorldWindow(int x, int y, int w, int h, char *label);

	// Destructor
	~WorldWindow();

	// init light array
	void init_lights(); 

	void change_lighting(int button);

	// draw() method invoked whenever the view changes or the window
	// otherwise needs to be redrawn.
	void	draw(void);

	// Event handling method. Uses only mouse events.
	int	handle(int);

	// Update the world according to any events that have happened since
	// the last time this method was called.
	bool	Update(float);

	void setCartLocation(float x, float y, float z, float anglePos, float angleTilt);

    private:
	GLubyte		display_list;   // The display list that does all the work.
	unsigned int numStars;		// Number of randomly generated stars
	Ground	    ground;			// The ground object.
	Track		traintrack;	    // The train and track.
	Flag		flag;			// The flag
	Sphere		moon;			// The moon
	Light		**lights;		// circle of lights
	Light		**lights2;		// circle of lights
	Vector		camera;
	int			autoCamera;
	GLfloat		cameraTransform[16];

	const int NUMLIGHTS = 20;
	const int NUMLIGHTS2 = 3;

	static const double FOV_X; // The horizontal field of view.

	float	phi;	// Viewer's inclination angle.
	float	theta;	// Viewer's azimuthal angle.
	float	dist;	// Viewer's distance from the look-at point.
	float	x_at;	// The x-coord to look at.
	float	y_at;	// The y-coord to look at. z_at is assumed 2.0.

	int     button;	// The mouse button that is down, -1 if none.
	int	x_last;	// The location of the most recent mouse event
	int	y_last;
	int	x_down; // The location of the mouse when the button was pushed
	int	y_down;
	float   phi_down;   // The view inclination angle when the mouse
			    // button was pushed
	float   theta_down; // The view azimuthal angle when the mouse
			    // button was pushed
	float	dist_down;  // The distance when the mouse button is pushed.
	float	x_at_down;  // The x-coord to look at when the mouse went down.
	float	y_at_down;  // The y-coord to look at when the mouse went down.

	void	Drag(float);	// The function to call for mouse drag events

	int key_press(int event, int key);
};


#endif
