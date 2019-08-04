#include "Flag.h"
#include "libtarga.h"
#include <stdio.h>
#include <GL/glu.h>
#include <math.h>


// Destructor
Flag::~Flag(void)
{
	if (initialized)
	{
		glDeleteLists(display_list, 2);
		glDeleteTextures(1, &pole_text_obj);
		glDeleteTextures(1, &flag_text_obj);
	}
}


// Initializer. Returns false if something went wrong, like not being able to
// load the texture.
bool
Flag::Initialize(void)
{
	// flag movement vars
	flag_rot_theta = 0.0;
	flag_rot_direction = 0;
	flag_shear = 0;
	flag_shear_direction = 0;

	// shear matrix
	shear[0] = 1.0f; shear[1] = 0.0f; shear[2] = 0.0f; shear[3] = 0.0f;
	shear[4] = 0.0f; shear[5] = 1.0f; shear[6] = 0.0f; shear[7] = 0.0f;
	shear[8] = 0.0f; shear[9] = 0.0f; shear[10] = 1.0f; shear[11] = 0.0f;
	shear[12] = 0.0f; shear[13] = 0.0f; shear[14] = 0.0f; shear[15] = 1.0f;
	
	// pole movement vars
	pole_rot_theta = 0.0;
	pole_rot_direction = 0;
	
	ubyte   *image_data;
	int	    image_height, image_width;

	GLUquadricObj* pole = gluNewQuadric();
	GLUquadricObj* ball = gluNewQuadric();
	float radius = 0.25f;
	int height = 28;
	int numSlices = 32;
	int numStacks = 8;
	
	// Now do the geometry. Create the display list.
	display_list = glGenLists(3);

	// base display list
	glNewList(display_list, GL_COMPILE);

	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("concrete.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Flag::Initialize: Couldn't load concrete.tga\n");
		return false;
	}

	// create pole texture object
	glGenTextures(1, &base_text_obj);
	glBindTexture(GL_TEXTURE_2D, base_text_obj);

	// This sets a parameter for how the texture is loaded and interpreted.
	// basically, it says that the data is packed tightly in the image array.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, base_text_obj);

	// draw concrete base of flag pole
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0f, 2.5f, 2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5.0f, 2.5f, 2.5f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0f, -2.5f, 2.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5.0f, -2.5f, 2.5f);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0f, 2.5f, 0.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5.0f, 2.5f, 2.5f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5.0f, -2.5f, 2.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5.0f, -2.5f, 0.0f);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-5.0f, 2.5f, 2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5.0f, 2.5f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0f, -2.5f, 0.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5.0f, -2.5f, 2.5f);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0f, 2.5f, 2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5.0f, 2.5f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0f, 2.5f, 0.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5.0f, 2.5f, 2.5f);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5.0f, -2.5f, 0.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5.0f, -2.5f, 2.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-5.0f, -2.5f, 2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5.0f, -2.5f, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEndList();
	
	// pole display list
	glNewList(display_list+1, GL_COMPILE);

	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("Steel2.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Flag::Initialize: Couldn't load Steel.tga\n");
		return false;
	}

	// create pole texture object
	glGenTextures(1, &pole_text_obj);
	glBindTexture(GL_TEXTURE_2D, pole_text_obj);

	// This sets a parameter for how the texture is loaded and interpreted.
	// basically, it says that the data is packed tightly in the image array.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, (const GLvoid *)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// bind texture to cylinder
	gluQuadricDrawStyle(pole, GLU_FILL);
	gluQuadricTexture(pole, GL_TRUE);
	gluQuadricNormals(pole, GLU_SMOOTH);

	// Use white, because the texture supplies the color.
	//glColor3f(1.0, 1.0, 1.0);
	/*
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glEnable(GL_COLOR_MATERIAL);
	*/
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pole_text_obj);

	// draw cylinder
	gluCylinder(pole, radius, radius, height, numSlices, numStacks);
	glDisable(GL_TEXTURE_2D);

	// bind texture to sphere
	gluQuadricDrawStyle(ball, GLU_FILL);
	gluQuadricTexture(ball, GL_TRUE);
	gluQuadricNormals(ball, GLU_SMOOTH);
	// Use white, because the texture supplies the color.
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pole_text_obj);
	// draw sphere
	glTranslatef(0.0, 0.0, height);
	gluSphere(ball, 0.5, numSlices, numStacks);
	glDisable(GL_TEXTURE_2D);

	glEndList();

	// Flag display list
	glNewList(display_list+2, GL_COMPILE);
	
	// Load the image for the texture. The texture file has to be in
	// a place where it will be found.
	if (!(image_data = (ubyte*)tga_load("flag.tga", &image_width,
		&image_height, TGA_TRUECOLOR_24)))
	{
		fprintf(stderr, "Flag::Initialize: Couldn't load flag.tga\n");
		return false;
	}

	glGenTextures(1, &flag_text_obj);
	glBindTexture(GL_TEXTURE_2D, flag_text_obj);

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
	glBindTexture(GL_TEXTURE_2D, flag_text_obj);

	// Draw the Flag as a quadrilateral, specifying texture coordinates.
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0f, 0.5f, 2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5.0f, 0.05f, 2.5f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0f, -0.05f, 2.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5.0f, -0.5f, 2.5f);
	
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0f, -0.5f, -2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5.0f, -0.05f, -2.5f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0f, 0.05f, -2.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5.0f, 0.5f, -2.5f);
	
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0f, 0.5f, -2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5.0f, 0.5f, 2.5f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5.0f, -0.5f, 2.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5.0f, -0.5f, -2.5f);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-5.0f, 0.05f, 2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5.0f, 0.05f, -2.5f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0f, -0.05f, -2.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5.0f, -0.05f, 2.5f);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0f, 0.5f, 2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5.0f, 0.5f, -2.5f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0f, 0.05f, -2.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5.0f, 0.05f, 2.5f);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5.0f, -0.5f, -2.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5.0f, -0.5f, 2.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-5.0f, -0.05f, 2.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5.0f, -0.05f, -2.5f);
	
	glEnd();
	
	// Turn texturing off again, because we don't want everything else to
	// be textured.
	glDisable(GL_TEXTURE_2D);
	glEndList();

	//glNewList(display_list + 2, GL_COMPILE);
	

	// We only do all this stuff once, when the GL context is first set up.
	initialized = true;

	return true;
}


