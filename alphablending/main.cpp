#include <algorithm>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

#include <GL/glut.h>
#include "vec3f.h"
using namespace std;

const float PI = 3.1415926535f;
const float BOX_SIZE = 7.0f; //The length of each side of the cube
const float ALPHA = 0.6f; //The opacity of each face of the cube

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
}
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			exit(0);
	}
}
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	
	glEnable(GL_BLEND); //Enable alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function
	
}

//Three perpendicular vectors for a face of the cube.  out indicates the
//direction that the face is from the center of the cube.
struct Face {
	Vec3f up;
	Vec3f right;
	Vec3f out;
};
//Represents a cube.
struct Cube {
	Face top;
	Face bottom;
	Face left;
	Face right;
	Face front;
	Face back;
};

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


Cube _cube; //The cube

GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
GLfloat lightPos[] = {-2 * BOX_SIZE, BOX_SIZE, 4 * BOX_SIZE, 1.0f};

void drawScene() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(0.0f, 0.0f, -20.0f);
	
	GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	
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
	
	glutSwapBuffers();
}
void update(int value) {
	rotate(_cube, Vec3f(0, 1, 0), 1);
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	
	glutCreateWindow("Vjera Turk");
	initRendering();
	initCube(_cube);
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}









