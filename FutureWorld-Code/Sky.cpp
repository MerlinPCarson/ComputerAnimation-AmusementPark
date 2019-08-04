#include "Sky.h"
#include "libtarga.h"
#include <stdio.h>
#include <GL/glu.h>
#include "Fl/math.h"
#include <time.h>

// Destructor
Sphere::~Sphere(void)
{
	if (initialized)
	{
		glDeleteLists(display_list, 1);
		glDeleteTextures(1, &texture_obj);
	}
}


// Initializer. Returns false if something went wrong, like not being able to
// load the texture.
bool
Sphere::Initialize(void)
{
	GLUquadricObj* sphere = gluNewQuadric();
	float radius = 15.0f;
	int numSlices = 32;
	int numStacks = 8;
	rot_theta = 0.0;
	//rev_theta = 184.75;
	rev_theta = 169.5;

	ubyte   *image_data;
	int	    image_height, image_width;
	
	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("moon-texture.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Sphere::Initialize: Couldn't load moon-texture.tga\n");
		return false;
	}


	/*
	// This creates a texture object and binds it, so the next few operations
	// apply to this texture.
	glGenTextures(1, &texture_obj);
	glBindTexture(GL_TEXTURE_2D, texture_obj);

	// This sets a parameter for how the texture is loaded and interpreted.
	// basically, it says that the data is packed tightly in the image array.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// This sets up the texture with high quality filtering. First it builds
	// mipmaps from the image data, then it sets the filtering parameters
	// and the wrapping parameters. We want the grass to be repeated over the
	// Sphere.
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image_width, image_height,
		GL_RGB, GL_UNSIGNED_BYTE, image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST_MIPMAP_LINEAR);

	// This says what to do with the texture. Modulate will multiply the
	// texture by the underlying color.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	*/
	// Now do the geometry. Create the display list.
	display_list = glGenLists(200);
	glNewList(display_list, GL_COMPILE);


	glGenTextures(1, &texture_obj);
	glBindTexture(GL_TEXTURE_2D, texture_obj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluQuadricDrawStyle(sphere, GLU_FILL);
	//glBindTexture(GL_TEXTURE_2D, texture_obj);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	// Turn on texturing and bind the texture.
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texture_obj);
	
	// Use white, because the texture supplies the color.
	
	//glColor3f(1.0, 1.0, 0.0);
	//glTranslatef(-100.0, -100.0, 30.0);
	// Use white, because the texture supplies the color.
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	gluSphere(sphere, radius, numSlices, numStacks);
	
	// Turn texturing off again, because we don't want everything else to
	// be textured.
	glDisable(GL_TEXTURE_2D);
	glEndList();

	glNewList(display_list + 1, GL_COMPILE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	//glColor3f(1.0, 1.0, 1.0);
	glPointSize(2.0f);
	glNormal3f(1.0f, 0.0f, 0.0f);

	/* initialize random seed: */
	srand(time(NULL));

	// above horizon
	for (int x = 1; x < 100; ++x){
		glNewList(display_list+x, GL_COMPILE);
		glBegin(GL_POINTS);
		glVertex3f(rand() % 300 - 150, rand() % 300 - 150, rand() % 30 + 50);
		glEnd();
		glEndList();
	}
	
	// left of horizon
	for (int x = 100; x < 125; ++x){
		glNewList(display_list + x, GL_COMPILE);
		glBegin(GL_POINTS);
		glVertex3f(- 150, rand() % 300 - 150, rand() % 25 + 25);
		glEnd();
		glEndList();
	}
	// right of horizon
	for (int x = 125; x < 150; ++x){
		glNewList(display_list + x, GL_COMPILE);
		glBegin(GL_POINTS);
		glVertex3f(150, rand() % 300 - 150, rand() % 25 + 25);
		glEnd();
		glEndList();
	}
	// near of horizon
	for (int x = 150; x < 175; ++x){
		glNewList(display_list + x, GL_COMPILE);
		glBegin(GL_POINTS);
		glVertex3f(rand() % 300 - 150, -150, rand() % 25 + 25);
		glEnd();
		glEndList();
	}
	// far of horizon
	for (int x = 175; x < 200; ++x){
		glNewList(display_list + x, GL_COMPILE);
		glBegin(GL_POINTS);
		glVertex3f(rand() % 300 - 150, 150, rand() % 25 + 25);
		glEnd();
		glEndList();
	}

	glDisable(GL_POINT_SMOOTH);
	glBlendFunc(GL_NONE, GL_NONE);
	glDisable(GL_BLEND);
		
	// We only do all this stuff once, when the GL context is first set up.
	initialized = true;

	return true;
}


// Draw just calls the display list we set up earlier.
void
Sphere::Draw(void)
{
	// rotation angle
	rot_theta += 0.2;
	if (rot_theta >= 360.0){
		rot_theta = 0.0;
	}
	// revolution angle
	rev_theta += 0.001;
	if (rev_theta >= 360.0){
		rev_theta = 0.0;
	}

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();

	// Draw Sphere
	glPushMatrix();
	glTranslatef(100*cos(rev_theta), 100*sin(rev_theta), 75.0);
	glRotatef(rot_theta, 1, 0, -1);

	glCallList(display_list);
	glPopMatrix();

	// Draw stars
	for (int x = 1; x < 200; ++x){
		glPushMatrix();
		
		chance = rand();
		// 10% chance of being yellow
		if (chance > 16384){
			glColor3f(1.0, 1.0, 0.0);
			glCallList(display_list + x);
		}
		// .01% chance of twinkling away
		else if (chance > 33){
			glColor3f(1.0, 1.0, 1.0);
			glCallList(display_list + x);
		}
		glPopMatrix();
	}

}


