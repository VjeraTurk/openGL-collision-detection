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
const float GRAVITY =  0.0f;
using namespace std;

int selection=1;
int flag=1; //colide = 0 don't colide =-1;
int w_flag=1; //colide = 0 don't colide =-1;
float randomFloat() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

struct Ball{
  Vec3f pos; // position
  Vec3f v; //velocity with direction
  
  float r;
  Vec3f color;
  
  Ball* next;
};
struct triangle
{
    vector3f v0;
    vector3f v1;
    vector3f v2;
    vector3f vNormal;

    // Bounding sphere
    vector3f vCenter;
    float    fRadius;
    Ball *ball;
    
};
triangle g_tri1;
triangle g_tri2;

void createBoundingSphere( triangle *tri )
{
    float fMinX;
    float fMinY;
    float fMinZ;

    float fMaxX;
    float fMaxY;
    float fMaxZ;

    float fRadius1;
    float fRadius2;

    fMinX = fMaxX = tri->v0.x;
    fMinY = fMaxY = tri->v0.y;
    fMinZ = fMaxZ = tri->v0.z;

    if( tri->v1.x < fMinX ) fMinX = tri->v1.x;
    if( tri->v2.x < fMinX ) fMinX = tri->v2.x;
    if( tri->v1.y < fMinY ) fMinY = tri->v1.y;
    if( tri->v2.y < fMinY ) fMinY = tri->v2.y;
    if( tri->v1.z < fMinZ ) fMinZ = tri->v1.z;
    if( tri->v2.z < fMinZ ) fMinZ = tri->v2.z;

    if( tri->v1.x > fMaxX ) fMaxX = tri->v1.x;
    if( tri->v2.x > fMaxX ) fMaxX = tri->v2.x;
    if( tri->v1.y > fMaxY ) fMaxY = tri->v1.y;
    if( tri->v2.y > fMaxY ) fMaxY = tri->v2.y;
    if( tri->v1.z > fMaxZ ) fMaxZ = tri->v1.z;
    if( tri->v2.z > fMaxZ ) fMaxZ = tri->v2.z;

    tri->vCenter.x = (fMinX + fMaxX) / 2;
    tri->vCenter.y = (fMinY + fMaxY) / 2;
    tri->vCenter.z = (fMinZ + fMaxZ) / 2;
    
    //my add
    tri->ball->pos[0] = (fMinX + fMaxX) / 2;
    tri->ball->pos[1] = (fMinY + fMaxY) / 2;
    tri->ball->pos[2] = (fMinZ + fMaxZ) / 2;

    fRadius1 = sqrt( ((tri->vCenter.x - tri->v0.x) * (tri->vCenter.x - tri->v0.x)) +
                     ((tri->vCenter.y - tri->v0.y) * (tri->vCenter.y - tri->v0.y)) +
                     ((tri->vCenter.z - tri->v0.z) * (tri->vCenter.z - tri->v0.z)) );

    fRadius2 = sqrt( ((tri->vCenter.x - tri->v1.x) * (tri->vCenter.x - tri->v1.x)) +
                     ((tri->vCenter.y - tri->v1.y) * (tri->vCenter.y - tri->v1.y)) +
                     ((tri->vCenter.z - tri->v1.z) * (tri->vCenter.z - tri->v1.z)) );

    if( fRadius1 < fRadius2 )
        fRadius1 = fRadius2;

    fRadius2 = sqrt( ((tri->vCenter.x - tri->v2.x) * (tri->vCenter.x - tri->v2.x)) +
                     ((tri->vCenter.y - tri->v2.y) * (tri->vCenter.y - tri->v2.y)) +
                     ((tri->vCenter.z - tri->v2.z) * (tri->vCenter.z - tri->v2.z)) );

    if( fRadius1 < fRadius2 )
		fRadius1 = fRadius2;
   
    tri->fRadius = fRadius1;

    tri->ball->r = fRadius1;//my add
    glColor3f( 0.0f, 0.0f, 1.0f );
    
    g_tri1.ball->v=Vec3f(-2,0,0);  
    g_tri1.ball->color= Vec3f(1,0.5,1);
    
    return;

  
}

