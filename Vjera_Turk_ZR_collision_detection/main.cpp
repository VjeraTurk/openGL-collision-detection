#include <stdlib.h>
#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "vector3f.h"
#include "vec3f.h"

#include "lighting.h"
#include "navigation.h"
#include "structures.h"

#define RED 50
#define BOX_SIZE 12

using namespace std;

int selection=1;
int box=1;

//all selections 
const float ALPHA = 0.6f;
const float GRAVITY =  9.80665f;
const float TIME_BETWEEN_UPDATES = 0.01f;//The amount of time between each time that we handle collisions and apply the effects of gravity
const int TIMER_MS = 25; //The number of milliseconds to which the timer is set

//selection 1
 Ball ball1;
 Ball ball2;
 Ball testBall;
 BallPair bp;
 BallWallPair bw, bw2;
 Wall bottom, top;

 //dodati da loptice ostavljaju trag, liniju:
 exPos *ex1;
 exPos *ex2;
 
//selection 2 
 Ball* AllBalls;
 BallPair* AllBallPairs;
 BallPair* AllBallWallPairs;
 Wall l,r,f,n,b,c;

//selection x
// Node root;

//all
void drawCube(float box_size);
float randomFloat();
void update(int value);
void display();
const float t =0.01f;

//selection 1
void elastic_collision();

    void drawBall(Ball ball);
  
    void move_ball(Ball *ball, float dt);
    void pullBall(Ball *ball);
    bool check_collision(BallPair pair);
    bool check_wall_collision(BallWallPair pair);
    void responce(BallPair pair);
    void w_responce(BallWallPair pair);

//selection 2
void multiple_balls_collision();

    Ball *create_list(int max);

    void drawBalls(Ball *head);

    void applyGravity(Ball *head);
    void moveBalls(Ball *head);
    
    void bruteForce(Ball *head);
    void ballsWallsCollisions(Ball *head);

 
//selection 3
 
const int MAX_OCTREE_DEPTH = 6;
const int MIN_BALLS_PER_OCTREE = 3;
const int MAX_BALLS_PER_OCTREE = 6;
class Octree {
      public:
		Vec3f corner1; //(minX, minY, minZ)
		Vec3f corner2; //(maxX, maxY, maxZ)
		Vec3f center;//((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2)
		
		/* The children of this, if this has any.  children[0][*][*] are the
		 * children with x coordinates ranging from minX to centerX.
		 * children[1][*][*] are the children with x coordinates ranging from
		 * centerX to maxX.  Similarly for the other two dimensions of the
		 * children array.
		 */
		Octree *children[2][2][2];
		//Whether this has children
		bool hasChildren;
		//The depth of this in the tree
		int depth;
	
		//The number of balls in this, including those stored in its children
		int numBalls;
		
      private:	//The balls in this, if this doesn't have any children
		set<Ball*> balls; 
		
		//Adds a ball to or removes one from the children of this
		
		//A5
		void fileBall(Ball* ball, Vec3f pos, bool addBall) {
			//Figure out in which child(ren) the ball belongs
			for(int x = 0; x < 2; x++) {
				if (x == 0) {
					if (pos[0] - ball->r > center[0]) {
						continue;
					}
				}
				else if (pos[0] + ball->r < center[0]) {
					continue;
				}
				
				for(int y = 0; y < 2; y++) {
					if (y == 0) {
						if (pos[1] - ball->r > center[1]) {
							continue;
						}
					}
					else if (pos[1] + ball->r < center[1]) {
						continue;
					}
					
					for(int z = 0; z < 2; z++) {
						if (z == 0) {
							if (pos[2] - ball->r > center[2]) {
								continue;
							}
						}
						else if (pos[2] + ball->r < center[2]) {
							continue;
						}
						
						//Add or remove the ball
						if (addBall) {
							children[x][y][z]->add(ball);
						}
						else {
							children[x][y][z]->remove(ball, pos);
						}
					}
				}
			}
		}
		
		//Creates children of this, and moves the balls in this to the children
		//A6
		void haveChildren() {
			for(int x = 0; x < 2; x++) {
				float minX;
				float maxX;
				if (x == 0) {
					minX = corner1[0];
					maxX = center[0];
				}
				else {
					minX = center[0];
					maxX = corner2[0];
				}
				
				for(int y = 0; y < 2; y++) {
					float minY;
					float maxY;
					if (y == 0) {
						minY = corner1[1];
						maxY = center[1];
					}
					else {
						minY = center[1];
						maxY = corner2[1];
					}
					
					for(int z = 0; z < 2; z++) {
						float minZ;
						float maxZ;
						if (z == 0) {
							minZ = corner1[2];
							maxZ = center[2];
						}
						else {
							minZ = center[2];
							maxZ = corner2[2];
						}
						
						children[x][y][z] = new Octree(Vec3f(minX, minY, minZ),Vec3f(maxX, maxY, maxZ),depth + 1);
					}
				}
			}
			
			//Remove all balls from "balls" and add them to the new children
			for(set<Ball*>::iterator it = balls.begin(); it != balls.end();
					it++) {
				Ball* ball = *it;
				fileBall(ball, ball->pos, true);
			}
			balls.clear();
			
			hasChildren = true;
		}
		
