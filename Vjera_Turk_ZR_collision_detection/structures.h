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