bool isPointInsideTriangle( vector3f *vIntersectionPoint, triangle *tri )
{
	vector3f vVectors[3];
	float fTotalAngle = 0.0f; // As radians

	//
	// Create and normalize three vectors that radiate out from the
	// intersection point towards the triangle's three vertices.
	//
	vVectors[0] = *vIntersectionPoint - tri->v0;
	vVectors[0].normalize();

	vVectors[1] = *vIntersectionPoint - tri->v1;
	vVectors[1].normalize();

	vVectors[2] = *vIntersectionPoint - tri->v2;
	vVectors[2].normalize();

	//
	// We then sum together the angles that exist between each of the vectors.
	//
	// Here's how:
	//
	// 1. Use dotProduct() to get cosine of the angle between the two vectors.
	// 2. Use acos() to convert cosine back into an angle.
	// 3. Add angle to fTotalAngle to keep track of running sum.
	//

	fTotalAngle  = acos( dotProduct( vVectors[0], vVectors[1] ) );
	fTotalAngle += acos( dotProduct( vVectors[1], vVectors[2] ) );
	fTotalAngle += acos( dotProduct( vVectors[2], vVectors[0] ) );

	//
	// If we are able to sum together all three angles and get 360.0, the
	// intersection point is inside the triangle.
	//
	// We can check this by taking away 6.28 radians (360 degrees) away from
	// fTotalAngle and if we're left with 0 (allowing for some tolerance) the
	// intersection point is definitely inside the triangle.
	//

	if( fabsf( fTotalAngle - 6.28f ) < 0.01f )
		return true;

	return false;
}

bool getLinePlaneIntersectionPoint( vector3f *vLineStart, vector3f *vLineEnd,
				                    vector3f *vPointInPlane, vector3f *vPlaneNormal,
				                    vector3f *vIntersection )
{
	vector3f vDirection;
	vector3f L1;
	float	 fLineLength;
    float    fDistanceFromPlane;
	float    fPercentage;

	vDirection.x = vLineEnd->x - vLineStart->x;
	vDirection.y = vLineEnd->y - vLineStart->y;
	vDirection.z = vLineEnd->z - vLineStart->z;

	fLineLength = dotProduct( vDirection, *vPlaneNormal );

	// Check the line's length allowing for some tolerance for floating point
	// rounding errors. If it's 0 or really close to 0, the line is parallel to
	// the plane and can not intersect it.
	if( fabsf( fLineLength ) < 0.001f )
        return false;

	L1.x = vPointInPlane->x - vLineStart->x;
	L1.y = vPointInPlane->y - vLineStart->y;
	L1.z = vPointInPlane->z - vLineStart->z;

	fDistanceFromPlane = dotProduct( L1, *vPlaneNormal );

	// How far from Linestart , intersection is as a percentage of 0 to 1
	fPercentage	= fDistanceFromPlane / fLineLength;

	if( fPercentage < 0.0f || // The plane is behind the start of the line
		fPercentage > 1.0f )  // The line segment does not reach the plane
        return false;

	// Add the percentage of the line to line start
	vIntersection->x = vLineStart->x + vDirection.x * fPercentage;
	vIntersection->y = vLineStart->y + vDirection.y * fPercentage;
	vIntersection->z = vLineStart->z + vDirection.z * fPercentage;

	return true;
}


bool doSpheresIntersect( triangle *tri1, triangle *tri2 )
{
    float fDistance = tri1->fRadius + tri2->fRadius;
	float fRadius;

    fRadius = sqrt( ((tri2->vCenter.x - tri1->vCenter.x) * (tri2->vCenter.x - tri1->vCenter.x)) +
                    ((tri2->vCenter.y - tri1->vCenter.y) * (tri2->vCenter.y - tri1->vCenter.y)) +
                    ((tri2->vCenter.z - tri1->vCenter.z) * (tri2->vCenter.z - tri1->vCenter.z)) );

    if( fRadius < fDistance )
        return true;

    else
        return false;
}
bool doTrianglesIntersect( triangle tri1, triangle tri2 )
{
	bool bIntersect = false;
	vector3f vPoint;

	//
	// Create a normal for 'tri1'
	//

	vector3f vEdgeVec1 = tri1.v1 - tri1.v0;
	vector3f vEdgeVec2 = tri1.v2 - tri1.v0;
	tri1.vNormal = crossProduct( vEdgeVec1, vEdgeVec2 );
	
	tri1.vNormal.normalize(); // Some people feel compelled to normalize this, but it's not really necessary.

	//
	// Check the first line segment of triangle #2 against triangle #1
    //
    // The first line segment is defined by vertices v0 and v1.
	//

	bIntersect = getLinePlaneIntersectionPoint( &tri2.v0,      // Line start
		                                    &tri2.v1,      // Line end
						    &tri1.v0,      // A point in the plane
						    &tri1.vNormal, // The plane's normal
						    &vPoint );     // Holds the intersection point, if the function returns true

	if( bIntersect == true )
	{
		//
		// The line segment intersects the plane, but does it actually go 
		// through the triangle?
		//

		if( isPointInsideTriangle( &vPoint, &tri1 ) == true )
			return true;
	}

    //
	// Check the second line segment of triangle #2 against triangle #1
    //
    // The second line segment is defined by vertices v1 and v2.
	//

	bIntersect = getLinePlaneIntersectionPoint( &tri2.v1,      // Line start
		                                        &tri2.v2,      // Line end
				                                &tri1.v0,      // A point in the plane
							                    &tri1.vNormal, // The plane's normal
				                                &vPoint );     // Holds the intersection point, if the function returns true

	if( bIntersect == true )
	{
		//
		// The line segment intersects the plane, but does it actually go 
		// through the triangle?
		//

		if( isPointInsideTriangle( &vPoint, &tri1 ) == true )
			return true;
	}

    //
	// Check the third line segment of triangle #2 against triangle #1
    //
    // The third line segment is defined by vertices v2 and v0.
	//

	bIntersect = getLinePlaneIntersectionPoint( &tri2.v2,      // Line start
		                                        &tri2.v0,      // Line end
				                                &tri1.v0,      // A point in the plane
							                    &tri1.vNormal, // The plane's normal
				                                &vPoint );     // Holds the intersection point, if the function returns true

	if( bIntersect == true )
	{
		//
		// The line segment intersects the plane, but does it actually go 
		// through the triangle?
		//
		
		if( isPointInsideTriangle( &vPoint, &tri1 ) == true )
			return true;
	}

    return false;
}



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




 /*
 struct Node {
Point center;
// Center point of octree node (not strictly needed)
float halfWidth;
// Half the width of the node volume (not strictly needed)
Node *pChild[8];
// Pointers to the eight children nodes
Object *pObjList; // Linked list of objects contained at this node
};
  */
