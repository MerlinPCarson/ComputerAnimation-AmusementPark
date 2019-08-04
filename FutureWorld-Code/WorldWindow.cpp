/*
 * CS559 Maze Project
 *
 * Class file for the WorldWindow class.
 *
 * (c) Stephen Chenney, University of Wisconsin at Madison, 2001-2002
 *
 */

#include "WorldWindow.h"
//#include "Fl/math.h"
#include "Fl/gl.h"
#include <GL/glu.h>
#include <stdio.h>
#include <random>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

const double WorldWindow::FOV_X = 45.0;

WorldWindow::WorldWindow(int x, int y, int width, int height, char *label)
	: Fl_Gl_Window(x, y, width, height, label)
{
	//moon(1, 12, 24);
    button = -1;

    // Initial viewing parameters.
    phi = 45.0f;
    theta = 0.0f;
    dist = 100.0f;
    x_at = 0.0f;
    y_at = 0.0f;

	// camera tranformation matrix
	cameraTransform[0] = 1.0f; cameraTransform[1] = 0.0f; cameraTransform[2] = 0.0f; cameraTransform[3] = 0.0f;
	cameraTransform[4] = 0.0f; cameraTransform[5] = 1.0f; cameraTransform[6] = 0.0f; cameraTransform[7] = 0.0f;
	cameraTransform[8] = 0.0f; cameraTransform[9] = 0.0f; cameraTransform[10] = 1.0f; cameraTransform[11] = 0.0f;
	cameraTransform[12] = 0.0f; cameraTransform[13] = 0.0f; cameraTransform[14] = 0.0f; cameraTransform[15] = 1.0f;

	autoCamera = false;

}

WorldWindow::~WorldWindow(){
	for (int x = 0; x < NUMLIGHTS; ++x){
		delete lights[x];
	}

	delete[] lights;
}

/*
void WorldWindow::setCartLocation(float x, float y, float z, float anglePos, float angleTilt){
	camera.x = x;
	camera.y = y;
	camera.z = z;
	camera.anglePos = anglePos;
	camera.angleTilt = angleTilt;
}
*/

// Rotate a 3d vector about Z axis.
static void
Rotate_3(float v[3], float angle)
{
	v[0] = cos(angle * M_PI / 180) + -sin(angle * M_PI / 180);
	v[1] = sin(angle * M_PI / 180) + cos(angle * M_PI / 180);
	v[2] = v[2];
}

// Rotate a 3d vector about X axis.
static void Rotate_X(float v[3], float angle)
{
	v[0] = v[0];
	v[1] = cos((angle * M_PI) / 180) + -sin((angle * M_PI) / 180);
	v[2] = sin((angle * M_PI) / 180) + cos((angle * M_PI) / 180);
}

// Normalize a 3d vector.
static void
Normalize_3(float v[3])
{
	double  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	if (l == 0.0)
		return;

	v[0] /= (float)l;
	v[1] /= (float)l;
	v[2] /= (float)l;
}