		//Adds all balls in this or one of its descendants to the specified set
		void collectBalls(set<Ball*> &bs) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int y = 0; y < 2; y++) {
						for(int z = 0; z < 2; z++) {
							children[x][y][z]->collectBalls(bs);
						}
					}
				}
			}
			else {
				for(set<Ball*>::iterator it = balls.begin(); it != balls.end();
						it++) {
					Ball* ball = *it;
					bs.insert(ball);
				}
			}
		}
		
		//Destroys the children of this, and moves all balls in its descendants
		//to the "balls" set
		void destroyChildren() {
			//Move all balls in descendants of this to the "balls" set
			collectBalls(balls);
			
			for(int x = 0; x < 2; x++) {
				for(int y = 0; y < 2; y++) {
					for(int z = 0; z < 2; z++) {
						delete children[x][y][z];
					}
				}
			}
			
			hasChildren = false;
		}
		
		//Removes the specified ball at the indicated position
		void remove(Ball* ball, Vec3f pos) {
			numBalls--;
			
			if (hasChildren && numBalls < MIN_BALLS_PER_OCTREE) {
				destroyChildren();
			}
			
			if (hasChildren) {
				fileBall(ball, pos, false);
			}
			else {
				balls.erase(ball);
			}
		}
		
		/* Helper fuction for potentialBallWallCollisions(vector).  Adds
		 * potential ball-wall collisions to cs, where w is the type of wall,
		 * coord is the relevant coordinate of the wall ('x', 'y', or 'z'), and
		 * dir is 0 if the wall is in the negative direction and 1 if it is in
		 * the positive direction.  Assumes that this is in the extreme
		 * direction of the coordinate, e.g. if w is WALL_TOP, the function
		 * assumes that this is in the far upward direction.
		 */
		void potentialBallWallCollisions(vector<BallWallPair> &cs,Wall *w, char coord, int dir) {
			if (hasChildren) {
				//Recursively call potentialBallWallCollisions on the correct
				//half of the children (e.g. if w is WALL_TOP, call it on
				//children above centerY)
				for(int dir2 = 0; dir2 < 2; dir2++) {
					for(int dir3 = 0; dir3 < 2; dir3++) {
						Octree *child;
						switch (coord) {
							case 'x':
								child = children[dir][dir2][dir3];
								break;
							case 'y':
								child = children[dir2][dir][dir3];
								break;
							case 'z':
								child = children[dir2][dir3][dir];
								break;
						}
						
						child->potentialBallWallCollisions(cs, w, coord, dir);
					}
				}
			}
			else {
				//Add (ball, w) for all balls in this
				for(set<Ball*>::iterator it = balls.begin(); it != balls.end();
						it++) {
					Ball* ball = *it;
					BallWallPair bwp;
					bwp.ball = ball;
					bwp.wall = w;
					cs.push_back(bwp);
				}
			}
		}
	public:
		//Constructs a new Octree.  c1 is (minX, minY, minZ), c2 is (maxX, maxY,
		//maxZ), and d is the depth, which starts at 1.
	  
	  
		Octree(Vec3f c1, Vec3f c2, int d) {
			corner1 = c1;
			corner2 = c2;
			center = (c1 + c2) / 2;
			depth = d;
			numBalls = 0;
			hasChildren = false;
		}
		
		//Destructor
		~Octree() {
			if (hasChildren) {
				destroyChildren();
			}
		}
		
		//Adds a ball to this
		void add(Ball* ball) {
			numBalls++;
			if (!hasChildren && depth < MAX_OCTREE_DEPTH &&
				numBalls > MAX_BALLS_PER_OCTREE) {
				haveChildren();
			}
			
			if (hasChildren) {
				fileBall(ball, ball->pos, true);
			}
			else {
				balls.insert(ball);
			}
		}
		
		//Removes a ball from this
		void remove(Ball* ball) {
			remove(ball, ball->pos);
		}
		
		//Changes the position of a ball in this from oldPos to ball->pos
		void ballMoved(Ball* ball, Vec3f oldPos) {
			remove(ball, oldPos);
			add(ball);
		}
		
		//Adds potential ball-ball collisions to the specified set
		void potentialBallBallCollisions(vector<BallPair> &collisions) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int y = 0; y < 2; y++) {
						for(int z = 0; z < 2; z++) {
							children[x][y][z]->
								potentialBallBallCollisions(collisions);
						}
					}
				}
			}
			else {
				//Add all pairs (ball1, ball2) from balls
				for(set<Ball*>::iterator it = balls.begin(); it != balls.end();
						it++) {
					Ball* ball1 = *it;
					for(set<Ball*>::iterator it2 = balls.begin();
							it2 != balls.end(); it2++) {
						Ball* ball2 = *it2;
						//This test makes sure that we only add each pair once
						if (ball1 < ball2) {
							BallPair bp;
							bp.bA = ball1;
							bp.bB = ball2;
							collisions.push_back(bp);
						}
					}
				}
			}
		}
		
		//Adds potential ball-wall collisions to the specified set
		void potentialBallWallCollisions(vector<BallWallPair> &collisions) {
			potentialBallWallCollisions(collisions, &l, 'x', 0);
			potentialBallWallCollisions(collisions, &r, 'x', 1);
			potentialBallWallCollisions(collisions, &b, 'y', 0);
			potentialBallWallCollisions(collisions, &c, 'y', 1);
			potentialBallWallCollisions(collisions, &f, 'z', 0);
			potentialBallWallCollisions(collisions, &n, 'z', 1);
		}
};
vector<Ball*> _balls; //All of the balls in play
float _angle = 0.0f; //The camera angle
//The amount of time until performUpdate should be called
float _timeUntilUpdate = 0;
Octree* _octree; //An octree with all af the balls

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void drawCube(float box_size){
  
  
  glBegin(GL_QUADS);
  
	if(b.t_red)b.t_red--;
	if(c.t_red)c.t_red--;
	if(n.t_red)n.t_red--;	
	if(f.t_red)f.t_red--; //Where ever you areee
	if(l.t_red)l.t_red--;
	if(r.t_red)r.t_red--;		

	//Top face

	glColor4f(1,-(c.t_red/50)+ 1, -(c.t_red/50)+ 1, ALPHA);	
	glNormal3f(0.0, -1.0f, 0.0f);
	
	glVertex3f(-box_size / 2, box_size / 2, -box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, -box_size / 2);
	
	//Bottom face

	glColor4f(1,-(b.t_red/50)+ 1, -(b.t_red/50)+ 1, ALPHA);	
	glNormal3f(0.0, 1.0f, 0.0f);
	
	glVertex3f(-box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, box_size / 2);
	glVertex3f(-box_size / 2, -box_size / 2, box_size / 2);
	
	//Left face
	

	glColor4f(1,-(l.t_red/50)+ 1, -(l.t_red/50)+ 1, ALPHA);	
	

	glNormal3f(1.0, 0.0f, 0.0f);

	glVertex3f(-box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(-box_size / 2, -box_size / 2, box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, -box_size / 2);
	
	//Right face
	glColor4f(1,-(r.t_red/50)+ 1, -(r.t_red/50)+ 1, ALPHA);	

	glNormal3f(-1.0, 0.0f, 0.0f);
	
	glVertex3f(box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, box_size / 2);
		
	//near face
        glColor4f(1,-(n.t_red/50)+ 1, -(n.t_red/50)+ 1, ALPHA);	

	glNormal3f(0.0, 0.0f, -1.0f);
	
	glVertex3f(-box_size / 2, -box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, box_size / 2);
	
	//Far face
	glColor4f(1,-(f.t_red/50)+ 1, -(f.t_red/50)+ 1, ALPHA);	

	glNormal3f(0.0, 0.0f, 1.0f);
	
	glVertex3f(-box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, -box_size / 2);
	
	glEnd();
	
  
}
float randomFloat() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

void drawBall(Ball ball)
{
     glPushMatrix();
    
    glColor3f(ball.color[0],ball.color[1], ball.color[2]);
    glTranslatef(ball.pos[0],ball.pos[1],ball.pos[2]);
    glutSolidSphere(ball.r,20,20);
  
    glPopMatrix();
    
}

void move_ball(Ball *ball, float dt){
  
    glPushMatrix();
    
    ball->pos+=ball->v * dt;
   
    //cout <<"v.y "<<ball->v[1]<<endl;
    //cout <<"pos.y "<<ball->pos[1]<<endl;
    
    glPopMatrix();
  
}

void pullBall(Ball *ball){
  ball->v-= Vec3f(0, GRAVITY * TIME_BETWEEN_UPDATES, 0);
}

bool check_collision(BallPair pair){
   
   Ball *bA= pair.bA;
   Ball *bB= pair.bB;
   

   float r = bA->r + bB->r;
   //cout<<r<<endl;
  
   
   if ((bA->pos - bB->pos).magnitudeSquared() < r*r){  
     
     Vec3f netVelocity = bA->v - bB->v;
     Vec3f displacement = bA->pos - bB->pos;

    //Udaljavaju li se loptice?
    
    if(netVelocity.dot(displacement)<0){
      bA->t_red = RED;
      bB->t_red = RED;
      return 1;
    }  
    
    return 0;      
  
     
  }else return 0;

}
/*
void responce(BallPair pair){
  
  Ball *bA = pair.bA;
   Ball *bB = pair.bB;
  
   float m_1= bA->m;
   float m_2= bB->m;
   
   float v_1=bA->v[0];
   float v_2=bB->v[0];   
   
   	//v_1'=\frac{(m_1-m_2)v_1+2m_2v_2}{m_1+m_2}
   	
   	//v_2'=\frac{(m_2-m_1)v_2+2m_1v_1}{m_1+m_2}
   
 
   bA->v[0]= ((m_1 - m_2) * v_1	+ 2 * m_2 * v_2) / (m_1 + m_2) ;
   bB->v[0]= ((m_2 - m_1) * v_2 + 2 * m_1 * v_1)/ (m_1 + m_2) ;
  
   
     
   v_1=bA->v[1];
   v_2=bB->v[1]; 
   
   bA->v[1]= ((m_1 - m_2) * v_1	+ 2 * m_2 * v_2) / (m_1 + m_2) ;
   bB->v[1]= ((m_2 - m_1) * v_2 + 2 * m_1 * v_1)/ (m_1 + m_2) ;
   

   v_1=bA->v[2];
   v_2=bB->v[2]; 
   
   bA->v[2]= ((m_1 - m_2) * v_1	+ 2 * m_2 * v_2) / (m_1 + m_2) ;
   bB->v[2]= ((m_2 - m_1) * v_2 + 2 * m_1 * v_1)/ (m_1 + m_2) ;
   

//   Vec3f displacement = (bA->pos - bB->pos).normalize();
//  bA->v -= 2 * displacement * bA->v.dot(displacement);
//   bB->v -= 2 * displacement * bB->v.dot(displacement);
}*/

void responce(BallPair pair){
  
   Ball *bA = pair.bA;
   Ball *bB = pair.bB;
  
   float m_1= bA->m;
   float m_2= bB->m;
   
   Vec3f v_1=bA->v;
   Vec3f v_2=bB->v;   
   
   //v_1'=\frac{(m_1-m_2)v_1+2m_2v_2}{m_1+m_2}
   //v_2'=\frac{(m_2-m_1)v_2+2m_1v_1}{m_1+m_2}
   
 
   bA->v= (v_1*(m_1 - m_2) + v_2*(2 * m_2)) / (m_1 + m_2) ;
   bB->v = (v_2*(m_2 - m_1) + v_1*(2 * m_1 ))/ (m_1 + m_2) ; 
}

bool check_wall_collision(BallWallPair pair){
  
  Ball *b = pair.ball;
  Wall *w = pair.wall;
  
  Vec3f dir = w->direction;
  
  //if(b->pos[0]+b->r > BOX_SIZE/2 || b->pos[0]-b->r< -BOX_SIZE/2)
   if( b->pos.dot(dir) + b->r >= BOX_SIZE / 2 && b->v.dot(dir) > 0){
   //cout<<b->pos.dot(dir)<<endl;
   //cout<<b->v.dot(dir)<<endl;
   w->t_red=RED; 
   return 1;
  }
   return 0;
   
}

void real_responce(BallWallPair pair){
  
   Ball *b = pair.ball;
  
   float m_1= b->m;
   float m_2= 1; //loptice s vremenom stanu na pod, trepere dakle brzina im nije 0, polako tonu u pod
   
   float v_1;
   float v_2=0;

  //v_1'=\frac{(m_1-m_2)v_1+2m_2v_2}{m_1+m_2}
  //v_2'=\frac{(m_2-m_1)v_2+2m_1v_1}{m_1+m_2}
   
   v_1=b->v[0];
   b->v[0]= ((m_1 - m_2) * v_1	+ 2 * m_2 * v_2) / (m_1 + m_2) ;

   v_1 = b->v[1];
   b->v[1]= ((m_1 - m_2) * v_1	+ 2 * m_2 * v_2) / (m_1 + m_2) ;

   v_1 = b->v[2];
   b->v[2]= ((m_1 - m_2) * v_1	+ 2 * m_2 * v_2) / (m_1 + m_2) ;
  
   
}
/*
void real_responce(BallWallPair pair){
  
   Ball *b = pair.ball;
  
   float m_1= b->m;
   float m_2= 1; //loptice s vremenom stanu na pod, trepere dakle brzina im nije 0
   
   Vec3f v_1=b->v;
   Vec3f v_2=Vec3f(0,0,0);
  //v_1'=\frac{(m_1-m_2)v_1+2m_2v_2}{m_1+m_2}
  //v_2'=\frac{(m_2-m_1)v_2+2m_1v_1}{m_1+m_2}
   
    b->v= (v_1*(m_1 - m_2) + v_2*(2 * m_2)) / (m_1 + m_2) ;
   
}
*/
void ideal_responce(BallWallPair pair){
  
   Ball *b = pair.ball;
   Wall *w = pair.wall;

   Vec3f dir = w->direction.normalize();
   b->v -= 2 * dir * b->v.dot(dir);

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
  
      ball->m=ball->r;
      
      ball->next=NULL;
      
      if(!temp)head=ball;
      
      else  temp->next = ball;
      temp = ball;
    // cout<<ball->pos[3]<<endl;
  }
  return head;
}

