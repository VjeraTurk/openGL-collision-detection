#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <vector>
#include <GL/glut.h>

#include <set>
#include "vec3f.h"


struct Ball{
  Vec3f pos; // position
  Vec3f v;  // velocity vector
  
  float m; // ball mass
  float t_red;
  
  Ball* next;
 
  float r; //bal radius
  Vec3f color; 
};

struct BallPair{
  
  Ball* bA;
  Ball* bB;
  
  BallPair* next;
};

struct Wall{
  
      Vec3f direction;
      float t_red;
};

struct BallWallPair {
	Ball* ball;
	Wall* wall;
};

struct exPos{
  Vec3f pos;
  Vec3f *next;
};
//////////////////////////////////////////


///////////////////////////////////////////

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

