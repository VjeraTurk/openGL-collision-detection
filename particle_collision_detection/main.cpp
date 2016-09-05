
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <vector>
#include <setjmp.h>
#include <GL/glut.h>


#include <AntTweakBar.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp> 
#include <glm/gtc/random.hpp>

//#include <glm/vec3.hpp> // glm::vec3
//#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
//#include <glm/mat4x4.hpp> // glm::mat4
//#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
//#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "vec3f.h"
#include "cube.h"



using namespace std;

//using namespace glm;
//using glm::mat4;
//using glm::vec3;

float randomFloat() {
	return glm::linearRand(0.0f,1.0f);
}

//const float GRAVITY =  9.80665f;
const float GRAVITY =  0.0f;
const float BOX_SIZE = 12.0f; //The length of one side of the box
//The amount of time between each time that we handle collisions and apply the
//effects of gravity
const float TIME_BETWEEN_UPDATES = 0.01f;
const int TIMER_MS = 25; //The number of milliseconds to which the timer is set
float box_size= BOX_SIZE;



// Particle randomly initialized

float Random() 
{
    return 2.0f * ((float)rand() / RAND_MAX) - 1.0f;
}
/*
struct Particle2
{
    float Size;
    float Position[3];      // [px, py, pz]
    float Speed[3];         // [vx, vy, vz]
    float RotationAxis[3];  // [rx, ry, rz]
    float RotationAngle;    // in degree
    float RotationSpeed;
    float Color[3];         // [r, g, b]
    float Age;
   
    Particle2(float size, float speedDir[3], float speedNorm, float color[3]) // Constructor
    {
        Size = size * (1.0f + 0.2f * Random());
        Position[0] = Position[1] = Position[2] = 0;
        Speed[0] = speedNorm * (speedDir[0] + 0.1f * Random());
        Speed[1] = speedNorm * (speedDir[1] + 0.1f * Random());
        Speed[2] = speedNorm * (speedDir[2] + 0.1f * Random());
        RotationAxis[0] = Random();
        RotationAxis[1] = Random();
        RotationAxis[2] = Random();
        RotationAngle = 360.0f * Random();
        RotationSpeed = 360.0f * Random();
        Color[0] = color[0] + 0.2f * Random();
        Color[1] = color[1] + 0.2f * Random();
        Color[2] = color[2] + 0.2f * Random();
        Age = 0;
    }
    void Update(float dt) // Apply one animation step
    {
        Position[0] += dt * Speed[0];
        Position[1] += dt * Speed[1];
        Position[2] += dt * Speed[2];
        Speed[1] -= dt * 9.81f; // gravity
        RotationAngle += dt * RotationSpeed;
        Age += dt;
    }
};

*/

//Stores information regarding a ball
struct Object {
	glm::vec3 v; //Velocity
	glm::vec3 pos; //Position
	float r; //Radius
};

float _angle = 0.0f; //The camera angle
//The amount of time until performUpdate should be called
float _timeUntilUpdate = 0;


int _pause = 1;
int walls = 1;

int mode;
double beginx, beginy;
double dis = 10.0, azim = 0.0, elev = 0.0;
double ddis = 0.0, dazim = 0.0, delev = 0.0;

void
cb_mouse(int _b, int _s, int _x, int _y)
{
  if (_s == GLUT_UP)
  {
    dis += ddis;
    if (dis < .1) dis = .1;
    azim += dazim;
    elev += delev;
    ddis = 0.0;
    dazim = 0.0;
    delev = 0.0;
    return;
  }

  if (_b == GLUT_RIGHT_BUTTON)
  {
    mode = 0;
    beginy = _y;
    return;
  }
  else
  {
    mode = 1;
    beginx = _x;
    beginy = _y;
  }
}
void
cb_motion(int _x, int _y)
{
  if (mode == 0)
  {
    ddis = dis * (double)(_y - beginy)/200.0;
  }
  else
  {
    dazim = (_x - beginx)/5;
    delev = (_y - beginy)/5;      
  }
  
  glutPostRedisplay();
}
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			TwTerminate();
			exit(0);
		case 'n': _timeUntilUpdate=1;
			  break;
		case 'p': if(_pause == 1) _pause = 0;
			  else if(_pause==0) _pause = 1;
			  break;
		case 'w': if(walls == 1) walls = 0;
			  else walls=1;
			  break;
	
	}
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
}

void initRendering() {
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	//uncomment for Transparent Cube: 
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}
	
void Lightning(){
	
	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPos[] = {BOX_SIZE, BOX_SIZE, -4 * BOX_SIZE, BOX_SIZE}; //in front
	
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	
}
//Called every TIMER_MS milliseconds
void update(int value) {
	if(_pause){
	
	_angle += (float)TIMER_MS / 100;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	glutPostRedisplay();
	  
	  
	}
	glutTimerFunc(TIMER_MS, update, 0);
}
	

void drawScene() {
	
  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//mat4 objectTransformMAtrix = glm::translate(mat4(),vec3(0.0f,0.0f,-10.0f));
	//mat4 projectionMatrix 0 glm::pespective(60.0f,(float),
	glTranslatef(0.0f, 0.0f, -25.0f);
	
	glTranslatef(0.0, 0.0, -(dis+ddis));
	glRotated(elev+delev, 1.0, 0.0, 0.0);
	glRotated(azim+dazim, 0.0, 1.0, 0.0);
	//glRotated(90.0,-1.0,0.0,0.0);
	
	
	Lightning(); //svjetlo ne rotira
	
	//glRotatef(-_angle, 0.0f, 1.0f, 0.0f);


	
	glShadeModel(GL_SMOOTH);

	glPushMatrix();
	glColor3f(1,1,1);
	//glTranslatef(particle.pos[0],particle.pos[1],particle.pos[2]);
	
	

	glutSolidSphere(particle.r,12,12);
	
	glPopMatrix();

	  drawCube(BOX_SIZE);  
	

	TwDraw();
	glutSwapBuffers();
}


int main(int argc, char** argv) {
	srand((unsigned int)time(0)); //Seed the random number generator
 	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(500, 100);
	
	glutInitWindowSize(700, 600);
	
	glutCreateWindow("Collision Detection");
	
	
	TwInit(TW_OPENGL, NULL);
	// or
	//TwInit(TW_OPENGL_CORE, NULL); // for core profile
	TwWindowSize(500, 200);

	 //TwBar *myBar;
	 //myBar = TwNewBar("Collision Detection");

	
	//Polyhedron p;
	//Vertex_iterator v;
	
	//for(v = p.vertices_begin();v != p.vertices_end();v++)
	//std::cout << v->point() << std::endl;
	
	initRendering();

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutMouseFunc(cb_mouse);
	glutMotionFunc(cb_motion);  
	glutReshapeFunc(handleResize);
	glutTimerFunc(TIMER_MS, update, 0);
	
	
	glutMainLoop();
	return 0;
}