// Draw just calls the display list we set up earlier.
void
Flag::Draw(void)
{	
	// pole rotation angle
	if (pole_rot_direction == 0){
		pole_rot_theta += 0.02;
		if (pole_rot_theta >= 2){
			pole_rot_direction = 1;
		}

	}
	else{
		pole_rot_theta -= 0.02;
		if (pole_rot_theta <= -2){
			pole_rot_direction = 0;
		}
	}

	// Draw base
	glPushMatrix();
	glTranslatef(-21, -6, 0);
	glCallList(display_list);
	glPopMatrix();

	// Draw pole
	glPushMatrix();
	glTranslatef(-21, -6, 0);
	glRotatef(pole_rot_theta, 1, 0, 0);
	glCallList(display_list+1);
	glPopMatrix();

	// flag rotation angle
	if (flag_rot_direction == 0){
		flag_rot_theta += 0.25;
		if (flag_rot_theta >= 20){
			flag_rot_direction = 1;
		}

	}
	else{
		flag_rot_theta -= 0.25;
		if (flag_rot_theta <= -20){
			flag_rot_direction = 0;
		}
	}
	
	// flag shear
	if (flag_shear_direction == 0){
		flag_shear += 0.01;
		if (flag_shear >= 0.25){
			flag_shear_direction = 1;
		}

	}
	else{
		flag_shear -= 0.01;
		if (flag_shear <= -0.25){
			flag_shear_direction = 0;
		}
	}
	
	// Draw flag
	glPushMatrix();
	//glTranslatef(-5, 0, 22);
	shear[2] = flag_shear;
	//shear[4] = flag_rot_theta / 100;
	glTranslatef(-21, -6, 0);
	glMultMatrixf(shear);
	glRotatef(flag_rot_theta, 0, 0, 1);
	glRotatef(pole_rot_theta, 1, 0, 0);
	
	glTranslatef(-5, 0, 25);
	glCallList(display_list+2);
	glPopMatrix();

}