class Octree {
  /*
-Each node has a bounding region which defines the enclosing region
-Each node has a reference to the parent node
-Contains an array of eight child nodes (use arrays for code simplicity and cache performance)

-Contains a list of objects contained within the current enclosing region
I use a byte-sized bitmask for figuring out which child nodes are actively 
being used (the optimization benefits at the cost of additional complexity is somewhat debatable)

*/
  
	//hash value
  	Vec3f corner1; //(minX, minY, minZ)
	Vec3f corner2; //(maxX, maxY, maxZ)
	Vec3f center;//((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2)
	
	Octree *children[2][2][2];
	//Only leaf nodes have balls in them, they have no children
	Ball* balls = NULL;//pointer na 1. od svih lopti oktana
	
   	int depth;
	int numBalls;
//hash tablica svih oktana?
 private:	
	void placeBall(Ball* ball){
	  
	  for(int x=0;x<2;x++){
	     
	    if(ball->pos[0] - ball->r < center[0]){
	      continue;
	    }
	    
	    
	  }
	
	  
	}
	
	 
};

    
void drawTriangle(triangle tri){
  
  glColor3f( 1.0f, 0.0f, 0.0f );
  glPushMatrix();
     //glTranslatef(tri.ball->pos[0],tri.ball->pos[1], tri.ball->pos[2]);
   
  glPushMatrix();
     glTranslatef(tri.ball->pos[0],tri.ball->pos[1], tri.ball->pos[2]);
    // glTranslatef( g_tri1.vCenter.x, g_tri1.vCenter.y, g_tri1.vCenter.z );
    glBegin(GL_POLYGON);
      {
	      glVertex3f(tri.v0.x, tri.v0.y, tri.v0.z );
	      glVertex3f( tri.v1.x, tri.v1.y, tri.v1.z );
	      glVertex3f( tri.v2.x, tri.v2.y, tri.v2.z );
      } glEnd();
    glPopMatrix();
   
    glTranslatef( g_tri1.vCenter.x, g_tri1.vCenter.y, g_tri1.vCenter.z );
    glPushMatrix();
      glTranslatef(tri.ball->pos[0],tri.ball->pos[1], tri.ball->pos[2]);
      glutWireSphere(g_tri1.ball->r,16,16);  
    glPopMatrix();
    
      
 glPopMatrix();  	  
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
  
    ball->pos+=ball->v * dt;
    //cout << ball->pos[0]<<endl;
  
}

void pullBall(Ball *ball){
  ball->v-= Vec3f(0, GRAVITY * TIME_BETWEEN_UPDATES, 0);
}

void move_triangle(triangle *tr,float dt){
 /*
  tr->v0 += tr->ball->v * dt;
  tr->v1 += tr->ball->v * dt;
  tr->v2 += tr->ball->v * dt;
  */
  
}

