#include <GL/glut.h>

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			//TwTerminate();
			exit(0);
	}
}

int mode;
double beginx, beginy;
double dis = 10.0, azim = 0.0, elev = 0.0;
double ddis = 0.0, dazim = 0.0, delev = 0.0;

void cb_mouse(int _b, int _s, int _x, int _y)
{
  if (_s == GLUT_UP)
  {
    dis += ddis;
    if (dis < .1) dis = .1;
    azim += dazim;
    elev += delev;
    ddis = 0.0;
    dazim = 0.0;
    delev = 0.0;
    return;
  }

  if (_b == GLUT_RIGHT_BUTTON)
  {
    mode = 0;
    beginy = _y;
    return;
  }
  else
  {
    mode = 1;
    beginx = _x;
    beginy = _y;
  }
}
void cb_motion(int _x, int _y)
{
  if (mode == 0)
  {
    ddis = dis * (double)(_y - beginy)/200.0;
  }
  else
  {
    dazim = (_x - beginx)/5;
    delev = (_y - beginy)/5;      
  }
  
  glutPostRedisplay();
}