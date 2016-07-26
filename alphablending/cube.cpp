/*
 cube.cpp 
 */
#include <stdlib.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <GL/glut.h>

#include "vec3f.h"
#include "cube.h"

using namespace std;

const float PI = 3.1415926535f;
const float BOX_SIZE = 12.0f; //The length of each side of the cube
const float ALPHA = 0.6f; //The opacity of each face of the cube


//Initializes the up, right, and out vectors for the six faces of the cube.
void initCube(Cube &cube) {
	cube.top.up = Vec3f(0, 0, -1);
	cube.top.right = Vec3f(1, 0, 0);
	cube.top.out = Vec3f(0, 1, 0);
	
	cube.bottom.up = Vec3f(0, 0, 1);
	cube.bottom.right = Vec3f(1, 0, 0);
	cube.bottom.out = Vec3f(0, -1, 0);
	
	cube.left.up = Vec3f(0, 0, -1);
	cube.left.right = Vec3f(0, 1, 0);
	cube.left.out = Vec3f(-1, 0, 0);
	
	cube.right.up = Vec3f(0, -1, 0);
	cube.right.right = Vec3f(0, 0, 1);
	cube.right.out = Vec3f(1, 0, 0);
	
	cube.front.up = Vec3f(0, 1, 0);
	cube.front.right = Vec3f(1, 0, 0);
	cube.front.out = Vec3f(0, 0, 1);
	
	cube.back.up = Vec3f(1, 0, 0);
	cube.back.right = Vec3f(0, 1, 0);
	cube.back.out = Vec3f(0, 0, -1);
}
//Rotates the vector by the indicated number of degrees about the specified axis
Vec3f rotate(Vec3f v, Vec3f axis, float degrees) {
	axis = axis.normalize();
	float radians = degrees * PI / 180;
	float s = sin(radians);
	float c = cos(radians);
	return v * c + axis * axis.dot(v) * (1 - c) + v.cross(axis) * s;	
}

//Rotates the face by the indicated number of degrees about the specified axis
void rotate(Face &face, Vec3f axis, float degrees) {
	face.up = rotate(face.up, axis, degrees);
	face.right = rotate(face.right, axis, degrees);
	face.out = rotate(face.out, axis, degrees);
}

//Rotates the cube by the indicated number of degrees about the specified axis
void rotate(Cube &cube, Vec3f axis, float degrees) {
	rotate(cube.top, axis, degrees);
	rotate(cube.bottom, axis, degrees);
	rotate(cube.left, axis, degrees);
	rotate(cube.right, axis, degrees);
	rotate(cube.front, axis, degrees);
	rotate(cube.back, axis, degrees);
}

//Returns whether face1 is in back of face2.
bool compareFaces(Face* face1, Face* face2) {
	return face1->out[2] < face2->out[2];
}

//Stores the four vertices of the face in the array "vs".
void faceVertices(Face &face, Vec3f* vs) {
	vs[0] = BOX_SIZE / 2 * (face.out - face.right - face.up);
	vs[1] = BOX_SIZE / 2 * (face.out - face.right + face.up);
	vs[2] = BOX_SIZE / 2 * (face.out + face.right + face.up);
	vs[3] = BOX_SIZE / 2 * (face.out + face.right - face.up);
}

void drawTopFace(Face &face) {
	Vec3f vs[4];
	faceVertices(face, vs);
	
	glBegin(GL_QUADS);
	
	glColor4f(1.0f, 1.0f, 0.0f, ALPHA);
	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);
	
	glEnd();
}
void drawBottomFace(Face &face) {
	Vec3f vs[4];
	faceVertices(face, vs);

	glBegin(GL_QUADS);
	
	glColor4f(1.0f, 0.0f, 1.0f, ALPHA);
	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);
	
	glEnd();
}
void drawLeftFace(Face &face) {
	Vec3f vs[4];
	faceVertices(face, vs);
	
	glBegin(GL_QUADS);
	
	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glColor4f(0.0f, 1.0f, 1.0f, ALPHA);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glColor4f(0.0f, 0.0f, 1.0f, ALPHA);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);
	
	glEnd();
}
void drawRightFace(Face &face) {
	Vec3f vs[4];
	faceVertices(face, vs);
	
	glBegin(GL_QUADS);
	
	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glColor4f(1.0f, 0.0f, 0.0f, ALPHA);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glColor4f(0.0f, 1.0f, 0.0f, ALPHA);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);
	
	glEnd();
}
void drawFrontFace(Face &face) {
	Vec3f vs[4];
	faceVertices(face, vs);
	
	glBegin(GL_QUADS);
	
	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glColor4f(0.7f, 0.0f, 1.0f, ALPHA);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glColor4f(0.0f, 0.0f, 0.2f, ALPHA);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);
	
	glEnd();
}
void drawBackFace(Face &face) {
	Vec3f vs[4];
	faceVertices(face, vs);
	
	glBegin(GL_QUADS);

	
	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glColor4f(1.0f, 1.0f, 1.0f, ALPHA);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glColor4f(0.0f, 1.0f, 0.0f, ALPHA);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);
	
	glEnd();
}

//Draws the indicated face on the specified cube.
void drawFace(Face* face, Cube &cube) {
	if (face == &(cube.top)) {
		drawTopFace(cube.top);
	}
	else if (face == &(cube.bottom)) {
		drawBottomFace(cube.bottom);
	}
	else if (face == &(cube.left)) {
		drawLeftFace(cube.left);
	}
	else if (face == &(cube.right)) {
		drawRightFace(cube.right);
	}
	else if (face == &(cube.front)) {
		drawFrontFace(cube.front);
	}
	else {
		drawBackFace(cube.back);
	}
}


void drawCube(Cube &_cube){
  
  vector<Face*> faces;
	faces.push_back(&(_cube.top));
	faces.push_back(&(_cube.bottom));
	faces.push_back(&(_cube.left));
	faces.push_back(&(_cube.right));
	faces.push_back(&(_cube.front));
	faces.push_back(&(_cube.back));
	
	//Sort the faces from back to front
	sort(faces.begin(), faces.end(), compareFaces);
	
	for(unsigned int i = 0; i < faces.size(); i++) {
		drawFace(faces[i], _cube);
	}  
}