void check_collision(BallPair pair){
   
   Ball *bA= pair.bA;
   Ball *bB= pair.bB;
   
   float r = bA->r + bB->r;
 
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
Ball* AllBalls;


void applyGravity(Ball *head) {
  Ball *temp;
  for(temp=head; temp!=NULL; temp= temp->next){
    pullBall(temp);
  }

}
void moveBalls(Ball *head){
    Ball *temp=NULL;
 temp=head;
 for(temp=head; temp!=NULL; temp= temp->next){ 
   move_ball(temp,t);
 }
}

/*
  
int print_list(node *head) {
    node *trenutni;
 
    if(!head) return -1;
    for(trenutni=head; trenutni!=NULL; trenutni=trenutni->next)
        printf("%c ", trenutni->val);
    printf("\n");
    return 0;
}
 */
void drawBalls(Ball *head){
  
  Ball *temp;
  
 for(temp=head; temp!=NULL; temp = temp->next){
    drawBall(*temp);
  }

}


void update(int value) {
    //cout<<flag<<endl;
  
    switch(selection){
      
      case 1:
	
	    if(flag && w_flag)
        {
           move_ball(&ball1,t);
	   //move_triangle(&g_tri2,t);
	   move_ball(&ball2,t);
	   
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
	  pullBall(&ball1);
	  pullBall(&ball2);
	break;
 
      case 2:
	  moveBalls(AllBalls);
	  applyGravity(AllBalls);
	 break;
	
    }
    
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void elastic_collision(){
  
    drawBall(ball1); 
    drawBall(ball2); 
    
  
}
void multiple_balls_collision(){
  
     drawBalls(AllBalls);
    
  
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
   
    switch(selection){
      
      case 1 : elastic_collision();
		 break;
      case 2 : multiple_balls_collision();
		break;
    }

    
    //drawBall(*g_tri1.ball);
    //drawTriangle(g_tri1);
   
 
    glutSwapBuffers();
}

Ball *create_list(int max){   
    Ball* head;
    Ball* temp=NULL;
    Ball* ball;
    
  for(int i = 0; i < max; i++) {
        ball = new Ball();
   
      ball->pos = Vec3f(8 * randomFloat() - 4, 8 * randomFloat() - 4, 8 * randomFloat() -4);
      //ball->pos = Vec3f(0,0, 8 * randomFloat() -4);
      ball->v = Vec3f(8 * randomFloat() - 4,8 * randomFloat() - 4, 8 * randomFloat() - 4);
      ball->r = 0.1f * randomFloat() + 0.1f;
      ball->color = Vec3f(0.6f * randomFloat() + 0.2f, 0.6f * randomFloat() + 0.2f, 0.6f * randomFloat() + 0.2f);
  
      ball->next=NULL;
      
      if(!temp)head=ball;
      
      else  temp->next = ball;
      temp = ball;
     cout<<ball->pos[3]<<endl;
  }
  return head;
}

 void init(){
   /* 
   //triangle 1
    g_tri1.v0.x =  0.1f;
    g_tri1.v0.y =  0.2f;
    g_tri1.v0.z =  0.5f;
    
    g_tri1.v1.x =  0.5f;
    g_tri1.v1.y =  0.5f;
    g_tri1.v1.z =  0.5f;
    
    g_tri1.v2.x =  1.2f;
    g_tri1.v2.y =  0.5f;
    g_tri1.v2.z =  0.5f;
    
   // g_tri1.vNormal = vector3f( 0.0f, 0.0f, 0.0f );
  
    g_tri1.vNormal = vector3f( 1.0f, 1.0f, 1.0f );
   
    //triangle ball
    g_tri1.ball=&ball2;
    createBoundingSphere( &g_tri1 );   
    */
   
   //ball 1
    ball1.pos=Vec3f(-2,0.4,0);
    ball1.v=Vec3f(0,-5,0);  
    
    ball1.r=0.4f;
    ball1.color= Vec3f(1,0,0);
    
    //ball 2
    ball2.pos=Vec3f(2,0.4,0);
    ball2.v=Vec3f(0,5,0);  
   
    ball2.r=0.4f;
    ball2.color= Vec3f(1,1,0);
    
    
    bp.bA = &ball1;
    bp.bB = &ball2;
   
    bottom.direction = Vec3f(0, -1, 0);
    top.direction = Vec3f(0, 1, 0);
   
 
    bw.ball = &ball2;
    bw.wall = &bottom;

    bw2.ball = &ball1;
    bw2.wall = &top;


    AllBalls=create_list(40);

}
 
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			//TwTerminate();
			exit(0);
		case '1': selection=1;
			break;
		case '2':selection=2;
	}
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

