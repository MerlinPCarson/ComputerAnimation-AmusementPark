#include "Light.h"
#include "libtarga.h"
#include <stdio.h>
#include <GL/glu.h>
#include "Fl/math.h"

const float RADS = M_PI / 180.0;

// Constructor

Light::Light(double distance, double lightRadius, double standRadius, double theta, double height, double rotSpeed, ColorList color)
			: distance(distance), lightRadius(lightRadius), standRadius(standRadius), localTheta(theta), rotSpeed(rotSpeed), height(height){

	
	
	currColor.color = color;
	display_list = 0; 
	initialized = false;
	numSlices = 8;
	numStacks = 2;

	direction = 1;
	colorDelay = 20;
	colorTimer = 0;
	
	standLoc[0] = cos(theta*RADS)*distance;
	standLoc[1] = sin(theta*RADS)*distance;
	standLoc[2] = 0;

	lightLoc[0] = standLoc[0];
	lightLoc[1] = standLoc[1];
	lightLoc[2] = height;

	rotTheta = 0.0;


}

// Destructor
Light::~Light(void)
{
	if (initialized)
	{
		glDeleteLists(display_list, 2);
		glDeleteTextures(1, &light_text_obj);
		glDeleteTextures(1, &stand_text_obj);
	}
}

void Light::next_color(){
	
	if (direction){
		switch (currColor.color){
		case Orange:
			currColor.color = Red;
			currColor.red = 1.0;
			currColor.green = 0.0;
			currColor.blue = 0.0;
			break;
		case Red:
			currColor.color = Yellow;
			currColor.red = 1.0;
			currColor.green = 1.0;
			currColor.blue = 0.0;
			break;
		case Yellow:
			currColor.color = Purple;
			currColor.red = 0.625;
			currColor.green = 0.125;
			currColor.blue = 0.9375;
			break;
		case Purple:
			currColor.color = Orange;
			currColor.red = 1.0;
			currColor.green = 0.645;
			currColor.blue = 0.0;
			break;
		default:
			break;

		}
	}
	else{
		switch (currColor.color){
		case Orange:
			currColor.color = Purple;
			currColor.red = 0.625;
			currColor.green = 0.125;
			currColor.blue = 0.9375;
			break;
		case Purple:
			currColor.color = Yellow;
			currColor.red = 1.0;
			currColor.green = 1.0;
			currColor.blue = 0.0;
			break;
		case Yellow:
			currColor.color = Red;
			currColor.red = 1.0;
			currColor.green = 0.0;
			currColor.blue = 0.0;
			break;
		case Red:
			currColor.color = Orange;
			currColor.red = 1.0;
			currColor.green = 0.645;
			currColor.blue = 0.0;
			break;
		default:
			break;

		}
	}
}

void Light::change_lights(int button){

	switch (button){
	case 105:
		direction = 0;
		return;
	case 106:
		rotSpeed += 2;
		return;
	case 107:
		direction = 1;
		return;
	case 108:
		rotSpeed -= 2;
		return;
	}

}

// Initializer. Returns false if something went wrong, like not being able to
// load the texture.
bool
Light::Initialize(void)
{
	GLUquadricObj* light = gluNewQuadric();
	GLUquadricObj* stand = gluNewQuadric();

	ubyte   *image_data;
	int	    image_height, image_width;

	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("leds.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Light::Initialize: Couldn't load leds.tga\n");
		return false;
	}

	// Now do the geometry. Create the display list.
	display_list = glGenLists(2);

	// light display list
	glNewList(display_list, GL_COMPILE);

	glGenTextures(1, &light_text_obj);
	glBindTexture(GL_TEXTURE_2D, light_text_obj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_height, image_width, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// setup texture for light
	gluQuadricDrawStyle(light, GLU_FILL);
	gluQuadricTexture(light, GL_TRUE);
	gluQuadricNormals(light, GLU_SMOOTH);

	// set initial color.
	next_color();

	glEnable(GL_TEXTURE_2D);
	gluSphere(light, lightRadius, numSlices, numStacks);

	// Turn texturing off again, because we don't want everything else to
	// be textured.
	glDisable(GL_TEXTURE_2D);
	glEndList();
	
	// light stand list
	glNewList(display_list + 1, GL_COMPILE);

	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("tiki-texture.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Light::Initialize: Couldn't load tiki-texture.tga\n");
		return false;
	}

	glGenTextures(1, &stand_text_obj);
	glBindTexture(GL_TEXTURE_2D, stand_text_obj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_height, image_width, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// bind texture to cylinder
	gluQuadricDrawStyle(stand, GLU_FILL);
	gluQuadricTexture(stand, GL_TRUE);
	gluQuadricNormals(stand, GLU_SMOOTH);

	// Use white, because the texture supplies the color.
	//glColor3f(1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, stand_text_obj);

	// draw cylinder
	gluCylinder(stand, standRadius, 10*standRadius, height, numSlices, numStacks);
	glDisable(GL_TEXTURE_2D);

	glEndList();

	// We only do all this stuff once, when the GL context is first set up.
	initialized = true;

	return true;
}


// Draw just calls the display list we set up earlier.
void
Light::Draw(void)
{
	if (++colorTimer > colorDelay){
		next_color();
		colorTimer = 0;
	}

	// rotation angle
	rotTheta += rotSpeed;
	if (rotTheta >= 360.0){
		rotTheta = 0.0;
	}

	// Draw Light
	glPushMatrix();
	glTranslatef(lightLoc[0], lightLoc[1], lightLoc[2]);
	glRotatef(rotTheta, 0, 0, 1);
	glColor3f(currColor.red, currColor.green, currColor.blue);
	glCallList(display_list);
	glPopMatrix();

	// Draw Stand
	glPushMatrix();
	
	glTranslatef(standLoc[0], standLoc[1], standLoc[2]);
	glRotatef(rotTheta, 0, 0, 1);
	glColor3f(0.5f, 0.5f, 0.5f);
	glCallList(display_list + 1);
	glPopMatrix();

}