void applyGravity(Ball *head) {
  Ball *temp;
  for(temp=head; temp!=NULL; temp=temp->next){
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

void drawBalls(Ball *head){
  
  Ball *temp;
 for(temp=head; temp!=NULL; temp = temp->next){
    glPushMatrix();
   // cout<<temp->m<<endl;
 	
      glTranslatef(temp->pos[0], temp->pos[1], temp->pos[2]);
		
		if(temp->t_red){
		
		  glColor3f(1, (-temp->t_red/50)+1,(-temp->t_red/50)+1);  
		  temp->t_red--;
		  
		}else{
		//glColor3f(temp->color[0],temp->color[1],temp->color[2]);  
		glColor3f(1, 1, 1);
		  
		}	//glColor3f(temp->color[0], temp->color[1], temp->color[2]);
		
		glutSolidSphere(temp->r, 12, 12); //Draw a sphere
		glPopMatrix(); 
  }

}

void bruteForce(Ball *head){
  
    Ball *temp1=NULL;
    Ball *temp2=NULL;
    
    BallPair bp;
    bp.bA = temp1;
    bp.bB = temp2;
    
 for(temp1=head; temp1!=NULL; temp1 = temp1->next){

    for(temp2=temp1->next; temp2!=NULL; temp2 = temp2->next){
     
      bp.bA = temp1;
      bp.bB = temp2;

      if(check_collision(bp))responce(bp);
      
    }   
   
 }
  
}
Wall *walls[]={&l,&r,&f,&n,&b,&c};

void ballsWallsCollisions(Ball *head){
  
	Ball *temp;
	 
	for(temp=head; temp!=NULL; temp = temp->next){
		
	    for(int j = 0; j < 6; j++) {
			
			BallWallPair bwp;
			bwp.ball = temp;
			bwp.wall = walls[j];
			
			if(check_wall_collision(bwp) && box)real_responce(bwp);
			if(check_wall_collision(bwp) && !box)ideal_responce(bwp);
		
	      
	    }
	}

}


void elastic_collision(){
  glPushMatrix();
  glTranslatef(0,-BOX_SIZE/2,0);
    glBegin(GL_QUADS);
	  glColor3f(1,1,1);	
	  glNormal3f(0.0, 1.0f, 0.0f);
	  //glNormal3f(0.0, -1.0f, 0.0f);		
	  glVertex3f(-BOX_SIZE / 2, 0, -BOX_SIZE / 2);
	  glVertex3f(BOX_SIZE / 2, 0, -BOX_SIZE / 2);
	  glVertex3f(BOX_SIZE / 2, 0, BOX_SIZE / 2);
	  glVertex3f(-BOX_SIZE / 2, 0, BOX_SIZE / 2);
      glEnd();
      
   glPopMatrix(); 
   
      drawBall(ball1); 
      drawBall(ball2); 
      
  
}

void multiple_balls_collision(){ 
     drawBalls(AllBalls); 
}

void init_central(){
   //ball 1
    ball1.m=0.4f;
    ball1.r=0.4f; 
   
    ball1.pos=Vec3f(-2,-BOX_SIZE/2+ball1.r,0);
    ball1.v=Vec3f(4,0,0);  
    
    ball1.color= Vec3f(1,0,0);
    
    
    //ball 2
    ball2.r=0.8f;
    ball2.m=1.0f;
    
    ball2.pos=Vec3f(2,-BOX_SIZE/2+ball2.r,0);
    ball2.v=Vec3f(-10,0,0);  
    
    ball2.color= Vec3f(0,0,1);
    
    bp.bA = &ball1;
    bp.bB = &ball2;
  
  
}
void init(){
     
   //ball 1
    ball1.m=0.4f;
    ball1.r=0.4f; 
   
    ball1.pos=Vec3f(-3,-BOX_SIZE/2+ball1.r,-2);
    ball1.v=Vec3f(4,0,4.5);  
    
    ball1.color= Vec3f(1,0,0);
    
    
    //ball 2
    ball2.r=0.8f;
    ball2.m=1.0f;
    
    ball2.pos=Vec3f(2,-BOX_SIZE/2+ball2.r,-1);
    ball2.v=Vec3f(-10,0,4);  
    
    ball2.color= Vec3f(0,0,1);
    
    bp.bA = &ball1;
    bp.bB = &ball2;
   
    bottom.direction = Vec3f(0, -1, 0);
    top.direction = Vec3f(0, 1, 0);
   
    bw.ball = &ball2;
    bw.wall = &bottom;
 
    bw2.ball = &ball1;
    bw2.wall = &bottom;
    
    
    AllBalls=create_list(100);
    
    //walls
      l.direction = Vec3f(-1, 0, 0);
      r.direction=Vec3f(1, 0, 0);
      f.direction=Vec3f(0, 0, -1);
      n.direction=Vec3f(0, 0, 1);
      c.direction=Vec3f(0, 1, 0);
      b.direction=Vec3f(0, -1, 0);
	
   //selection 3

    _octree = new Octree(Vec3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2),Vec3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2), 1);

     //selection X
  /*    
     //test ball
 
    testBall.pos=Vec3f(3,0,0);
    testBall.r=0.5;
    testBall.color=Vec3f(0,1,0);
    root.center=Point(0,0,0);//radi
    root.halfWidth=12;
    //root.center=Point(root.halfWidth,0,0);
    
    //BuildOctree(root.center,root.halfWidth,1);   
 */
}
int pause = 1;
int show_walls = 1;
int ot = 1;
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void drawOctree(Octree* octree, float box_size){

      if (octree->hasChildren)
      {  
	 for(unsigned int i=0;i<2;i++){
	    for(unsigned int j=0;j<2;j++){
		for(unsigned int k=0;k<2;k++){
	
	    drawOctree(octree->children[i][j][k], box_size/2);
	    
		}
	    }
	}
      
      }
      else if(!octree->hasChildren)
      {
	glPushMatrix();
	glDisable(GL_LIGHTING);
	//cout<< octree->depth/10;
	switch(octree->depth){
	 
	  case 1: glColor3f(1.0f,1.0f,1.0f);
		  break;
	  case 2: glColor3f(1.0f,0.0f,0.0f);
		  break;
	  case 3: glColor3f(0.0f,1.0f,0.0f);
		  break;
	  case 4: glColor3f(0.0f,0.7f,1.0f);
		  break;
	  case 5: glColor3f(1.0f,1.0f,0.0f);
		  break;
	  case 6: glColor3f(1.0f,0.0f,1.0f);
		  break;
	}
	glTranslatef(octree->center[0], octree->center[1], octree->center[2]);
	glutWireCube(box_size);
	glEnable(GL_LIGHTING);
	
	glPopMatrix();
      }
  
}
//Puts potential ball-ball collisions in potentialCollisions.  It must return
//all actual collisions, but it need not return only actual collisions.
void potentialBallBallCollisions(vector<BallPair> &potentialCollisions,
								 vector<Ball*> &balls, Octree* octree) {
	//Fast method
	octree->potentialBallBallCollisions(potentialCollisions);
	//cout << potentialCollisions.size() << '\n';
	/*
	//Slow method
	for(unsigned int i = 0; i < balls.size(); i++) {
		for(unsigned int j = i + 1; j < balls.size(); j++) {
			BallPair bp;
			bp.bA = balls[i];
			bp.bB = balls[j];
			potentialCollisions.push_back(bp);
		}
	}
	*/
}