void
WorldWindow::draw(void)
{
    double  eye[3];
    float   color[4], dir[4];

	float tangent[3];
	float trackeye[3];
	float trackposition[3];

    if ( ! valid() )
    {
	// Stuff in here doesn't change from frame to frame, and does not
	// rely on any coordinate system. It only has to be done if the
	// GL context is damaged.

	double	fov_y;

	// Sets the clear color to sky blue.
	glClearColor(0.0f, 0.0f, 0.1f, 1.0);

	// Turn on depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Turn on back face culling. Faces with normals away from the viewer
	// will not be drawn.
	glEnable(GL_CULL_FACE);

	// Enable lighting with one light.
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	// Ambient and diffuse lighting track the current color.
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Turn on normal vector normalization. You don't have to give unit
	// normal vector, and you can scale objects.
	glEnable(GL_NORMALIZE);

	// Set up the viewport.
	glViewport(0, 0, w(), h());

	// Set up the persepctive transformation.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	fov_y = 360.0f / M_PI * atan(h() * tan(FOV_X * M_PI / 360.0) / w());
	gluPerspective(fov_y, w() / (float)h(), 1.0, 1000.0);

	// Do some light stuff. Diffuse color, and zero specular color
	// turns off specular lighting.
	color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
	color[0] = 0.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_SPECULAR, color);

	// Initialize all the objects.
	ground.Initialize();
	traintrack.Initialize();
	flag.Initialize();
	moon.Initialize();

	init_lights();

    }

    // Stuff out here relies on a coordinate system or must be done on every
    // frame.

    // Clear the screen. Color and depth.
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//camera = traintrack.getLocation();

	/*
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(camera.x, camera.y, camera.z));
	View = glm::rotate(View, camera.rotAngle1, glm::vec3(0.0f, 0.0f, 1.0f));
	View = glm::rotate(View, camera.rotAngle2, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 look = View * glm::vec4(camera.x, camera.y, camera.z, 1);
	*/
	
    // Set up the viewing transformation. The viewer is at a distance 
    // dist from (x_at, y_ay, 2.0) in the direction (theta, phi) defined
    // by two angles. They are looking at (x_at, y_ay, 2.0) and z is up.
    eye[0] = x_at + dist * cos(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
    eye[1] = y_at + dist * sin(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
    eye[2] = 2.0 + dist * sin(phi * M_PI / 180.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	if (autoCamera){
		if (autoCamera == 2){	// third person
			if (traintrack.posn_on_track - 0.15f < 0)
			{
				traintrack.track->Evaluate_Point(traintrack.posn_on_track + 3.85f, trackposition);
				traintrack.track->Evaluate_Derivative(traintrack.posn_on_track + 3.85f, tangent);
			}
			else
			{
				traintrack.track->Evaluate_Point(traintrack.posn_on_track - 0.15f, trackposition);
				traintrack.track->Evaluate_Derivative(traintrack.posn_on_track - 0.15f, tangent);
			}
		}
		else{	// first person
			traintrack.track->Evaluate_Point(traintrack.posn_on_track, trackposition);
			traintrack.track->Evaluate_Derivative(traintrack.posn_on_track, tangent);
		}

		float tangentx = tangent[0];
		float tangenty = tangent[1];
		float tangentz = tangent[2];

		Normalize_3(tangent);

		double angle = atan2(tangent[1], tangent[0]) * 180.0 / M_PI;
		Rotate_3(tangent, angle);

		angle = asin(-tangent[2]) * 180.0 / M_PI;
		//std::cout << angle << std::endl;
		Rotate_X(tangent, angle);
		//Rotate_Y(tangent, angle);

		trackeye[0] = trackposition[0];
		trackeye[1] = trackposition[1];
		trackeye[2] = trackposition[2];

		//std::cout << "Tangent at (" << tangent[0] << "," << tangent[1] << "," << tangent[2] << ")" << std::endl;
		Normalize_3(tangent);
		gluLookAt(trackeye[0], trackeye[1], trackeye[2] + 2.0f, tangentx*2.0f, tangenty*2.0f, tangentz*2.0f, 0, 0, 1);
	}
	else{
		gluLookAt(eye[0], eye[1], eye[2], x_at, y_at, 2.0, 0.0, 0.0, 1.0);
		//gluLookAt(camera.x, camera.y, camera.z, x_at, y_at, 2.0, 0.0, 0.0, 1.0);
	}

    // Position the light source. This has to happen after the viewing
    // transformation is set up, so that the light stays fixed in world
    // space. This is a directional light - note the 0 in the w component.
    dir[0] = 1.0; dir[1] = 1.0; dir[2] = 1.0; dir[3] = 0.0;
    glLightfv(GL_LIGHT0, GL_POSITION, dir);

	

    // Draw stuff. Everything.
    ground.Draw();
    traintrack.Draw();
	flag.Draw();
	moon.Draw();

	for (int x = 0; x < NUMLIGHTS; ++x){
		lights[x]->Draw();
	}

	for (int x = 0; x < NUMLIGHTS2; ++x){
		lights2[x]->Draw();
	}
}

void WorldWindow::init_lights(){
	lights = new Light*[NUMLIGHTS];
	double thetaDelta = 360/(NUMLIGHTS);
	//double theta = 0.0;

	for (int x = 0; x < NUMLIGHTS; ++x){

		//theta = x*thetaDelta;
		lights[x] = new Light(45.0, 3.0, 0.3, x*thetaDelta, 5.0, 4.0, ColorList(x%4));
		lights[x]->Initialize();
	}

	lights2 = new Light*[NUMLIGHTS2];
	thetaDelta = 270 / (NUMLIGHTS2);
	//theta = 0.0;

	for (int x = 0; x < NUMLIGHTS2; ++x){

		//theta = x*thetaDelta;
		lights2[x] = new Light(30.0, 6.0, 0.6, x*thetaDelta-40, 15.0, 1.0, ColorList(x % 4));
		lights2[x]->Initialize();
	}

}

void
WorldWindow::Drag(float dt)
{
    int	    dx = x_down - x_last;
    int     dy = y_down - y_last;

    switch ( button )
    {
      case FL_LEFT_MOUSE:
	// Left button changes the direction the viewer is looking from.
	theta = theta_down + 360.0f * dx / (float)w();
	while ( theta >= 360.0f )
	    theta -= 360.0f;
	while ( theta < 0.0f )
	    theta += 360.0f;
	phi = phi_down + 90.0f * dy / (float)h();
	if ( phi > 89.0f )
	    phi = 89.0f;
	if ( phi < -5.0f )
	    phi = -5.0f;
	break;
      case FL_MIDDLE_MOUSE:
	// Middle button moves the viewer in or out.
	dist = dist_down - ( 0.5f * dist_down * dy / (float)h() );
	if ( dist < 1.0f )
	    dist = 1.0f;
	break;
      case FL_RIGHT_MOUSE: {
	// Right mouse button moves the look-at point around, so the world
	// appears to move under the viewer.
	float	x_axis[2];
	float	y_axis[2];

	x_axis[0] = -(float)sin(theta * M_PI / 180.0);
	x_axis[1] = (float)cos(theta * M_PI / 180.0);
	y_axis[0] = x_axis[1];
	y_axis[1] = -x_axis[0];

	x_at = x_at_down + 100.0f * ( x_axis[0] * dx / (float)w()
				    + y_axis[0] * dy / (float)h() );
	y_at = y_at_down + 100.0f * ( x_axis[1] * dx / (float)w()
				    + y_axis[1] * dy / (float)h() );
	} break;
      default:;
    }
}


bool
WorldWindow::Update(float dt)
{
    // Update the view. This gets called once per frame before doing the
    // drawing.

    if ( button != -1 ) // Only do anything if the mouse button is down.
	Drag(dt);

    // Animate the train.
    traintrack.Update(dt);

    return true;
}

void WorldWindow::change_lighting(int button){
	for (int x = 0; x < NUMLIGHTS; ++x){
		lights[x]->change_lights(button);
	}
	for (int x = 0; x < NUMLIGHTS2; ++x){
		lights2[x]->change_lights(button);
	}
}

int
WorldWindow::handle(int event)
{
    // Event handling routine. Only looks at mouse events.
    // Stores a bunch of values when the mouse goes down and keeps track
    // of where the mouse is and what mouse button is down, if any.

    switch ( event )
    {	
      case FL_PUSH:
        button = Fl::event_button();
		x_last = x_down = Fl::event_x();
		y_last = y_down = Fl::event_y();
		phi_down = phi;
		theta_down = theta;
		dist_down = dist;
		x_at_down = x_at;
		y_at_down = y_at;
		return 1;
      case FL_DRAG:
		x_last = Fl::event_x();
		y_last = Fl::event_y();
		return 1;
      case FL_RELEASE:
        button = -1;
		return 1;
	  case FL_KEYDOWN:
		  key_press(event, Fl::event_key());
		  //change_lighting(Fl::event_button());
		  return 1;

	}
	
    // Pass any other event types on the superclass.
    return Fl_Gl_Window::handle(event);
}

int WorldWindow::key_press(int event, int key)
{
	switch (key) {

	case 'c':
		++autoCamera;
		if (autoCamera > 2){
			autoCamera = 0;
		}
		//autoCamera = !autoCamera;
		return 1;

	default:
		change_lighting(Fl::event_button());
		return 1;
	}
}


