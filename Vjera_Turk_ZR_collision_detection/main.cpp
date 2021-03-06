#include <stdlib.h>
#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "vec3f.h"
#include "lighting.h"
#include "navigation.h"
#include "structures.h"

#define RED 40
#define BOX_SIZE 12
int NUM_BALLS=20;
int PLUS_BALL=20;
int num_tests_octree;
int num_tests_list;

using namespace std;
/** Program se sastoji od tri veća dijela, ova varijabla oznacava koji dio se trenutno prikazuje. 
 Prikaz se mijenja tipkovnicom*/
int selection = 0;
/** box = true zidovi su vidljivi, box = false zidovi se ne iscrtavaju*/
bool box = 1;
/** pause = true, stanje loptica se ne ažurira, miruju na zatečenom mjestu*/
bool pause = 1;
/** ot = true Oktalno stablo je prikazano kao skup podjeljenih prostora*/
bool ot = 1;
/**dimenzija u kojoj se promatra sudar (ako pozicije dozvoljavaju)*/
int d = 2;
/**Faktor prozirnosti*/
const float ALPHA = 0.6f;
/**Gravitacijska konstanta*/
const float GRAVITY =  9.80665f;
/**Količina vremena između svakog ponovnog detektiranja sudare i simuliranja djelovanje gravitacije*/
const float TIME_BETWEEN_UPDATES = 0.001f;
/**Broj milisekundi tajmera*/
const int TIMER_MS = 25; 

//selection 1
 Ball ball1;
 Ball ball2;
 BallPair bp;
 BallWallPair bw, bw2;
 Wall bottom;
 
 
//selection 2 
 Ball* AllBalls;
 BallPair* AllBallPairs;
 BallPair* AllBallWallPairs;
 Wall l,r,f,n,b,c;
  
  void update(int value);
  void display();
/** Funkcija crta kutiju veličine stranice box_size, mijanja boju zidova ovisno o sudaru loptica s njima
 
*/  
  void drawCube(float box_size);
  float randomFloat();

  void handleKeypress(unsigned char key, int x, int y);
  void handleKeypress2(unsigned char key, int x, int y);
  void handleKeypress3(unsigned char key, int x, int y);
/**  Funkcija crta lopticu pomoću njenih parametara r i pos
*/    
  void drawBall(Ball ball);
  void drawBigBall(Ball ball);
  void move_ball(Ball *ball, float dt);
  void pullBall(Ball *ball);
  
  
  Ball *create_list(int max);
  void multiple_balls_collision();
  void drawBalls(Ball *head);
  void applyGravity(Ball *head);
  void moveAllBalls(Ball *head, float t);
  void bruteForce(Ball *head);
  void ballsWallsCollisions(Ball *head);

  bool check_collision(BallPair pair);
  bool check_wall_collision(BallWallPair pair);

  void w_responce(BallWallPair pair);

  void collision2D(BallPair pair);
  void collision3D(BallPair pair,float& vx2, float& vy2, float& vz2); 
  void angleFreeCollision3D(BallPair pair);

//////////////////////////////////////////////////////////////
//selection 3 - Octree 
  const int MAX_OCTREE_DEPTH = 6;
  const int MIN_BALLS_PER_OCTREE = 6;
  const int MAX_BALLS_PER_OCTREE = 12;

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
vector<Ball*> _balls; //Sve loptice
float _angle = 0.0f; //Kut kamere
//Količina vremena do sljedećeg poziva funkcije preformUpdate
float _timeUntilUpdate = 0;
Octree* _octree; //Korijen stabla, stablo sa svim lopticama
/////////////////////////////////////////////////////////////