//Puts potential ball-wall collisions in potentialCollisions.  It must return
//all actual collisions, but it need not return only actual collisions.
void potentialBallWallCollisions(vector<BallWallPair> &potentialCollisions,
								 vector<Ball*> &balls, Octree* octree) {
	//Fast method
	octree->potentialBallWallCollisions(potentialCollisions);
	
	/*
	//Slow method
	Wall walls[] =
		{WALL_LEFT, WALL_RIGHT, WALL_FAR, WALL_NEAR, WALL_TOP, WALL_BOTTOM};
	for(unsigned int i = 0; i < balls.size(); i++) {
		for(int j = 0; j < 6; j++) {
			BallWallPair bwp;
			bwp.ball = balls[i];
			bwp.wall = walls[j];
			potentialCollisions.push_back(bwp);
		}
	}
	*/
}

//Moves all of the balls by their velocity times dt
void moveBalls(vector<Ball*> &balls, Octree* octree, float dt) {
	for(unsigned int i = 0; i < balls.size(); i++) {
		Ball* ball = balls[i];
		Vec3f oldPos = ball->pos;
		ball->pos += ball->v * dt;
		octree->ballMoved(ball, oldPos);
	}
}

//Decreases the y coordinate of the velocity of each ball by GRAVITY *
//TIME_BETWEEN_UPDATES
void applyGravity(vector<Ball*> &balls) {
	for(unsigned int i = 0; i < balls.size(); i++) {
		Ball* ball = balls[i];
		ball->v -= Vec3f(0, GRAVITY * TIME_BETWEEN_UPDATES, 0);
	}
}

