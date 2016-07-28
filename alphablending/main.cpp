#include <stdlib.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <GL/glut.h>

#include <set> //rekao profesor da ne koristi

#include "vec3f.h"
#include "cube.h"
#include "ball.h"

using namespace std;

//const float GRAVITY = 8.0f;
const float PI = 3.1415926535f;
const float BOX_SIZE = 12.0f; //The length of each side of the cube
const float ALPHA = 0.6f; //The opacity of each face of the cube

//Returns a random float from 0 to < 1
float randomFloat() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

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
	glEnable(GL_CULL_FACE);
	
	
}
void drawAxes(void)
{
 
    /*  Length of axes */
    double len = 2.0;
    
    glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
      
	glBegin(GL_LINES);
	glVertex3d(0,0,0);
	glVertex3d(len,0,0);
	glVertex3d(0,0,0);
	glVertex3d(0,len,0);
	glVertex3d(0,0,0);
	glVertex3d(0,0,len);
	glEnd();
    /*  Label axes */
	glRasterPos3d(len,0,0);
	
	glRasterPos3d(0,len,0);
	
	glRasterPos3d(0,0,len);
   
     glEnable(GL_LIGHTING);
  
}

GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
//GLfloat lightColor[] = {1, 1, 1, 1.0f};
GLfloat lightPos[] = {-2 * BOX_SIZE, BOX_SIZE, 4 * BOX_SIZE, 1.0f};

void TurnOnLight(){
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

const float TIME_BETWEEN_UPDATES = 0.001f;
const int TIMER_MS = 25; //The number of milliseconds to which the timer is set
float _angle = 0.0f; //kako cemo vrtiti sve ako se kocka vrti sama?

Cube _cube; //The cube

void update(int value) {
  
	rotate(_cube, Vec3f(0, 1, 0), 1);
  	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}


vector<Ball*> _balls; //All of the balls in play
void drawScene() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glShadeModel(GL_SMOOTH);
	
		
	gluLookAt ( 0.0, 0, 10.0, // camera
		    0.0, 0.0, 0.0, // where
		    0.0f, 3.0f, 0.0f ); // up vector
  
	glTranslatef(0.0f, 0.0f, -20.0f);
	
	TurnOnLight();

      glPushMatrix();
	glTranslatef(-2 * BOX_SIZE, BOX_SIZE, 4 * BOX_SIZE);
	glutSolidSphere(1,10,10);
      glPopMatrix();

	drawAxes();
	
	
      /*
	for(unsigned int i = 0; i < _balls.size(); i++) {
		Ball* ball = _balls[i];
		glPushMatrix();
		glTranslatef(ball->pos[0], ball->pos[1], ball->pos[2]);
		//glColor3f(ball->color[0], ball->color[1], ball->color[2]);
		glColor3f(1,1,1);
		glutSolidSphere(ball->r, 12, 12); //Draw a sphere
		glPopMatrix();
	}
	*/
	//glCullFace(GL_FRONT);
	//glCullFace(GL_BACK);
	
	drawCube(_cube); //in cube.cpp
	
	//Draw the ballsy
      
      
	glutSwapBuffers();
	
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









