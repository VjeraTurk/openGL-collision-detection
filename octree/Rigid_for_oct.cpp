#include <stdlib.h>
#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
//#include <AntTweakBar.h>

#include "vector3f.h"
#include "vec3f.h"
#include "cube.h"
#include "lighting.h"
#include "navigation.h"

#define BOX_SIZE 6
#define MAX_OCTREE_DEPTH 6
using namespace std;

int flag=1; //colide = 0 don't colide =-1;
int w_flag=1; //colide = 0 don't colide =-1;

struct Ball{
  Vec3f pos; // position
  Vec3f v; //velocity with direction
  
  float r;
  Vec3f color;
  
  Ball* next;
};



struct BallPair{
  Ball* bA;
  Ball* bB;
  
  BallPair* next;
};

 Ball ball1;
 Ball ball2;
 BallPair bp;

struct Wall{
  Vec3f direction;    
  float size;
  //points
};
  Wall bottom, top;

//Wall front, back, left, right,top, bottom;
  
struct BallWallPair {
	Ball* ball;
	Wall* wall;
};

 BallWallPair bw, bw2;
// hash table 0000 0001 0010 0011 ... lista? Kako




void drawBall(Ball ball)
{
    glColor3f(ball.color[0],ball.color[1], ball.color[2]);

    glPushMatrix();
    
    glTranslatef(ball.pos[0],ball.pos[1],ball.pos[2]);
    glutSolidSphere(ball.r,20,20);
  
    glPopMatrix();
    
}
const float TIME_BETWEEN_UPDATES = 0.001f;
const int TIMER_MS = 25; //The number of milliseconds to which the timer is set
const float t =0.005;

void move_ball(Ball *ball, float dt){
  
   // glPushMatrix();
    ball->pos+=ball->v * dt;
    //cout << ball->pos[0]<<endl;
    //glPopMatrix();
  
}


void check_collision(BallPair pair){
   
   Ball *bA= pair.bA;
   Ball *bB= pair.bB;
   
   float r = bA->r + bB->r;
   cout<<r<<endl;
 
   if ((bA->pos - bB->pos).magnitudeSquared() < r*r){ 
    flag=0;
     
  }else flag=1;

}


void responce(BallPair pair){
   Ball *bA = pair.bA;
   Ball *bB = pair.bB;
  
   Vec3f displacement = (bA->pos - bB->pos).normalize();
  bA->v -= 2 * displacement * bA->v.dot(displacement);
  bB->v -= 2 * displacement * bB->v.dot(displacement);
  flag=1;
}

void check_wall_collision(BallWallPair pair){
  
  /*
  Ball *b = pair.ball;
  Wall *w = pair.wall;
  
  Vec3f dir = w->direction;
  
  if( b->pos.dot(dir) + b->r >= BOX_SIZE / 2 && b->v.dot(dir) > 0){
   w_flag=0;
  }
  */
}

void w_responce(BallWallPair pair){
   Ball *b = pair.ball;
   Wall *w = pair.wall;
  
  Vec3f dir = w->direction.normalize();
  b->v -= 2 * dir * b->v.dot(dir);
  
  w_flag = 1;
}

void update(int value) {
    //cout<<flag<<endl;
     if(flag && w_flag)
        {
           move_ball(&ball1,t);
	   move_ball(g_tri1.ball,t);
	   check_collision(bp);
	   check_wall_collision(bw);
           check_wall_collision(bw2);
	  
	}
   
   if(!flag)
    {
	  responce(bp);  
    }
    
    if(!w_flag){
	  w_responce(bw);  
	  w_responce(bw2);      
    }
    
    
	glutPostRedisplay();

	glutTimerFunc(TIMER_MS, update, 0);
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    glTranslatef(0.0, 0.0, -(dis+ddis));
    glRotated(elev+delev, 1.0, 0.0, 0.0);
    glRotated(azim+dazim, 0.0, 1.0, 0.0);

    Lighting(BOX_SIZE);
    glShadeModel(GL_SMOOTH);
    glColor3f(1,1,1);
   // drawCube(BOX_SIZE);
    
    drawBall(ball1);
    
    glutSwapBuffers();
}
 void init(){
     
   //ball 1
    ball1.pos=Vec3f(-2,0.4,0);
    ball1.v=Vec3f(4,0,0);  
    ball1.r=0.4f;
    ball1.color= Vec3f(1,1,0);
    
    bp.bA = &ball1;
    bp.bB = &ball2;
   
    bottom.direction = Vec3f(0, -1, 0);
    top.direction = Vec3f(0, 1, 0);
   
    bw.ball = &ball2;
    bw.wall = &bottom;
    

    bw2.ball = &ball1;
    bw2.wall = &top;
}
  
int main(int argc,char **argv)
{
    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);

    glutInitWindowSize(700, 600);

    glutCreateWindow("Collision Window");

    initRendering();

  init();

    glutDisplayFunc(display);

    glutIdleFunc(display);
    
    glutKeyboardFunc(handleKeypress);
    
    glutReshapeFunc(handleResize);
    glutMouseFunc(cb_mouse);
    glutMotionFunc(cb_motion);

  
    glutTimerFunc(TIMER_MS, update, 0);
	

    glutMainLoop();

    return(0);
}

