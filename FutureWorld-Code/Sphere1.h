#ifndef __SPHERE__
#define __SPHERE__
#define _USE_MATH_DEFINES
#include "FL/gl.h"
#include "Fl/math.h"
#include <GL/glu.h>
#include <stdio.h>
#include "Fl/Fl.h"
#include "Fl/Fl_Gl_Window.h"
#include <vector>
#include <cmath>

// your framework of choice here

class Sphere
{
protected:
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> texcoords;
	std::vector<GLushort> indices;
	float radius;
	unsigned int rings;
	unsigned int sectors;
public:
	void draw(GLfloat x, GLfloat y, GLfloat z);
	//Sphere(float radius, unsigned int rings, unsigned int sectors);
	Sphere();
};
#endif