//Returns whether two balls are colliding
bool testBallBallCollision(Ball* b1, Ball* b2) {
	//Check whether the balls are close enough
	float r = b1->r + b2->r;
	if ((b1->pos - b2->pos).magnitudeSquared() < r * r) {
		//Check whether the balls are moving toward each other
		Vec3f netVelocity = b1->v - b2->v;
		Vec3f displacement = b1->pos - b2->pos;
		
		if(netVelocity.dot(displacement)<0){
		b1->t_red=RED;
		b2->t_red=RED;
		  
		} 
		
		return netVelocity.dot(displacement) < 0;
		
	  
	}
	else
		return false;
}

//Handles all ball-ball collisions
void handleBallBallCollisions(vector<Ball*> &balls, Octree* octree) {
	vector<BallPair> bps;
	
	potentialBallBallCollisions(bps, balls, octree);
	vector<Ball*> redballs;
	
	for(unsigned int i = 0; i < bps.size(); i++) {
		BallPair bp = bps[i];
		
		Ball* b1 = bp.bA;
		Ball* b2 = bp.bB;
	
		if (testBallBallCollision(b1, b2)) {
			//Make the balls reflect off of each other
			Vec3f displacement = (b1->pos - b2->pos).normalize();
			b1->v -= 2 * displacement * b1->v.dot(displacement);
			b2->v -= 2 * displacement * b2->v.dot(displacement);
		}
		
	}
}

