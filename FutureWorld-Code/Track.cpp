/*
 * Track.cpp: A class that draws the train and its track.
 *
 * (c) 2001-2002: Stephen Chenney, University of Wisconsin at Madison.
 */


#include "Track.h"
#include <stdio.h>
#include "FL/math.h"
#include <math.h>

const float RADS = M_PI / 180.0;

// The control points for the track spline.
const int   Track::TRACK_NUM_CONTROLS = 4;
const float Track::TRACK_CONTROLS[TRACK_NUM_CONTROLS][3] =
		{ { -20.0, -20.0, -18.0 }, { 20.0, -20.0, 40.0 },
		  { 20.0, 20.0, -18.0 }, { -20.0, 20.0, 40.0 } };

// The carriage energy and mass
const float Track::TRAIN_ENERGY = 250.0f;


// Normalize a 3d vector.
static void
Normalize_3(float v[3])
{
    double  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    if ( l == 0.0 )
	return;

    v[0] /= (float)l;
    v[1] /= (float)l;
    v[2] /= (float)l;
}


// Destructor
Track::~Track(void)
{
    if ( initialized )
    {
	glDeleteLists(track_list, 1);
	glDeleteLists(train_list, 1);
    }
}


// Initializer. Would return false if anything could go wrong.
bool
Track::Initialize(void)
{
    //CubicBspline    refined(3, true);
	refined = new CubicBspline(3, true);
    //int		    n_refined;
    float	    p[3];

    int		    i;


	float radius = 0.125f;
	float height = 1.0;
	int numSlices = 32;
	int numStacks = 8;

	rot_theta = 0.0;
	rot_theta2 = 0.0;
	rot_theta3 = 0.0;

	// vars for color cycling
	currColor.color = Orange;
	direction = 1;
	colorDelay = 8;
	colorTimer = 0;

	// track sign
	// Now do the geometry. Create the display list.
	sign_list = glGenLists(1);

	// base display list
	glNewList(sign_list, GL_COMPILE);

	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("signFlip.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Flag::Initialize: Couldn't load sign.tga\n");
		return false;
	}

	// create pole texture object
	glGenTextures(1, &texture_obj);
	glBindTexture(GL_TEXTURE_2D, texture_obj);

	// This sets a parameter for how the texture is loaded and interpreted.
	// basically, it says that the data is packed tightly in the image array.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_height, image_width, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_obj);

	// draw track sign
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(4.0f, .25f, 5.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-4.0f, .25f, 5.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-4.0f, -.25f, 5.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(4.0f, -.25f, 5.0f);
	
	glNormal3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(4.0f, .25f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(4.0f, .25f, 5.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(4.0f, -.25f, 5.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(4.0f, -.25f, 1.0f);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-4.0f, .25f, 5.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-4.0f, .25f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-4.0f, -.25f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-4.0f, -.25f, 5.0f);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(4.0f, .25f, 5.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(4.0f, .25f, 1.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-4.0f, .25f, 1.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-4.0f, .25f, 5.0f);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(4.0f, -.25f, 1.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(4.0f, -.25f, 5.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-4.0f, -.25f, 5.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-4.0f, -.25f, 1.0f);
	
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(4.0f, .25f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-4.0f, .25f, 1.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-4.0f, -.25f, 1.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(4.0f, -.25f, 1.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEndList();


    // Create the track spline.
    track = new CubicBspline(3, true);
    for ( i = 0 ; i < TRACK_NUM_CONTROLS ; i++ )
	track->Append_Control(TRACK_CONTROLS[i]);

    // Refine it down to a fixed tolerance. This means that any point on
    // the track that is drawn will be less than 0.1 units from its true
    // location. In fact, it's even closer than that.
    track->Refine_Tolerance(*refined, 0.1f);
    n_refined = refined->N();

    // Create the display list for the track - just a set of line segments
    // We just use curve evaluated at integer paramer values, because the
    // subdivision has made sure that these are good enough.	
	
	GLUquadricObj* tracks = gluNewQuadric();
    track_list = glGenLists(2);
    glNewList(track_list, GL_COMPILE);
	if (!(image_data = (ubyte*)tga_load("Steel2.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Sphere::Initialize: Couldn't load Steel2.tga\n");
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, texture_obj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluQuadricDrawStyle(tracks, GLU_FILL);
	gluQuadricTexture(tracks, GL_TRUE);
	gluQuadricNormals(tracks, GLU_SMOOTH);
	glColor3f(1.0f, 1.0, 1.0f);
	

	glBegin(GL_POLYGON);
	    for ( i = 0 ; i <= n_refined ; i++ )
	    {
		refined->Evaluate_Point((float)i, p);
		refined->Evaluate_Derivative(float(i), tangent);
		Normalize_3(tangent);
		glPushMatrix();
		glTranslatef(p[0], p[1], p[2]);
		
		glRotatef(tangent[0]*180/M_PI, 1, 0, 0);
		glRotatef(tangent[1] * 180 / M_PI, 0, 1, 0);
		glRotatef(tangent[2] * 180 / M_PI, 0, 0, 1);
		glEnable(GL_TEXTURE_2D);
		gluCylinder(tracks, radius, radius, height, numSlices, numStacks);
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	    }
	glEnd();
	glEndList();

	GLUquadricObj* sphere = gluNewQuadric();
	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("leds.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Sphere::Initialize: Couldn't load leds.tga\n");
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, texture_obj2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);

	radius = 0.15f;
	numSlices = 32;
	numStacks = 8;

	glNewList(track_list+1, GL_COMPILE);
	glEnable(GL_TEXTURE_2D);
	gluSphere(sphere, radius, numSlices, numStacks);
	glDisable(GL_TEXTURE_2D);
	glEndList();

	train_list = glGenLists(2);
    glNewList(train_list, GL_COMPILE);
	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("red_lens2.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Flag::Initialize: Couldn't load red_lens2.tga\n");
		return false;
	}

	glGenTextures(1, &texture_obj);
	glBindTexture(GL_TEXTURE_2D, texture_obj);

	// This sets a parameter for how the texture is loaded and interpreted.
	// basically, it says that the data is packed tightly in the image array.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Use white, because the texture supplies the color.
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_obj);


    // Set up the train. At this point a cube is drawn. NOTE: The
    // x-axis will be aligned to point along the track. The origin of the
    // train is assumed to be at the bottom of the train.

	
	// Torus sweep
	//glTranslatef(2.0, 0.0, 0.0);
	radius = 0.5;
	int j, k;
	double s, t, x, y, z, twopi;
	//float texCoor[3];
	int currTextCoor = 0;
	int numc = 100;
	int numt = 100;
	glPushMatrix();
	twopi = 2 * (double)M_PI;
	for (i = 0; i < numc; i++) {
		
		glRotatef(90, 0, 1, 0);
		glBegin(GL_QUAD_STRIP);
		//glNormal3f(0.0f, 0.0f, -1.0f);
		for (j = 0; j <= numt; j++) {
			for (k = 1; k >= 0; k--) {
				s = (i + k) % numc + 0.5;
				t = j % numt;

				x = (2 + .1*cos(s*twopi / numc))*cos(t*twopi / numt);
				y = (2 + .1*cos(s*twopi / numc))*sin(t*twopi / numt);
				z = .1 * sin(s * twopi / numc);
				switch (currTextCoor){
				case (0) : glTexCoord2f(1.0, 1.0); break;
				case (1) : glTexCoord2f(1.0, 0.0); break;
				case (2) : glTexCoord2f(0.0, 0.0); break;
				case (3) : glTexCoord2f(0.0, 1.0); break;
				default: break;
				}
				glVertex3f(x, y, z);
				++currTextCoor;
				if (currTextCoor > 3){
					currTextCoor = 0;
				}
			}
		}
	}
	glEnd();
	glPopMatrix();
	glEndList();
	glDisable(GL_TEXTURE_2D);

	glNewList(train_list+1, GL_COMPILE);
	
	sphere = gluNewQuadric();
	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("blue_screen.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Sphere::Initialize: Couldn't load electric.tga\n");
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, texture_obj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	
	glPushMatrix();

	//glTranslatef(0.0,0.0,0.65);

	gluSphere(sphere, radius, numSlices, numStacks);
	glPopMatrix();
	glEndList();
	glDisable(GL_TEXTURE_2D);
	

	
	GLUquadricObj* rods = gluNewQuadric();
	rod_list = glGenLists(1);
	glNewList(rod_list, GL_COMPILE);
	if (!(image_data = (ubyte*)tga_load("electric.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Sphere::Initialize: Couldn't load electric.tga\n");
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, texture_obj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluQuadricDrawStyle(rods, GLU_FILL);
	gluQuadricTexture(rods, GL_TRUE);
	gluQuadricNormals(rods, GLU_SMOOTH);
	glColor3f(1.0f, 1.0, 1.0f);

	

	
	//glNewList(rod_list, GL_COMPILE);
	
	radius = 0.125;
	height = 1.00;

	glPushMatrix();
	//glTranslatef(0.0, 0.75, 0.0);
	//glRotatef(-45, 1, 0, 0);
	gluCylinder(rods, radius, radius, height, numSlices, numStacks);
	glPopMatrix();
/*
	glPushMatrix();
	glTranslatef(0.0, -0.75, 0.0);
	glRotatef(45, 1, 0, 0);
	gluCylinder(trackRight, radius, radius, height, numSlices, numStacks);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.0, 1.25, -1.5 + radius);
	glRotatef(45, 1, 0, 0);
	gluCylinder(trackRight, radius, radius, height+0.25, numSlices, numStacks);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, -1.25, -1.5 + radius);
	glRotatef(-45, 1, 0, 0);
	gluCylinder(trackRight, radius, radius, height+0.25, numSlices, numStacks);
	glPopMatrix();
	*/
	glEndList();
	glDisable(GL_TEXTURE_2D);

    initialized = true;

    return true;
}

Vector Track::getLocation(){
	return cartPos;
}


// Draw
void
Track::Draw(void)
{
    float   posn[3];
    float   tangent[3];
	//float	color[3];

    double  angle1;
	double  angle2;

	// direction and timing of light cycling on track

	if (++colorTimer > colorDelay){
		next_color();
		colorTimer = 0;
	}

	colorIdx = currColor.color;

	// rotation angles
	rot_theta += speed;
	if (rot_theta >= 360.0){
		rot_theta = 0.0;
	}
	rot_theta2 -= 2.0;
	if (rot_theta2 < 0.0){
		rot_theta2 = 360.0;
	}
	rot_theta3 -= 95.0;
	if (rot_theta3 < 0.0){
		rot_theta3 = 360.0;
	}

    if ( ! initialized )
	return;

	// sign for track
	glCallList(sign_list);

	// lights on track
	for (int i = 0; i <= n_refined; i++)
	{
		refined->Evaluate_Point((float)i, posn);
		refined->Evaluate_Derivative(float(i), tangent);
		Normalize_3(tangent);
		glPushMatrix();
		glTranslatef(posn[0], posn[1], posn[2]);
		get_color(colorIdx);
		glColor3f(currColor.red, currColor.green, currColor.red);
		++colorIdx;
		if (colorIdx > 3){
			colorIdx = 0;
		}
		glCallList(track_list+1);
		glPopMatrix();
	}

    // Draw the track
    glCallList(track_list);

    glPushMatrix();

    // Figure out where the train is
    track->Evaluate_Point(posn_on_track, posn);

    // Translate the train to the point
    glTranslatef(posn[0], posn[1], posn[2]+0.85);

    // ...and what it's orientation is
    track->Evaluate_Derivative(posn_on_track, tangent);
    Normalize_3(tangent);

    // Rotate it to poitn along the track, but stay horizontal
    angle1 = atan2(tangent[1], tangent[0]) * 180.0 / M_PI;
    glRotatef((float)angle1, 0.0f, 0.0f, 1.0f);

    // Another rotation to get the tilt right.
    angle2 = asin(-tangent[2]) * 180.0 / M_PI;
    glRotatef((float)angle2, 0.0f, 1.0f, 0.0f);
			
    // Draw the torus of train
	glPushMatrix();
	glRotatef(rot_theta2, 0.5f, 0.0f, 0.0f);
    glCallList(train_list);
	glPopMatrix();

	// Draw sphere of train
	glPushMatrix();
	//glTranslatef(0.0, 0.0, 0.65);
	glRotatef(rot_theta, -0.5f, 1.0f, 0.0f);
	glCallList(train_list+1);
	glPopMatrix();
	
	// Draw rods of train
	glPushMatrix();
	glRotatef(rot_theta2, 0.5f, 0.0f, 0.0f);
	glTranslatef(0.0, 0.55, 0.72);
	glRotatef(-45, 1, 0, 0);
	glRotatef(rot_theta3, 0, 0, 1);
	glCallList(rod_list);
	glPopMatrix();
	
	glPushMatrix();
	glRotatef(rot_theta2, 0.5f, 0.0f, 0.0f);
	glTranslatef(0.0, -0.55, 0.72);
	glRotatef(45, 1, 0, 0);
	glRotatef(rot_theta3, 0, 0, 1);
	glCallList(rod_list);
	glPopMatrix();
	
	glPushMatrix();
	glRotatef(rot_theta2, 0.5f, 0.0f, 0.0f);
	glTranslatef(0.0, 1.25, -1.42);
	glRotatef(45, 1, 0, 0);
	glRotatef(rot_theta3, 0, 0, 1);
	glCallList(rod_list);
	glPopMatrix();
	
	glPushMatrix();
	glRotatef(rot_theta2, 0.5f, 0.0f, 0.0f);
	glTranslatef(0.0, -1.25, -1.42);
	glRotatef(-45, 1, 0, 0);
	glRotatef(rot_theta3, 0, 0, 1);
	glCallList(rod_list);
	glPopMatrix();
	
    glPopMatrix();
    glPopMatrix();

	/*
	cartPos.x = posn[0];
	cartPos.y = posn[1];
	cartPos.z = posn[2];
	cartPos.rotAngle1 = angle1;
	cartPos.rotAngle2 = angle2;
		*/
}


void
Track::Update(float dt)
{
    float   point[3];
    float   deriv[3];
    double  length;
    double  parametric_speed;

    if ( ! initialized )
	return;

    // First we move the train along the track with its current speed.

    // Get the derivative at the current location on the track.
    track->Evaluate_Derivative(posn_on_track, deriv);

    // Get its length.
    length = sqrt(deriv[0]*deriv[0] + deriv[1]*deriv[1] + deriv[2]*deriv[2]);
    if ( length == 0.0 )
	return;

    // The parametric speed is the world train speed divided by the length
    // of the tangent vector.
    parametric_speed = speed / length;

    // Now just evaluate dist = speed * time, for the parameter.
    posn_on_track += (float)(parametric_speed * dt);

    // If we've just gone around the track, reset back to the start.
	if (posn_on_track > track->N()){
		posn_on_track -= track->N();
		direction = -direction;
	}

    // As the second step, we use conservation of energy to set the speed
    // for the next time.
    // The total energy = z * gravity + 1/2 speed * speed, assuming unit mass
    track->Evaluate_Point(posn_on_track, point);
	refined->Evaluate_Derivative(posn_on_track, tangent);
	//Normalize_3(tangent);
    if ( TRAIN_ENERGY - 9.81 * point[2] < 0.0 )
	speed = 0.0;
    else
	speed = (float)sqrt(2.0 * ( TRAIN_ENERGY - 9.81 * point[2] )) + 20*tangent[2];
}

void Track::get_color(int colorIdx){

	switch (colorIdx){
		case Red:
			currColor.red = 1.0;
			currColor.green = 0.0;
			currColor.blue = 0.0;
			break;
		case Yellow:
			currColor.red = 0.0;
			currColor.green = 1.0;
			currColor.blue = 0.0;
			break;
		case Purple:
			currColor.red = 0.0;
			currColor.green = 0.0;
			currColor.blue = 1.0;
			break;
		case Orange:
			currColor.red = 1.0;
			currColor.green = 1.0;
			currColor.blue = 1.0;
			break;
		default:
			break;

	}
	
}

void Track::next_color(){

	if (direction){
		switch (currColor.color){
		case Orange:
			currColor.color = Red;
			break;
		case Red:
			currColor.color = Yellow;
			break;
		case Yellow:
			currColor.color = Purple;
			break;
		case Purple:
			currColor.color = Orange;
			break;
		default:
			break;

		}
	}
	else{
		switch (currColor.color){
		case Orange:
			currColor.color = Purple;
			break;
		case Purple:
			currColor.color = Yellow;
			break;
		case Yellow:
			currColor.color = Red;
			break;
		case Red:
			currColor.color = Orange;
			break;
		default:
			break;

		}
	}
}

void Track::TorusPoint(int numc, int numt){
	int i, j, k;
	double s, t, x, y, z, twopi;

	twopi = 2 * (double)M_PI;
	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= numt; j++) {
			for (k = 1; k >= 0; k--) {
				s = (i + k) % numc + 0.5;
				t = j % numt;

				x = (1 + .1*cos(s*twopi / numc))*cos(t*twopi / numt);
				y = (1 + .1*cos(s*twopi / numc))*sin(t*twopi / numt);
				z = .1 * sin(s * twopi / numc);
				glVertex3f(x, y, z);
			}
		}
		glEnd();
	}
}