void drawCube(float box_size){

  glBegin(GL_QUADS);
  
	if(b.t_red)b.t_red--;
	if(c.t_red)c.t_red--;
	if(n.t_red)n.t_red--;
	if(f.t_red)f.t_red--;
	if(l.t_red)l.t_red--;
	if(r.t_red)r.t_red--;	
	//strop
	
	glColor4f(-(c.t_red/RED)+ 1,1, -(c.t_red/RED)+ 1, ALPHA);
	//glColor4f(1,-(c.t_red/RED)+ 1, -(c.t_red/RED)+ 1, ALPHA); //odkomentirati za crveni zid nakon sudara	
	glNormal3f(0.0, -1.0f, 0.0f);
	
	glVertex3f(-box_size / 2, box_size / 2, -box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, -box_size / 2);
	
	//pod
	glColor4f(-(b.t_red/RED)+ 1,1, -(b.t_red/RED)+ 1, ALPHA);	
	//glColor4f(1,-(b.t_red/RED)+ 1, -(b.t_red/RED)+ 1, ALPHA);	
	glNormal3f(0.0, 1.0f, 0.0f);
	
	glVertex3f(-box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, box_size / 2);
	glVertex3f(-box_size / 2, -box_size / 2, box_size / 2);
	
	//lijevi zidovi
	glColor4f(-(l.t_red/RED)+ 1,1, -(l.t_red/RED)+ 1, ALPHA);	
	//glColor4f(1,-(l.t_red/RED)+ 1, -(l.t_red/RED)+ 1, ALPHA);	
	glNormal3f(1.0, 0.0f, 0.0f);

	glVertex3f(-box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(-box_size / 2, -box_size / 2, box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, -box_size / 2);
	
	//desni zid
	glColor4f(-(r.t_red/RED)+ 1,1, -(r.t_red/RED)+ 1, ALPHA);	
	//glColor4f(1,-(r.t_red/RED)+ 1, -(r.t_red/RED)+ 1, ALPHA);	
	glNormal3f(-1.0, 0.0f, 0.0f);
	
	glVertex3f(box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, box_size / 2);
		
	//prednji zid (near)
	glColor4f(-(n.t_red/RED)+ 1,1, -(n.t_red/RED)+ 1, ALPHA);	
        //glColor4f(1,-(n.t_red/RED)+ 1, -(n.t_red/RED)+ 1, ALPHA);	
	glNormal3f(0.0, 0.0f, -1.0f);
	
	glVertex3f(-box_size / 2, -box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, box_size / 2);
	
	//stražnji zid (far)
	glColor4f(-(f.t_red/RED)+ 1,1, -(f.t_red/RED)+ 1, ALPHA);	
	//glColor4f(1,-(f.t_red/RED)+ 1, -(f.t_red/RED)+ 1, ALPHA);	
	glNormal3f(0.0, 0.0f, 1.0f);
	
	glVertex3f(-box_size / 2, -box_size / 2, -box_size / 2);
	glVertex3f(-box_size / 2, box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, box_size / 2, -box_size / 2);
	glVertex3f(box_size / 2, -box_size / 2, -box_size / 2);

  glEnd();
	
  
}
float randomFloat() {
	return (float)rand()/((float)RAND_MAX + 1);
}
void drawBall(Ball ball)
{
    glPushMatrix();
      //glColor3f(ball.color[0],ball.color[1], ball.color[2]);// odkomentirati za loptice u boji
      glTranslatef(ball.pos[0],ball.pos[1],ball.pos[2]);
      glutSolidSphere(ball.r,20,20);
    glPopMatrix();
    
}
void drawBigBall(Ball ball)
{
     glPushMatrix();
	glColor3f(ball.color[0],ball.color[1], ball.color[2]);
	glTranslatef(ball.pos[0],ball.pos[1],ball.pos[2]);
	glutSolidSphere(ball.r,50,50);
    glPopMatrix();
}
void move_ball(Ball *ball, float dt){
    glPushMatrix(); 
      ball->pos+=ball->v * dt;
    glPopMatrix();
}
void pullBall(Ball *ball){
  if(ball->pos[1]-ball->r >= -BOX_SIZE/2) ball->v-= Vec3f(0, GRAVITY * TIME_BETWEEN_UPDATES, 0);
}
bool check_collision(BallPair pair){
   
   Ball *bA= pair.bA;
   Ball *bB= pair.bB;
   
   float r = bA->r + bB->r;

   if ((bA->pos - bB->pos).magnitudeSquared() < r*r){  
     
      Vec3f netVelocity = bA->v - bB->v;
      Vec3f displacement = bA->pos - bB->pos;

      if(netVelocity.dot(displacement)<0){
	  bA->t_red = RED;
	  bB->t_red = RED;
	  return 1;
      }  
      return 0;
      
  }else return 0;

}
void collision2D(BallPair pair){

    Ball *b1=pair.bA;
    Ball *b2=pair.bB;

    float m1=b1->m;
    float m2=b2->m;
    
    float m21 = m2/m1;
    
    float x21 = b2->pos[0]- b1->pos[0];
    float z21 = b2->pos[2]- b1->pos[2];
    
    float vx21 = b2->v[0]- b1->v[0];
    float vz21 = b2->v[2]- b1->v[2];
    
    float a=z21/x21; //tanges kuta
    float dvx2= -2*(vx21 +a*vz21)/((1+a*a)*(1+m21)) ;
    
    b2->v[0]+=dvx2;
    b2->v[2]+=a*dvx2;
    b1->v[0]-=m21*dvx2;
    b1->v[2]-=a*m21*dvx2;
  
} 
void collision3D(BallPair pair,float& vx2, float& vy2, float& vz2){
    
    float  r12,m21,st,ct,sp,cp;
    float vx1r,vy1r,vz1r;
    float theta2,phi2,thetav,phiv,alpha,beta,sbeta,cbeta;
    float dr,a,dvz2;
    float vx2r,vy2r,vz2r;
      
    Ball *b1=pair.bA;
    Ball *b2=pair.bB;

    m21=b2->m/b1->m;
    r12=b1->r+b2->r;
    
    Vec3f pos21=b2->pos-b1->pos; 
    float d=pos21.magnitude(); //relativna udaljenost
    Vec3f v21=b2->v - b1->v;
    float v=v21.magnitude(); //relativna brzina
    
    b2->pos=pos21;
    Vec3f pos2=pos21;
     
    b1->v=-v21;

    theta2=acos(b2->pos[2]/d);
  
    if (b2->pos[1]==0 && b2->pos[0]==0) {phi2=0;} else {phi2=atan2(b2->pos[1],b2->pos[0]);}
      
    st=sin(theta2);
    ct=cos(theta2);
    sp=sin(phi2);
    cp=cos(phi2);

// express the velocity vector of ball 1 in a rotated coordinate
// system where ball 2 lies on the z-axis
    vx1r=ct*cp*b1->v[0]+ct*sp*b1->v[1]-st*b1->v[2];
    vy1r=cp*b1->v[1]-sp*b1->v[0];
    vz1r=st*cp*b1->v[0]+st*sp*b1->v[1]+ct*b1->v[2];
  
    thetav=acos(vz1r/v);
    phiv=atan2(vy1r,vx1r);						
// calculate the normalized impact parameter
    dr=d*sin(thetav)/r12;

//calculate impact angles if balls do collide
    alpha=asin(-dr);
    beta=phiv;
    sbeta=sin(beta);
    cbeta=cos(beta);
    
//update positions and reverse the coordinate shift
    b2->pos=pos2+b1->pos;
//update velocitie
    a=tan(thetav+alpha);
    dvz2=2*(vz1r+a*(cbeta*vx1r+sbeta*vy1r))/((1+a*a)*(1+m21));
    
    vz2r=dvz2;
    vx2r=a*cbeta*dvz2;
    vy2r=a*sbeta*dvz2;
    
    vz1r=vz1r-m21*vz2r;
    vx1r=vx1r-m21*vx2r;
    vy1r=vy1r-m21*vy2r;
    
//rotate the velocity vectors back and add the initial velocity
//vector of ball 2 to retrieve the original coordinate system       
    b1->v=Vec3f(ct*cp*vx1r-sp*vy1r+st*cp*vz1r +vx2,ct*sp*vx1r+cp*vy1r+st*sp*vz1r+vy2,ct*vz1r-st*vx1r+vz2);
    b2->v=Vec3f(ct*cp*vx2r-sp*vy2r+st*cp*vz2r+vx2,ct*sp*vx2r+cp*vy2r+st*sp*vz2r+vy2,ct*vz2r-st*vx2r+vz2);  
       return;
}
void angleFreeCollision3D(BallPair pair){

    Ball *b1=pair.bA;
    Ball *b2=pair.bB;

    float m1=b1->m;
    float m2=b2->m;

    Vec3f v1=b1->v;
    Vec3f v2=b2->v;  

    Vec3f v12=v1-v2;
    Vec3f v21=v2-v1;
    Vec3f pos12=b1->pos-b2->pos;
    Vec3f pos21=b2->pos-b1->pos;  

    float x12= pos12.magnitudeSquared();
    float x21= pos21.magnitudeSquared();  

    b1->v -= pos12*(v12.dot(pos12)/x12)*((2*m2)/(m1+m2)); 
    b2->v -= pos21*(v21.dot(pos21)/x21)*((2*m1)/(m1+m2));
}
bool check_wall_collision(BallWallPair pair){
  
  Ball *b = pair.ball;
  Vec3f dir = pair.wall->direction;
   
  if( b->pos.dot(dir) + b->r >= BOX_SIZE / 2 && b->v.dot(dir)>0)pair.wall->t_red=RED; 
  
  return b->pos.dot(dir) + b->r >= BOX_SIZE / 2 && b->v.dot(dir) > 0;  
}
void ideal_responce(BallWallPair *pair){
  
   Ball *b = pair->ball;
   Wall *w = pair->wall;

   Vec3f dir = w->direction.normalize();
   b->v -= 2 * dir * b->v.dot(dir);

}
Ball *elongate_list(int max, Ball *head){
  
   Ball* temp=NULL;
   Ball* ball;
   
   for(int i = 0; i < max; i++) {
    ball = new Ball();
   
    ball->r = 0.1f * randomFloat() + 0.1f;
    ball->m=ball->r;
    ball->pos = Vec3f((BOX_SIZE-3) * randomFloat() - 4, (BOX_SIZE-3) * randomFloat() - 4, 8 * randomFloat() -4);
    ball->v = Vec3f((BOX_SIZE-3) * randomFloat() - 4,(BOX_SIZE-3) * randomFloat() - 4, (BOX_SIZE-3) * randomFloat() - 4);
    //ball->color = Vec3f(0.6f * randomFloat() + 0.2f, 0.6f * randomFloat() + 0.2f, 0.6f * randomFloat() + 0.2f);//odkomentirati za loptice u boji
    ball->next=NULL;
    
    temp->next = ball;

    temp = ball;
     
  }
  return head;
}

Ball *create_list(int max){   
    Ball* head;
    Ball* temp=NULL; 
    Ball* ball;
    
  for(int i = 0; i < max; i++) {
      ball = new Ball();
	
      ball->r = 0.1f * randomFloat() + 0.1f;
      ball->m=ball->r;
      ball->pos = Vec3f((BOX_SIZE-3) * randomFloat() - 4, (BOX_SIZE-3) * randomFloat() - 4, 8 * randomFloat() -4);
      ball->v = Vec3f((BOX_SIZE-3) * randomFloat() - 4,(BOX_SIZE-3) * randomFloat() - 4, (BOX_SIZE-3) * randomFloat() - 4);
      //ball->color = Vec3f(0.6f * randomFloat() + 0.2f, 0.6f * randomFloat() + 0.2f, 0.6f * randomFloat() + 0.2f);//odkomentirati za loptice u boji
      ball->next=NULL;
      
      if(!temp)head=ball;
      else  temp->next = ball;

      temp = ball;

  }
  return head;
}
void applyGravity(Ball *head) {
  Ball *temp;
  for(temp=head; temp!=NULL; temp=temp->next){
    pullBall(temp);
  }
}
void moveAllBalls(Ball *head, float t){
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
		
		  glColor3f(1, (-temp->t_red/RED)+1,(-temp->t_red/RED)+1);  
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
	  
	  // if(check_collision(bp))collision3D(bp,temp2->v[0],temp2->v[1],temp2->v[2]);
	    if(check_collision(bp))angleFreeCollision3D(bp);
	    num_tests_list++;
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
	    
	    if(check_wall_collision(bwp) && box)ideal_responce(&bwp);
	    if(check_wall_collision(bwp) && !box)ideal_responce(&bwp);
	}
    }
}
void elastic_collision(){
  glPushMatrix();
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
    
   glPushMatrix();
      glTranslatef(0,BOX_SIZE/2,0);
      drawBigBall(ball1); 
      drawBigBall(ball2); 
    glPopMatrix();    

}
void multiple_balls_collision(){ 
     drawBalls(AllBalls); 
}
int central=0;
void init_central(){  
  
  elev=17.8;
  delev=0;
  azim=0; 
  dazim=0;
    
  switch(central){
    
    case 1:
	    /*
	    U slučaju kad je m1 = m2 kugle jednostavno zamijene brzine v 1' = v2 i v2 ' = v 1 . Ako
	    druga kugla miruje (v2 = 0) , tada je i v 1 ' = 0 , a v2 ' = v 1 ; nakon sudara prva kugla
	    stane dok druga odleti brzinom koju je prije sudara imala prva kugla.
	    */
	    //ball 1
	    ball1.m=0.7f;
	    ball1.r=0.7f; 
	      //ball 2
	    ball2.r=0.7f;
	    ball2.m=0.7f;
	    
	    ball1.v=Vec3f(4,0,0);  
	    ball2.v=Vec3f(0,0,0); 
	    
	    break;
    case 2: 
	    /*
	    U slučaju kad je m1 << m2, ( v2 = 0) , tj. kad savršeno elastična kugla mase m1 i
	    brzine v1 udara u vrlo veliku kuglu ili savršeno elastični zid. Slijedi da je
	    v 1 ' = − v 1 , tj. kugla se odbija jednakom brzinom kojom je došla, a zid pri tom
	    dobije impuls sile 2 m 1v 1 . Zid ne dobije nikakvu energiju jer kugla prilikom sudara
	    ne mijenja energiju. Ukupna promjena količine gibanja kugle je 2 m 1v 1 .
	    */ 
	    //ball 1
	    ball1.m=0.4f;
	    ball1.r=0.4f; 
	    //ball 2
	    ball2.m=300;
	    ball2.r=3;
	
	    ball1.v=Vec3f(4,0,0);  
	    ball2.v=Vec3f(0,0,0); 
	    break;
    case 3 : 
	    /*
	    U slučaju kad je m1 >> m2, (v2 = 0) , tj. kad vrlo velika kugla mase m_1 udari u kuglicu
	    koja miruje. Pri tom se brzina velike kugle vrlo malo promijeni dok lagana
	    kuglica odleti brzinom 2 puta većom od brzine upadne kugle.
	    */
	    ball1.m=300;
	    ball1.r=3; 
	    
	    ball2.m=0.4f;
	    ball2.r=0.4f;
	    
	    ball1.v=Vec3f(4,0,0);  
	    ball2.v=Vec3f(0,0,0); 
	    break;
  }
    
      ball1.pos=Vec3f(-4,-BOX_SIZE/2+ball1.r,0);
      ball1.color= Vec3f(1,0,0);
      
      ball2.pos=Vec3f(4,-BOX_SIZE/2+ball2.r,0);
      ball2.color= Vec3f(0,0,1);
      
      bp.bA = &ball1;
      bp.bB = &ball2; 
}