bool testBallWallCollision(Ball* ball, Wall* wall) {
	Vec3f dir = wall->direction;
	//Check whether the ball is far enough in the "dir" direction, and whether
	//it is moving toward the wall
	//return ball->pos.dot(dir) + ball->r > BOX_SIZE / 2 && ball->v.dot(dir) > 0;
	
	if( ball->pos.dot(dir) + ball->r >= BOX_SIZE / 2 && ball->v.dot(dir)>0 ){ 
	  
	  wall->t_red=50.0f;
	}
	
	return ball->pos.dot(dir) + ball->r >= BOX_SIZE / 2 && ball->v.dot(dir) > 0;
  
	
}

//Handles all ball-wall collisions
void handleBallWallCollisions(vector<Ball*> &balls, Octree* octree) {
	
	vector<BallWallPair> bwps;
	
	potentialBallWallCollisions(bwps, balls, octree);//fill bwps
	
	for(unsigned int i = 0; i < bwps.size(); i++) {
		
		BallWallPair bwp = bwps[i];
		Ball* b = bwp.ball;
		Wall* w = bwp.wall;
		
		 
		if (testBallWallCollision(b, w)) {
			//Make the ball reflect off of the wall
			Vec3f dir = w->direction.normalize();
			b->v -= 2 * dir * b->v.dot(dir);

		}
	}
}

