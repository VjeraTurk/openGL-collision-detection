#include <stdlib.h>
#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdlib.h>
//#include <vector>
#include <math.h>
#include "vector3f.h"
#include "vec3f.h"

#include "lighting.h"
#include "navigation.h"
#include "octree.h"

#define RED 40

#define BOX_SIZE 12
#define MAX_OCTREE_DEPTH 6
const float GRAVITY =  8.0f;

int selection=1;
/*
 struct Ball{
  Vec3f pos; // position
  Vec3f v; //velocity with direction
  
  float m;
  
  float t_red;
  
  Ball* next;
 
  float r;
  Vec3f color;
};
 */

struct BallPair{
  Ball* bA;
  Ball* bB;
  
  BallPair* next;
};

struct Wall{
  
      Vec3f direction;
      float t_red;
};

class BallWallPair {
  
public:
	Ball* ball;
	Wall* wall;
};
//all selections 
const float ALPHA = 0.6f;

//selection 1
 Ball ball1;
 Ball ball2;
 Ball testBall;
 BallPair bp;
 BallWallPair bw, bw2;
 Wall bottom, top;

//selection 2 
 Ball* AllBalls;
 BallPair* AllBallPairs;
 BallPair* AllBallWallPairs;
 Wall l,r,f,n,b,c;

//selection 3
 Node root;

//all
void drawCube(float box_size);
float randomFloat();
void update(int value);
void display();

//selection 1
void elastic_collision();

    void drawBall(Ball ball);
  
    void move_ball(Ball *ball, float dt);
    void pullBall(Ball *ball);
    int check_collision(BallPair pair);
    int check_wall_collision(BallWallPair pair);
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
    glColor3f(ball.color[0],ball.color[1], ball.color[2]);

    glPushMatrix();
    
    glTranslatef(ball.pos[0],ball.pos[1],ball.pos[2]);
    glutSolidSphere(ball.r,20,20);
  
    glPopMatrix();
    
}
const float TIME_BETWEEN_UPDATES = 0.01f;
const int TIMER_MS = 25; //The number of milliseconds to which the timer is set
const float t =0.01f;

void move_ball(Ball *ball, float dt){
  
    glPushMatrix();
    ball->pos+=ball->v * dt;
    //cout <<"moved"<<endl;
    glPopMatrix();
  
}

void pullBall(Ball *ball){
  ball->v-= Vec3f(0, GRAVITY * TIME_BETWEEN_UPDATES, 0);
}

int check_collision(BallPair pair){
   
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

void responce(BallPair pair){
   Ball *bA = pair.bA;
   Ball *bB = pair.bB;
  
   Vec3f displacement = (bA->pos - bB->pos).normalize();
  bA->v -= 2 * displacement * bA->v.dot(displacement);
  bB->v -= 2 * displacement * bB->v.dot(displacement);
}

int check_wall_collision(BallWallPair pair){
  
  Ball *b = pair.ball;
  Wall *w = pair.wall;
  
  Vec3f dir = w->direction;
  
  if( b->pos.dot(dir) + b->r >= BOX_SIZE / 2 && b->v.dot(dir) > 0){
   w->t_red=50.0f;
   return 1;
  }
   return 0;
}

void w_responce(BallWallPair pair){
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

void drawBalls(Ball *head){
  
  Ball *temp;
  
 for(temp=head; temp!=NULL; temp = temp->next){
    glPushMatrix();
	
      glTranslatef(temp->pos[0], temp->pos[1], temp->pos[2]);
		
		if(temp->t_red){
		
		  glColor3f(1, (-temp->t_red/50)+1,(-temp->t_red/50)+1);  
		  temp->t_red--;
		  
		}else{
		  
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
// l,r,f,n,b,c;
Wall *walls[]={&l,&r,&f,&n,&b,&c};
void ballsWallsCollisions(Ball *head){
  
	Ball *temp;
	 
	for(temp=head; temp!=NULL; temp = temp->next){
		
	    for(int j = 0; j < 6; j++) {
			
			BallWallPair bwp;
			bwp.ball = temp;
			bwp.wall = walls[j];
			
			if(check_wall_collision(bwp))w_responce(bwp);
		}
	}

}
	

void update(int value) {
 
    switch(selection){
      
      case 1:
	
           move_ball(&ball1,t);
	   move_ball(&ball2,t);
	   pullBall(&ball1);
	   pullBall(&ball2);
	   
	   if(check_collision(bp)) responce(bp);
	   if(check_wall_collision(bw))w_responce(bw);
           if(check_wall_collision(bw2))w_responce(bw2);
	  
	break;
 
      case 2:
	  moveBalls(AllBalls);
	  applyGravity(AllBalls);
	  bruteForce(AllBalls);
	  ballsWallsCollisions(AllBalls);
	 
	  break;
      case 3:
	
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
void balls_in_octree(){
  
    drawBall(testBall);
    drawOctree(&root);
    
  
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
   // glTranslatef(0.0, 0.0, -(dis+ddis));

    glTranslatef(0.0, 0.0, -30.0f);

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
	      drawCube(BOX_SIZE);  
	      multiple_balls_collision();
		break;	
      case 3 : balls_in_octree();
    }

    
    
    
    glutSwapBuffers();
}

 void init(){
     
   //ball 1
    ball1.pos=Vec3f(-2,0.4,0);
    ball1.v=Vec3f(4,0,0);  
    
    ball1.r=0.4f;
    ball1.color= Vec3f(1,0,0);
    
    //ball 2
    ball2.pos=Vec3f(2,0.4,0);
    ball2.v=Vec3f(-4,0,0);  
   
    ball2.r=0.4f;
    ball2.color= Vec3f(1,1,0);
    
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
	
   //test ball
 
    testBall.pos=Vec3f(3,0,0);
    testBall.r=0.5;
    testBall.color=Vec3f(0,1,0);
    root.center=Point(0,0,0);//radi
    root.halfWidth=12;
    //root.center=Point(root.halfWidth,0,0);
    
    BuildOctree(root.center,root.halfWidth,1);   

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
		case 'r' : testBall.pos=Vec3f(root.center.x,root.center.y,root.center.z);
		      break;
		case '1': selection=1;
			break;
		case '2': selection=2;
		      break;
		case '3': selection=3;
		      break;
		case '0':
			init();
		  break;
	  
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
    
    glutKeyboardFunc(handleKeypress2);
    
    glutReshapeFunc(handleResize);
    glutMouseFunc(cb_mouse);
    glutMotionFunc(cb_motion);
  
    glutTimerFunc(TIMER_MS, update, 0);
	

    glutMainLoop();

    return(0);
}