void init(){

    switch(selection){
      
	case 1: //2 balls
		cout<<"Type 'a' to change balls atributs"<<endl;
		cout<<"Type 'p' to pause/unpause"<<endl;
		pause = 1;    
		ball1.color= Vec3f(1,0,0);// ball1 = red
		ball2.color= Vec3f(0,0,1);//ball2 = blue     
		
		bp.bA = &ball1;
		bp.bB = &ball2;
	      
		bottom.direction = Vec3f(0, -1, 0);
	      
		bw.ball = &ball2;
		bw.wall = &bottom;
	    
		bw2.ball = &ball1;
		bw2.wall = &bottom;
		break;
	
	case 2: //Loptice u listi AllBalls
		cout<<"Type 'w' to switch drawing walls"<<endl;
		cout<<"Type 'p' to pause/unpause"<<endl;
		elev=0;
		delev=0;
		azim=0; 
		dazim=0;
		break;
	    
	case 3: //Loptice u oktalnom stablu
		cout<<"Type 'w' to switch drawing walls"<<endl;
		cout<<"Type 'o' to switch drawing octree on/off"<<endl;
		cout<<"Type 'p' to pause/unpause"<<endl;

		elev=0;
		delev=0;
		azim=0; 
		dazim=0;
		break;
    }

  
	//zidovi
	l.direction = Vec3f(-1, 0, 0);
	r.direction = Vec3f(1, 0, 0);
	f.direction = Vec3f(0, 0, -1);
	n.direction = Vec3f(0, 0, 1);
	c.direction = Vec3f(0, 1, 0);
	b.direction = Vec3f(0, -1, 0);
}
///////////////////////////////////////////////////
void drawOctree(Octree* octree, float box_size){

      if (octree->hasChildren){  
	 for(unsigned int i=0;i<2;i++){
	    for(unsigned int j=0;j<2;j++){
		for(unsigned int k=0;k<2;k++){

		drawOctree(octree->children[i][j][k], box_size/2);
		}
	    }
	}
      }else if(!octree->hasChildren){
	
	glPushMatrix();
	  glDisable(GL_LIGHTING);

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
void potentialBallBallCollisions(vector<BallPair> &potentialCollisions,vector<Ball*> &balls, Octree* octree) {
	octree->potentialBallBallCollisions(potentialCollisions);
}

//Puts potential ball-wall collisions in potentialCollisions.  It must return
//all actual collisions, but it need not return only actual collisions.
void potentialBallWallCollisions(vector<BallWallPair> &potentialCollisions, vector<Ball*> &balls, Octree* octree) {
	//Fast method
	octree->potentialBallWallCollisions(potentialCollisions);
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

//Handles all ball-ball collisions
void handleBallBallCollisions(vector<Ball*> &balls, Octree* octree) {
	vector<BallPair> bps;
	
	potentialBallBallCollisions(bps, balls, octree);
	vector<Ball*> redballs;
	
	for(unsigned int i = 0; i < bps.size(); i++) {
		BallPair bp = bps[i];
		/*
		//Ne prikazuje elastični sudar jer ne uzima u obzir mase loptica
		Ball* b1 = bp.bA;
		Ball* b2 = bp.bB;
		
		if (testBallBallCollision(b1, b2)) {
			//Make the balls reflect off of each other
			Vec3f displacement = (b1->pos - b2->pos).normalize();
			b1->v -= 2 * displacement * b1->v.dot(displacement);
			b2->v -= 2 * displacement * b2->v.dot(displacement);
		}
		*/
		if(check_collision(bp))angleFreeCollision3D(bp);
		num_tests_octree++;
	}
}
bool testBallWallCollision(Ball* ball, Wall* wall) {
	Vec3f dir = wall->direction;
	//Check whether the ball is far enough in the "dir" direction, and whether it is moving toward the wall
	//return ball->pos.dot(dir) + ball->r > BOX_SIZE / 2 && ball->v.dot(dir) > 0;
	
	if( ball->pos.dot(dir) + ball->r >= BOX_SIZE / 2 && ball->v.dot(dir)>0 ){ 
	  wall->t_red=RED;
	}
	return ball->pos.dot(dir) + ball->r >= BOX_SIZE / 2 && ball->v.dot(dir) > 0;
}

//Handles all ball-wall collisions
void handleBallWallCollisions(vector<Ball*> &balls, Octree* octree) {
	
	vector<BallWallPair> bwps;
	potentialBallWallCollisions(bwps, balls, octree);
	
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
	cout<<num_tests_octree<<endl;
	num_tests_octree=0;
}
//Advances the state of the balls by t.  timeUntilUpdate is the amount of time
//until the next call to performUpdate.
void advance(vector<Ball*> &balls,Octree* octree, float t, float &timeUntilUpdate) {
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
	    glColor3f(1, (-ball->t_red/RED)+1,(-ball->t_red/RED)+1);  
	    ball->t_red--;
	  }else{
	  glColor3f(1, 1, 1);
	  //glColor3f(ball->color[0], ball->color[1], ball->color[2]); //odkomentirati za loptce u boji
	  }	
		glutSolidSphere(ball->r, 12, 12); //Draw a sphere
		glPopMatrix();
	}
}
Ball *B=&ball1;
float *a=&B->r;
Vec3f *v=&B->pos;

void handleKeypress(unsigned char key, int x, int y) {
  
  switch (key) {
    case 27: //Escape key
	      cleanup();
	      exit(0);
    case 'm' :	// promijeni masu
	      cout<<'m'<<endl;
	      a=&B->m;
	      break;
    case 'r' : // promijeni radijus
	      cout<<'r'<<endl;
	      a=&B->r;
	      break;
    case 'v' :// promijeni brzinu
	      cout<<'v';
	      v=&B->v;
	      glutKeyboardFunc(handleKeypress3);
	      break;
    case 'p' :  //// promijeni poziciju
	      cout<<"pos"<<endl;
	      v=&B->pos;
	      glutKeyboardFunc(handleKeypress3);
	      break;    
    case '+': 
	      (*a)+=0.1;
	      //if(a==&B->r)B->pos[1]=-BOX_SIZE/2+B->r;
	      if(a==&B->r && B->pos[1]-B->r < -BOX_SIZE/2+B->r )B->pos[1]=-BOX_SIZE/2+B->r;
	      cout<<"r:"<<B->r<<endl;
	      cout<<"m:"<<B->m<<endl;
	      cout<<"v:"<<B->v[0]<<" "<<B->v[1]<<" "<<B->v[2]<<endl;
	      cout<<"pos:"<<B->pos[0]<<" "<<B->pos[1]<<" "<<B->pos[2]<<endl;
	      break;
    case '-': 
	      if((a==&B->m ||a==&B->r)&&(*a)<0.1)break;
	      (*a)-=0.1;
	      if(a==&B->r && B->pos[1]-B->r < -BOX_SIZE/2+B->r )B->pos[1]=-BOX_SIZE/2+B->r;
	      // B->pos[1]=-BOX_SIZE/2+B->r;   
	      cout<<"r:"<<B->r<<endl;
	      cout<<"m:"<<B->m<<endl;
	      cout<<"v:"<<B->v[0]<<" "<<B->v[1]<<" "<<B->v[2]<<endl;
	      cout<<"pos:"<<B->pos[0]<<" "<<B->pos[1]<<" "<<B->pos[2]<<endl;
	      break;
	      
    case 'b' :
	      cout<<"BLUE BALL pick atribute m-mass, r-radius, v-velocity, p- position"<<endl;
	      cout<<"q to return to selection menue"<<endl;
	      B = &ball2;
	      break;
    case 'q': 
	      cout<<"q"<<endl;
	      glutKeyboardFunc(handleKeypress2);
	      break;

  }
    
}
void handleKeypress3(unsigned char key, int x, int y){
     switch (key) {
       case 27: //Escape key
		exit(0);
       
       case 'b' :
		 B = &ball2;
		 break;

	case 'x':
	        cout<<"x"<<endl;
		a=&(*v)[0];
		break;
	case 'y':
		cout<<"y"<<endl;
		a=&(*v)[1];
		break;
	case 'z':
		cout<<"z"<<endl;
		a=&(*v)[2];
		break;
     }
    cout<<(*a)<<endl;
    if(key!='b')glutKeyboardFunc(handleKeypress);
  
}
void handleKeypress2(unsigned char key, int x, int y) {

    switch (key) {
	  case 27: //Escape key
		  exit(0);
	  case '1': selection=1;
		    elev=14.6;
		    delev=0;
		    azim=55; 
		    dazim=0;
		      
		    //ball 1
		    ball1.m=0.2;
		    ball1.r=1.5f; 
		    ball1.pos=Vec3f(0,-BOX_SIZE/2+ ball1.r+10,0);
		    //ball1.v=Vec3f(1,0,1);  
		    ball1.v=Vec3f(0,0,0);  
		    
		    //ball 2
		    ball2.r=0.6;
		    ball2.m=1.5;
		    ball2.pos=Vec3f(0,-BOX_SIZE/2+ball2.r+8,0);
		    //ball2.v=Vec3f(2,0,-2);
		    ball2.v=Vec3f(0,0,0);  
		    
		    init();
		  break;
	  case '2': selection=2;
		    elev=0;
		    delev=0;
		    azim=0; 
		    dazim=0;
		  init();
		  break;
	  case '3': selection=3;
		    init();
		break;
	  case '0':
		  ball1.pos=Vec3f(-5,-BOX_SIZE/2+ ball1.r,-1);
		  ball1.v=Vec3f(1,0,1); 
		  ball2.pos=Vec3f(-5,-BOX_SIZE/2+ball2.r,3);
		  ball2.v=Vec3f(2,0,-2);
		  
		  cout<<selection<<endl;
		  init();
		break;
	  case 'w': if(box==1)box=0;
		    else box=1;
;
		  break;  
	  case 'c': central++;
		    if(central>3)central=1;
		    init_central();
		    break;
		  //Add balls with a random position, velocity, radius, and color
	  case ' ': 
		  if(selection==1)break;	
			AllBalls=create_list(NUM_BALLS+=PLUS_BALL);
			cout<<NUM_BALLS<<endl;
			//break;			  
		  if(selection==2){cout<<NUM_BALLS<<endl;}

		  for(int i = 0; i < 20; i++) {
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
		  } 
		  if(selection==3)cout<<_octree->numBalls<<endl;
		  break;
		  
	  case 'p': if(pause == 1) pause = 0;
		    else if(pause==0) pause = 1;
		    break;
		    
	  case 'o': 
		      if(ot == 1) ot=0;
		      else ot = 1;
		      break;
	  case  'a': 
		    cout<<"RED BALL pick atribute m-mass, r-radius, v-velocity, p- position"<<endl;
		    cout<<"q to return to selection menue"<<endl;
		    B = &ball1;	  
		    glutKeyboardFunc(handleKeypress);
		    break;
	  case 'f' : 
		    d=3;
		    cout<<d<<endl;
	      
		    break;
	  case 'd' : 
		    d=2;
		    cout<<d<<endl;
		    break;
	  case 't' : 
		    d=32;
		    cout<<d<<endl;
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
    
    glTranslatef(0.0, 0.0, -25.0f);
    glRotated(elev+delev, 1.0, 0.0, 0.0);
    glRotated(azim+dazim, 0.0, 1.0, 0.0);

    Lighting(BOX_SIZE);
    glShadeModel(GL_SMOOTH);
    
    glColor3f(1,1,1); 

     switch(selection){
    
      case 1 :   
		elastic_collision();
		break;
      case 2 : 
		//glRotatef(-_angle, 0.0f, 1.0f, 0.0f); //odkomentirati za rotiranje
		if(box)drawCube(BOX_SIZE);
		else glutWireCube(BOX_SIZE);
		multiple_balls_collision();
		break;	
      case 3 : 
	    	//glRotatef(-_angle, 0.0f, 1.0f, 0.0f);//odkomentirati za rotiranje
		if(box==1)drawCube(BOX_SIZE);  
		else glutWireCube(BOX_SIZE);
		
		if(ot==1)drawOctree(_octree, BOX_SIZE);  
		drawBalls();
		break;
    }
    
    glutSwapBuffers();
}


void advanceAllBalls(float t, float &timeUntilUpdate) {
    while (t > 0) {
	if (timeUntilUpdate <= t) {
		applyGravity(AllBalls);
		moveAllBalls(AllBalls, timeUntilUpdate);
		bruteForce(AllBalls);
		cout<<num_tests_list<<endl;
		num_tests_list=0;
		ballsWallsCollisions(AllBalls);
		
		t -= timeUntilUpdate;
		timeUntilUpdate = TIME_BETWEEN_UPDATES;
	}
	else {
		moveAllBalls(AllBalls, t);
		timeUntilUpdate -= t;
		t = 0;
	}
     }
}

void advance2Balls(float t, float &timeUntilUpdate) {
 
  while (t > 0) {
	if (timeUntilUpdate <= t) {
		  
	  pullBall(&ball1);
	  pullBall(&ball2);
	  move_ball(&ball1,timeUntilUpdate);
	  move_ball(&ball2,timeUntilUpdate);
	  
	 
	  
	  if(d==2 && bp.bA->pos[1]-bp.bA->r==0 && bp.bA->pos[1]-bp.bA->r==0){
	   if(check_collision(bp))collision2D(bp); 
	  }else if(d==3 && check_collision(bp))angleFreeCollision3D(bp);
	  else if(check_collision(bp))collision3D(bp,bp.bB->v[0],bp.bB->v[1],bp.bB->v[2]);
	  
	  if(check_wall_collision(bw))ideal_responce(&bw);
	  if(check_wall_collision(bw2))ideal_responce(&bw2);

	  t -= timeUntilUpdate;
	  timeUntilUpdate = TIME_BETWEEN_UPDATES;
	}
	else {
		move_ball(&ball1,t);
		move_ball(&ball2,t);
		timeUntilUpdate -= t;
		t = 0;
	}
   }
}


void update(int value) {
 
    switch(selection){
      
      case 1:
	  if(!pause)advance2Balls((float)TIMER_MS / 1000.0f, _timeUntilUpdate);
	break;
 
      case 2:

	  if(!pause)advanceAllBalls((float)TIMER_MS / 1000.0f, _timeUntilUpdate);
	  
	  _angle += (float)TIMER_MS / 100;
	  if (_angle > 360) {
		_angle -= 360;
	  }
	 
	  break;
	  
      case 3:
	
	if(!pause)advance(_balls, _octree, (float)TIMER_MS / 1000.0f, _timeUntilUpdate);
	
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
    cout<<"Type '1' for 2 balls elastic collision"<<endl;
    cout<<"Type '2' for multiple balls in list (Brute Force algorithm)"<<endl;
    cout<<"Type '3' for multiple balls in octree (Octree space partitioning)"<<endl;
        
    srand((unsigned int)time(0)); //Seed the random number generator	
    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(700, 600);
    glutCreateWindow("Collision Window");
    
    initRendering();
    _octree = new Octree(Vec3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2),Vec3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2), 1);
    for(int i = 0; i < 20; i++) {
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
    } 
    AllBalls=create_list(NUM_BALLS);
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