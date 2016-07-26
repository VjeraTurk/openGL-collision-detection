/*
 cube.h 
 */
//Three perpendicular vectors for a face of the cube.  out indicates the
//direction that the face is from the center of the cube.
struct Face {
	Vec3f up;
	Vec3f right;
	Vec3f out;
};
//Represents a cube.
struct Cube {
	Face top;
	Face bottom;
	Face left;
	Face right;
	Face front;
	Face back;
};

//Initializes the up, right, and out vectors for the six faces of the cube.
void initCube(Cube &cube);

//Rotates the vector by the indicated number of degrees about the specified axis
Vec3f rotate(Vec3f v, Vec3f axis, float degrees);

//Rotates the face by the indicated number of degrees about the specified axis
void rotate(Face &face, Vec3f axis, float degrees);

//Rotates the cube by the indicated number of degrees about the specified axis
void rotate(Cube &cube, Vec3f axis, float degrees);
//Returns whether face1 is in back of face2.
bool compareFaces(Face* face1, Face* face2);
//Stores the four vertices of the face in the array "vs".
void faceVertices(Face &face, Vec3f* vs) ;

void drawTopFace(Face &face);
void drawBottomFace(Face &face);
void drawFrontFace(Face &face);
void drawBackFace(Face &face);
void drawLeftFace(Face &face);
void drawRightFace(Face &face);

//Draws the indicated face on the specified cube.
void drawFace(Face* face, Cube &cube);

void drawCube(Cube &cube);