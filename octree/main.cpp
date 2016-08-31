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

#define BOX_SIZE 12
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
 Ball testBall;
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
/*
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
 */
class Point{


public:

  float x,y,z;

  Point(){
    
  }
  
  Point(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
  } 
///ne znamm :( WHY??

Point operator+(Point p2){
  float x_,y_,z_;
   
   x_ = x + p2.x;
   y_= y + p2.y;
   z_ = z + p2.z;
   
   Point result=Point(x_,y_,z_);
   
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
void drawOctree(Node *pTree);
Node *BuildOctree(Point center, float halfWidth, int stopDepth)
{
    if (stopDepth < 0)return NULL;
      
    else {
      // Construct and fill in â€™rootâ€™ of this subtree
	Node *pNode = new Node;
	pNode->center = center;
	pNode->halfWidth = halfWidth;
	pNode->pObjList = NULL;

      // Recursively construct the eight children of the subtree
      Point offset;//Point(0,0,0);

      float step = halfWidth * 0.5f;
      
	  for (int i = 0; i < 8; i++) {
	    offset.x= ((i & 1) ? step : -step);
	    offset.y= ((i & 2) ? step : -step);
	    offset.z= ((i & 4) ? step : -step);
	    
	    cout<<"offset: "<<offset.x<<" "<<offset.y<<" "<<offset.z<<endl;
	    pNode->pChild[i] = BuildOctree(center+offset, step, stopDepth - 1);
	  }

      return pNode;
    }
}

void InsertObject(Node *pTree, Ball *pObject)
{
 //cout<<root.center;
    
    int index = 0, straddle = 0;
// Compute the octant number [0..7] the object sphere center is in
// If straddling any of the dividing x, y, or z planes, exit directly
  
    for (int i=0; i< 3; i++){
   
    float delta = pObject->pos[i] - pTree->center.x;
    cout<<pTree->halfWidth<<endl;
     
      if(abs(delta) < pTree->halfWidth + pObject->r){
	//cout<<"i:"<<i<<endl;
	 //cout<<"pos:"<<pObject->pos[0]<<" "<<pObject->pos[1]<<" "<<pObject->pos[2]<<endl;
	straddle = 1;
	cout<<i;
	break;
      }
      
      if(delta > 0.0f) index |= (1<<i);
      //ZYX ubaci 1 na Z X ili Y mjesto u indexu (little-endian format)
      //index nije polje, vec int- a ovime mijenjamo njegovu binarnu vrijednos
    }
    cout<<":"<<straddle<<endl;
    
    if(!straddle){
        
    //cout<<"pos:"<<pObject->pos[0]<<" "<<pObject->pos[1]<<" "<<pObject->pos[2]<<endl;
    //cout<<"str:"<<straddle<<endl;
    //cout<<"indx:"<<index<<endl;
    //cout<<"hw:"<<pTree->halfWidth<<endl;
      
      if(pTree->pChild[index] == NULL){
	//cout<<"case 2 ";
	  //pTree = new Node;
	  pTree = BuildOctree(pTree->center,pTree->halfWidth,1);
	 //pTree->pChild[index]->pChild[0]=NULL;
	InsertObject(pTree->pChild[index], pObject);
	  cout<<"center:"<<pTree->center.x<<" "<<pTree->center.y<<" "<<pTree->center.z;
	 cout<<"center:"<<pTree->pChild[0]->center.x<<" "<<pTree->pChild[0]->center.y<<" "<<pTree->pChild[0]->center.z;
      
	
      }else{
	  
	//InsertObject(pTree->pChild[index], pObject);
	  //cout<<"case 4 ";
	
      }
	
	
    }else{
      //postavi ga kao prvog u listi!!!
      pObject->next = pTree->pObjList;
      pTree->pObjList = pObject;
    }
    /*
    cout<<"pos:"<<pObject->pos[0]<<" "<<pObject->pos[1]<<" "<<pObject->pos[2]<<endl;; 
    cout<<"str:"<<straddle<<endl;
    cout<<"indx:"<<index<<endl;
    cout<<"hw:"<<pTree->halfWidth<<endl;*/
  
}

// Tests all objects that could possibly overlap due to cell ancestry and coexistence
// in the same cell. Assumes objects exist in a single cell only, and fully inside it
void TestAllCollisions(Node *pTree)
{
    cout<<"tutut";
// Keep track of all ancestor object lists in a stack
    const int MAX_DEPTH = 40;
    static Node *ancestorStack[MAX_DEPTH];
    static int depth = 0; // ’Depth == 0’ is invariant over calls
// Check collision between all objects on this level and all
// ancestor objects. The current level is included as its own
// ancestor so all necessary pairwise tests are done
    ancestorStack[depth++] = pTree;
    for (int n = 0; n < depth; n++) {
    Ball *pA, *pB;
    for (pA = ancestorStack[n]->pObjList; pA; pA = pA->next) {
	for (pB = pTree->pObjList; pB; pB = pB->next) {
// Avoid testing both A->B and B->A
	  if (pA == pB) break;
// Now perform the collision test between pA and pB in some manner
	  //////TestCollision(pA, pB);
	  }
	}
    }
// Recursively visit all existing children
    for (int i = 0; i < 8; i++)
    if (pTree->pChild[i]) 
      TestAllCollisions(pTree->pChild[i]);

  // Remove current node from ancestor stack before returning
  depth--;
}



void drawOctree(Node *pTree){
  
  //cout<<"dO:"<<pTree->halfWidth<<endl;
 
  if(pTree->pChild[0]!= NULL){
    //cout<<"s djecom"<<endl;
    for(int i=0; i < 8;i++){
      drawOctree(pTree->pChild[i]);
	//glColor3f(0,1,1);
	//glutWireCube(pTree->pChild[i]->halfWidth);
      
    }
    
  }else if(pTree->pChild[0] == NULL){
    
    //cout<<"bez djece"<<endl;
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(0,1,0);
    glTranslatef(pTree->center.x-pTree->halfWidth,pTree->center.y-pTree->halfWidth,pTree->center.z-pTree->halfWidth);
    glutWireCube(pTree->halfWidth);
    //glutWireCube(1);
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
  
    glPushMatrix();
    
    ball->pos+=ball->v * dt;
    cout <<"moved"<<endl;
    glPopMatrix();
  
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

int check_wall_collision(BallWallPair pair){
  
  
  Ball *b = pair.ball;
  Wall *w = pair.wall;
  
  Vec3f dir = w->direction;
  
  if( b->pos.dot(dir) + b->r >= BOX_SIZE / 2 && b->v.dot(dir) > 0){
   return 1;
  }
   return 0;
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
        { /*
           move_ball(&ball1,t);
	   InsertObject(&root,&ball1);
	   move_ball(&ball2,t);
	   InsertObject(&root, &ball2);
	   check_collision(bp);
	   */
	    InsertObject(&root, &testBall);

	   //if( check_wall_collision(bw))w_responce(bw);
	   //if(check_wall_collision(bw2))w_responce(bw2);	 
	   
	}
   
   if(!flag)
    {
	  responce(bp);  
    }
    
    
	glutPostRedisplay();

	glutTimerFunc(TIMER_MS, update, 0);
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
   // glTranslatef(0.0, 0.0, -(dis+ddis));

    glTranslatef(0.0, 0.0, -60.0f);

    glRotated(elev+delev, 1.0, 0.0, 0.0);
    glRotated(azim+dazim, 0.0, 1.0, 0.0);

    Lighting(BOX_SIZE);
    glShadeModel(GL_SMOOTH);
    
    glColor3f(1,1,1);
   // drawCube(BOX_SIZE);

 //   InsertObject(&root,&ball1);
//    InsertObject(&root,&ball2);
    
    //drawBall(ball1);
    //drawBall(ball2);
    drawBall(testBall);
     
    drawOctree(&root);
    drawCube(BOX_SIZE/2);
    glutSwapBuffers();
}
 void init(){
     
   //ball 1
    ball1.pos=Vec3f(3,3,3);
    ball1.v=Vec3f(0,2,0);  
    ball1.r=0.05f;
    ball1.color= Vec3f(1,1,0);
    
    //ball 2
    ball2.pos=Vec3f(-2,-2,-2);
    ball2.v=Vec3f(0,-2,0);  
    ball2.r=0.7f;
    ball2.color= Vec3f(1,0,0);
    
    //test ball
 
    testBall.pos=Vec3f(3,0,0);
    testBall.r=0.5;
    testBall.color=Vec3f(0,1,0);
    
    bp.bA = &ball1;
    bp.bB = &ball2;
   
    bottom.direction = Vec3f(0, -1, 0);
    top.direction = Vec3f(0, 1, 0);
   
    bw.ball = &ball2;
    bw.wall = &bottom;
 
    bw2.ball = &ball1;
    bw2.wall = &top;
    
    root.center=Point(0,0,0);//radi
    root.halfWidth=12;
    //root.center=Point(root.halfWidth,0,0);
    
   // BuildOctree(root.center,root.halfWidth,1);   

}
  
void handleKeypress2(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			//TwTerminate();
			exit(0);
		case 'w': testBall.pos+=Vec3f(0,0.1f,0);
		      break;
		case 'a': testBall.pos+=Vec3f(-0.1f,0,0);
		       break;
		case 'd': testBall.pos+=Vec3f(0.1f,0,0);
		      break;
		case 's': testBall.pos+=Vec3f(0,-0.1,0);
		      break;
		case 'e' : testBall.pos+=Vec3f(0,0,-0.1);
		      break;
		case 'y' : testBall.pos+=Vec3f(0,0,0.1);
		      break;
		case 'r' : testBall.pos=Vec3f(0,0,0);
		      break;
		      
	  
	}
}	
int main(int argc,char **argv)
{
    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);

    glutInitWindowSize(600, 600);

    glutCreateWindow("Collision Window");

    initRendering();

    init();

    glutDisplayFunc(display);

    glutIdleFunc(display);
    
    glutKeyboardFunc(handleKeypress2);
    
    glutReshapeFunc(handleResize);
    glutMouseFunc(cb_mouse);
    glutMotionFunc(cb_motion);
  
    glutTimerFunc(TIMER_MS, update, 0);
	

    glutMainLoop();

    return(0);
}
