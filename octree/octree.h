#include <stdlib.h>
#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include "structures.h"
#define BOX_SIZE 12
#define MAX_OCTREE_DEPTH 6

using namespace std;

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
	    
	    //cout<<"offset: "<<offset.x<<" "<<offset.y<<" "<<offset.z<<endl;
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
    
     cout<<"delta"<<abs(delta)<<endl;
    cout<<pTree->halfWidth + pObject->r<<endl;
	
      if(abs(delta) < pTree->halfWidth + pObject->r)  {
	//cout<<"i:"<<i<<endl;
	 //cout<<"pos:"<<pObject->pos[0]<<" "<<pObject->pos[1]<<" "<<pObject->pos[2]<<endl;
	
	straddle = 1;
	cout<<"i "<<i<<endl;
	cout<<straddle;
	break;
      }
      
      if(delta > 0.0f) index |= (1<<i);

     /* 
      //ZYX ubaci 1 na Z X ili Y mjesto u indexu (little-endian format)
      //index nije polje, vec int- a ovime mijenjamo njegovu binarnu vrijednos
    }
   // cout<<":"<<straddle<<endl;
    
    if(!straddle){
       // cout<<index;
    //cout<<"pos:"<<pObject->pos[0]<<" "<<pObject->pos[1]<<" "<<pObject->pos[2]<<endl;
    //cout<<"str:"<<straddle<<endl;
    //cout<<"indx:"<<index<<endl;
    //cout<<"hw:"<<pTree->halfWidth<<endl;
      
      if(pTree->pChild[index] == NULL){
	//cout<<"case 2 ";
	  pTree = new Node;
	  pTree = BuildOctree(pTree->center,pTree->halfWidth,1);
	 //pTree->pChild[index]->pChild[0]=NULL;
	  InsertObject(pTree->pChild[index], pObject);
	  //cout<<"center:"<<pTree->center.x<<" "<<pTree->center.y<<" "<<pTree->center.z;
	 //cout<<"center:"<<pTree->pChild[0]->center.x<<" "<<pTree->pChild[0]->center.y<<" "<<pTree->pChild[0]->center.z;
      
	
      }else{
	  
	//InsertObject(pTree->pChild[index], pObject);
	  //cout<<"case 4 ";
	
      }
	
	
    }else{
      //postavi ga kao prvog u listi!!!
      pObject->next = pTree->pObjList;
      pTree->pObjList = pObject;
   */ 
      
    }
    /*
    cout<<"pos:"<<pObject->pos[0]<<" "<<pObject->pos[1]<<" "<<pObject->pos[2]<<endl;; 
    cout<<"str:"<<straddle<<endl;*/
    cout<<"indx:"<<index<<endl;
    cout<<"hw:"<<pTree->halfWidth<<endl;
  
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
  //  glTranslatef(pTree->center.x-pTree->halfWidth,pTree->center.y-pTree->halfWidth,pTree->center.z-pTree->halfWidth);
    glTranslatef(pTree->center.x,pTree->center.y,pTree->center.z);
    //glutWireCone(1,2,20,1);
    glutWireCube(pTree->halfWidth);
    //glutWireCube(1);
    glEnable(GL_LIGHTING);
    glPopMatrix();
 
    
  }
  

}