//Applies gravity and handles all collisions.  Should be called every
//TIME_BETWEEN_UPDATES seconds.
void performUpdate(vector<Ball*> &balls, Octree* octree) {
	
	applyGravity(balls);
	handleBallBallCollisions(balls, octree);
	handleBallWallCollisions(balls, octree);
	cout<<(octree->numBalls)<<endl;
}
//Advances the state of the balls by t.  timeUntilUpdate is the amount of time
//until the next call to performUpdate.
void advance(vector<Ball*> &balls,
			 Octree* octree,
			 float t,
			 float &timeUntilUpdate) {
	while (t > 0) {
		if (timeUntilUpdate <= t) {
			moveBalls(balls, octree, timeUntilUpdate);
			performUpdate(balls, octree);
			t -= timeUntilUpdate;
			timeUntilUpdate = TIME_BETWEEN_UPDATES;
		}
		else {
			moveBalls(balls, octree, t);
			timeUntilUpdate -= t;
			t = 0;
		}
	}
}

//Deletes everything.  This should be called when exiting the program.
void cleanup() {
	for(unsigned int i = 0; i < _balls.size(); i++) {
		delete _balls[i];
	}
	delete _octree;
}
void drawBalls(){
  //Draw the balls
	
      for(unsigned int i = 0; i < _balls.size(); i++) {
		Ball* ball = _balls[i];
		glPushMatrix();
		glTranslatef(ball->pos[0], ball->pos[1], ball->pos[2]);
		
		if(ball->t_red){
		
		  glColor3f(1, (-ball->t_red/50)+1,(-ball->t_red/50)+1);  
		  ball->t_red--;
		  
		}else{
		  
		glColor3f(1, 1, 1);
		  
		}	//glColor3f(ball->color[0], ball->color[1], ball->color[2]);
		
		glutSolidSphere(ball->r, 12, 12); //Draw a sphere
		glPopMatrix();
	}
	/*
	for(unsigned int i = 0; i < _balls.size(); i++) {
		Ball* ball = _balls[i];
		glPushMatrix();
		glTranslatef(ball->pos[0], ball->pos[1], ball->pos[2]);
		glColor3f(1,0,0);
		glutSolidSphere(ball->r, 12, 12); //Draw a sphere
		glPopMatrix();
	}*/
	
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
		//case 'r' : testBall.pos=Vec3f(root.center.x,root.center.y,root.center.z);
		 //     break;
		case '1': selection=1;
			break;
		case '2': selection=2;
		      break;
		case '3': selection=3;
		      break;
		case '0':
			init();
		  break;
		case 'b': if(box==1)box=0;
			  else box=1;
			  
			  if(show_walls == 1) show_walls = 0;
			  else show_walls=1;
			  
		case 'c': init_central();
		  
		  break;
				case ' ':
			//Add balls with a random position, velocity, radius, and color
			for(int i = 0; i < 100; i++) {
				Ball* ball = new Ball();
				ball->pos = Vec3f(8 * randomFloat() - 4,
								  8 * randomFloat() - 4,
								  8 * randomFloat() - 4);
				ball->v = Vec3f(8 * randomFloat() - 4,
								8 * randomFloat() - 4,
								8 * randomFloat() - 4);
				ball->r = 0.1f * randomFloat() + 0.1f;
				ball->m = 0.1f * randomFloat() + 0.1f;
				_balls.push_back(ball);
				_octree->add(ball);
			} break;
		case 'n': _timeUntilUpdate=1;
			  break;
		case 'p': if(pause == 1) pause = 0;
			  else if(pause==0) pause = 1;
			  break;
		case 'o': if(ot == 1) ot=0;
			  else ot = 1;
			  break;
	

	}
}	

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void display()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
   // glTranslatef(0.0, 0.0, -(dis+ddis));

    glTranslatef(0.0, 0.0, -25.0f);

    glRotated(elev+delev, 1.0, 0.0, 0.0);
    glRotated(azim+dazim, 0.0, 1.0, 0.0);

    Lighting(BOX_SIZE);
    glShadeModel(GL_SMOOTH);
    
    glColor3f(1,1,1);
   // drawCube(BOX_SIZE);

 //   InsertObject(&root,&ball1);
