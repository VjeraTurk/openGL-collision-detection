
#include <stdlib.h>
#include <vector>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set> //rekao profesor da ne koristim
#include <GL/glut.h>

#include "vec3f.h"

using namespace std;

const float BOX_SIZE = 12.0f; //The length of one side of the box
const float TIME_BETWEEN_UPDATES = 0.001f;

void changeSize ( int w, int h )
{
    if ( h == 0 )
    {
        h = 1;
    }

    float ratio = 1.0* w / h;

    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
    glViewport ( 0, 0, w, h );
    gluPerspective ( 45,ratio,1,1000 ); // view angle u y, aspect, near, far
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			//cleanup();
			exit(0);
	/*
		case ' ':
			//Add 20 balls with a random position, velocity, radius, and color
			for(int i = 0; i < 20; i++) {
				Ball* ball = new Ball();
				ball->pos = Vec3f(8 * randomFloat() - 4,
								  8 * randomFloat() - 4,
								  8 * randomFloat() - 4);
				ball->v = Vec3f(8 * randomFloat() - 4,
								8 * randomFloat() - 4,
								8 * randomFloat() - 4);
				ball->r = 0.1f * randomFloat() + 0.1f;
				ball->color = Vec3f(0.6f * randomFloat() + 0.2f,
									0.6f * randomFloat() + 0.2f,
									0.6f * randomFloat() + 0.2f);
				_balls.push_back(ball);
				_octree->add(ball);
			}
	
	  */
	}
}

void initRendering() {
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	
	//za prozirnost!
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv) {
	
	//srand((unsigned int)time(0)); //Seed the random number generator
	
	 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	
	glutCreateWindow("Collision Detection");
	initRendering();
	TurnOnLight();
	/*
	 octree
	 
	 */
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(changeSize);
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}

GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
GLfloat lightPos[] = {1.0f, 0.2f, 0.0f, 0.0f};

void TurnOnLight(){

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);  
}

void drawBox(){
  //Draw the top and the bottom of the box
	glShadeModel(GL_FLAT);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	//prozirnost
	//glColor4f(1.0f, 1.0f, 1.0f, 0.7);
	//glColor4b(1.0f, 1.0f, 1.0f, 0.75);
	
	
	
	glBegin(GL_QUADS);
	//bottom
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	
	//top
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	
	//front
	glNormal3f(0.0f, 0.0f, 1.0f); 
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	
	//back
	glNormal3f(0.0f, 0.0f, -1.0f); 
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	
	//right
	glNormal3f(1.0f, 0.0f, 0.0f); 
	glVertex3f(BOX_SIZE / 2,-BOX_SIZE / 2,-BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2,-BOX_SIZE / 2,BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2,BOX_SIZE / 2,BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2,BOX_SIZE / 2,-BOX_SIZE / 2);
	
	//left
	glNormal3f(-1.0f, 0.0f, 0.0f); 
	glVertex3f(-BOX_SIZE / 2,-BOX_SIZE / 2,-BOX_SIZE / 2);  
	glVertex3f(-BOX_SIZE / 2,-BOX_SIZE / 2,BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2,BOX_SIZE / 2,BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2,BOX_SIZE / 2,-BOX_SIZE / 2);
	 
    glEnd();
	glShadeModel(GL_SMOOTH);
	
}
void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	glTranslatef(0.0f, 0.0f, -20.0f);
	//glRotatef(-_angle, 0.0f, 1.0f, 0.0f);
	
	drawBox();
		
	//Draw the balls
	/*
	for(unsigned int i = 0; i < _balls.size(); i++) {
		Ball* ball = _balls[i];
		glPushMatrix();
		glTranslatef(ball->pos[0], ball->pos[1], ball->pos[2]);
		glColor3f(ball->color[0], ball->color[1], ball->color[2]);
		glutSolidSphere(ball->r, 12, 12); //Draw a sphere
		glPopMatrix();
	}
	*/
	glutSwapBuffers();
}


const int TIMER_MS = 25; //The number of milliseconds to which the timer is set
//Called every TIMER_MS milliseconds
void update(int value) {
	

  /*
  advance(_balls, _octree, (float)TIMER_MS / 1000.0f, _timeUntilUpdate);
	_angle += (float)TIMER_MS / 100;
	if (_angle > 360) {
		_angle -= 360;
	}
	
  */
	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}