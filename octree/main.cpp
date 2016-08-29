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
class Point{

private:
    float p[3];
  
 
    
public:
  
  Point(){
    
  }
  
  Point(float x, float y, float z) {
	p[0] = x;
	p[1] = y;
	p[2] = z;
  } 
///ne znamm :( WHY??
 float operator[](int index) const{
   	return p[index];
  }
  
 float& operator[](int index) {
	return p[index];
  }
  
  
Point operator+(Point p2){
  float x,y,z;
   
   x = p[0] + p2[0];
   y= p[1] + p2[1];
   z = p[2] + p2[2];
   
   Point result=Point(x,y,z);
   
   return result;
}

};
 
struct Node {
  Point center;
// Center point of octree node (not strictly needed)
float halfWidth;
//Vec3f corner1; //(minX, minY, minZ)
//Vec3f corner2; //(maxX, maxY, maxZ)

Node *pChild[8];
// Pointers to the eight children nodes
Ball *pObjList; // Linked list of objects contained at this node
};
 Node root;
 
Node *BuildOctree(Point center, float halfWidth, int stopDepth)
{
    if (stopDepth < 0) return NULL;

  else {
      // Construct and fill in ’root’ of this subtree
	Node *pNode = new Node;
	pNode->center = center;
	pNode->halfWidth = halfWidth;
	pNode->pObjList = NULL;

      // Recursively construct the eight children of the subtree
      Point offset=Point(0,0,0);

      float step = halfWidth * 0.5f;
	  for (int i = 0; i < 8; i++) {
	    offset[0]= ((i & 1) ? step : -step);
	    offset[1]= ((i & 2) ? step : -step);
	    offset[2]= ((i & 4) ? step : -step);
	    
	    pNode->pChild[i] = BuildOctree(center+offset, step, stopDepth - 1);
	  }
     
      return pNode;
    }
}
//malo razmislii o straddle, nešto tu ne miriši dobro, inace, lajkam kod
void InsertObject(Node *pTree, Ball *pObject)
{
    int index = 0, straddle = 0;
// Compute the octant number [0..7] the object sphere center is in
// If straddling any of the dividing x, y, or z planes, exit directly
    for (int i=0; i< 3; i++){
   
    float delta = pObject->pos[i] - pTree->center[i];

      if(abs(delta)< pTree->halfWidth+pObject->r){
	   straddle=1;
	  break;
      }
      //cout<<straddle<<endl;
      if(delta>0.0f) index |= (1<<i);
      //ZYX ubaci 1 na Z X ili Y mjesto u indexu (little-endian format)
      //index nije polje, vec int- a ovime mijenjamo njegovu binarnu vrijednost
      //sve u svrhu očuvanja memorije
    }
    
    if(!straddle && pTree->pChild[index]){
      // Fully contained in existing child node; insert in that subtree
      InsertObject(pTree->pChild[index],pObject);
     
      
    }else if(!straddle){ //sa if(straddle) "kao" radi straddle sumnjiv!
      //ovo dodano prema knjizi, na dobro mijesto nadam se, jer piše, else{//as before...}
      if (pTree->pChild[index] == NULL) {
      pTree->pChild[index] = new Node;
      Node *pNode = new Node;
      pNode->center = pTree->center;
      pNode->halfWidth = pTree->halfWidth*0.5;
      pNode->pObjList = NULL;
      glutWireCube(pNode->halfWidth);
      //...initialize node contents here...
	}
      InsertObject(pTree->pChild[index], pObject);
    }else{
      // Straddling, or no child node to descend into, so
      // link object into linked list at this node
      pObject->next = pTree->pObjList;
      pTree->pObjList = pObject;

    }
    cout<<index<<endl;

    cout<<pTree->halfWidth<<endl;
}
void drawOctree(Node pTree){
  
  //cout<<pTree.halfWidth<<endl;
 
  if(pTree.pChild[0] != NULL){
    
    for(int i=0; i < 8;i++){
      drawOctree(*pTree.pChild[i]);
    }
    
  }else if(pTree.pChild == NULL){
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(0,1,0);
    glTranslatef(pTree.center[0],pTree.center[1],pTree.center[2]);
    //glutWireCube(pTree.halfWidth);
    glutWireCube(1);
    glEnable(GL_LIGHTING);
    glPopMatrix();
  }
  

}


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
   //cout<<r<<endl;
 
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
  
  
  Ball *b = pair.ball;
  Wall *w = pair.wall;
  
  Vec3f dir = w->direction;
  
  if( b->pos.dot(dir) + b->r >= BOX_SIZE / 2 && b->v.dot(dir) > 0){
   w_flag=0;
  }
  
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
           //move_ball(&ball1,t);
	   //move_ball(&ball2,t);
	 
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

    InsertObject(&root,&ball1);
    
    InsertObject(&root,&ball2);
    
    drawBall(ball1);
    drawBall(ball2);
     
    drawOctree(root);
    
    glutSwapBuffers();
}
 void init(){
     
   //ball 1
    ball1.pos=Vec3f(0,0,0);
    ball1.v=Vec3f(-4,0,0);  
    ball1.r=0.4f;
    ball1.color= Vec3f(1,1,0);
    
    //ball 2
    ball2.pos=Vec3f(-1,0,1);
    ball2.v=Vec3f(2,0,0);  
    ball2.r=0.3f;
    ball2.color= Vec3f(1,0,0);
    
 
    bp.bA = &ball1;
    bp.bB = &ball2;
   
    bottom.direction = Vec3f(0, -1, 0);
    top.direction = Vec3f(0, 1, 0);
   
    bw.ball = &ball2;
    bw.wall = &bottom;
 
    bw2.ball = &ball1;
    bw2.wall = &top;
    
    root.center=Point(2,2,2);
    Node *pRoot;
    
    pRoot= BuildOctree(root.center, 24.0f, MAX_OCTREE_DEPTH);

       

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