//    InsertObject(&root,&ball2);
       

        switch(selection){
      
      case 1 :   
	    elastic_collision();
		 break;
      case 2 : 
	      glRotatef(-_angle, 0.0f, 1.0f, 0.0f);
	      if(box)drawCube(BOX_SIZE);
	      else glutWireCube(BOX_SIZE);
	      multiple_balls_collision();
		break;	
      case 3 : 
	    	glRotatef(-_angle, 0.0f, 1.0f, 0.0f);
	
		if(show_walls==1){
		  drawCube(BOX_SIZE);  
		}
		if(ot==1){
		drawOctree(_octree, BOX_SIZE);  
		}
		drawBalls();

	  break;
    }

    
    
    
    glutSwapBuffers();
}

void update(int value) {
 
    switch(selection){
      
      case 1:
	   
           move_ball(&ball1,t);
	   move_ball(&ball2,t);
	  // pullBall(&ball1);
	  // pullBall(&ball2);
	   
	   
	   if(check_wall_collision(bw) || box)ideal_responce(bw);
           if(check_wall_collision(bw2)|| box)ideal_responce(bw2);
	   
	   if(check_collision(bp)) responce(bp);
	   
	break;
 
      case 2:
	  _angle += (float)TIMER_MS / 100;
	  if (_angle > 360) {
		_angle -= 360;
	  }
	  applyGravity(AllBalls);
	  moveBalls(AllBalls);
	  ballsWallsCollisions(AllBalls);
	  bruteForce(AllBalls);
	  break;
	  
      case 3:
	
	advance(_balls, _octree, (float)TIMER_MS / 1000.0f, _timeUntilUpdate);
	_angle += (float)TIMER_MS / 100;
	if (_angle > 360) {
		_angle -= 360;
	}
	break;
	
    }
    
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}


int main(int argc,char **argv)
{
    srand((unsigned int)time(0)); //Seed the random number generator	
    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(700, 600);
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



