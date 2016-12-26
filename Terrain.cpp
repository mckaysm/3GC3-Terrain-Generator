
/*
Name: Sean McKay
Course Code: CS 3GC3
Student number: 1423885
Student id: mckaysm
Program name: terrain.cpp
----Program description-----
Renders a 500x500 pixel random terrain map using the circles algorithm
to displace points upwards, with lighting from two lighting sources at either side of the terrain.
Terrain is shaded based on its height. 
Lighting, wireframe vs. solid rendering, and triangle vs. quad rendering is toggleable
Commands for toggling features printed to the shell 

------Extra features--------
1. Switch between rendering a new terrain using the fault or circles algorithm
2. 2D heightmap of the terrain in a second window
3. Terrain modifiable from the second window
			


*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "math2D.h"
#include <vector>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

using namespace std;

//Main window paramaters
int main_id;
int mainHeight = 720;
int mainWidth = 1280;

//Editing window paramaters
int editor_id;
int editorWidth = 500;
int editorHeight = 500;

//Viewing paramaters
float eye[] = {0,100,100};

//Parameters for circles when making the circle heightmap
int numCircles = 200;
//Maximum height of a single circle
int maxCircleHeight = 50;
//Maximum radius of a single circle
int maxRad = 100;


//Paramaters for making the fault height map
int numIterations = 1000;
//First fault line displaces terrain by this amount
float maxFaultHeight = 10;
//The last fault line displaces terrain by this amount
float minFaultHeight = 0.1;
//Iterations past this amount will displace terrain by the minimum amount
int minIterations = 100;


//Size of terrain map
int xSize = 500;
int zSize = 500;

//Counters for rotating camera
int yRotation = 0;
int xRotation = 0;



//Heightmap to store y values 
float heightMap[500][500];
//Normal vectors of each vertex
vec3D normals[500][500];
//Normal vectors of faces of first half of triangles created
vec3D f1Normals[500][500];
//Normal vectors of faces of second half of triangles created
vec3D f2Normals[500][500];

//Max and min height in the heightMap (for shading)
float top = -1000000.0;
float bot =  1000000.0;

//Lighting information
float light0_pos[] = {0,200,0,0.5};
float light1_pos[] = {(float)xSize, 200, (float)zSize, 0.5};

//Lighting values for light0
float amb0[4]  = {1, 1, 1, 1}; 
float diff0[4] = {0.2, 0.2, 0.2, 1}; 
float spec0[4] = {0, 0.2, 0, 0};

//Lighting values for light1
float amb1[4]  = {1, 1, 1, 1}; 
float diff1[4] = {0.2, 0.2, 0.2, 1}; 
float spec1[4] = {0, 0, 0.2, 1};

//Material values for the terrain
float m_amb[] = {1, 1, 1, 1.0}; 
float m_diff[] = {1, 1, 1, 1.0}; 
float m_spec[] = {0.1, 0.1, 0.1, 1.0}; 
float shiny = 0; 


//Wireframe toggle
bool wired = false;
bool solid = true;

//Quads toggle
bool quads = false;

//Lighting toggle
bool lighting = true;
bool perFace = false;

//Circle vs Fault algorithm toggle
bool fault = false;

//Toggle for erasing vs creating mode in editor window
bool erasing = false;

//TOggles for switching between moving the camera and moving light sources around
bool cam = true;
bool light0 = false;
bool light1 = false;

//Toggle for editing data per-vertex or in circles of specifiable size using the circle algorithm
bool singleVertex = false;
float editCircleRadius = 10.0;
float editCircleHeight = 10.0;

//Position of the cursor
int cursorX = 0;
int cursorY = 0;

//Flatland for resetting height map
void initializeEmptyHeightMap () {
	for (int x = 0; x < xSize; x++) {
		for (int z = 0; z < zSize; z++) {
			heightMap[x][z] = 0;
		}
	}

}

//Use the circle algorithm to calculate the height for a given (x,z) index with given circle centers
float calcHeight(int x, int z, int radii[], float heights[], point2D centers[], int circles) {
	float height = 0.0;
	point2D coord = point2D(x, z);
	float d; 
	float pd;

	//Iterate through each circle in the list of circlecenters
	for (int i = 0; i < circles; i++) {
		//Caulculate the distance from the given coordinate to the circle center
		d = coord.distance(centers[i]);

		pd = d * 2 / radii[i];

		//If the point falls within the radius of the circle, increase it's height
		//The closer it is to the circle center, the more it's increased, up to the max height for that circle
		if ((fabs) (pd) <= 1.0) {
			height += (heights[i] / 2.0) + ((cos(pd * 3.14)) * (heights[i] / 2.0));
		}

	}

	//Return the corresponding height of the given (x, z) coordinate
	return height;

}


//Fill the heightmap with y values for each corresponding (x,z) point 
//Using the circle algorithm with a given number of iterations to calculate the height of each point
void circleHeightMap (int circles, int maxHeight, int maxRad) {

	//List of coordinates of the circles' centers to use in the circle algorithm
	point2D circleCenters[circles];
	//Corresponding list of radii for the circles
	int radii[circles];
	//Corresponding list of heights for the circles
	float heights[circles];

	//Randomly pick (x, z) coordinates of the centers of the circles
	//Randomly set the radius to a value between 10 and maxRad
	//Randomly set the height to a value between 0 and maxHeight
	srand(time(NULL));
	for (int i = 0; i < circles; i++) {
		int xPos = rand() % xSize;
		int zPos = rand() % zSize;
		circleCenters[i] = point2D(xPos, zPos);
		radii[i] = (rand() % maxRad) + 10;
		heights[i] = rand() % maxHeight;
	}

	//Get the height values for each (x, z) coordinate, using the circle algorithm
	float h = 0.0;
	for (int x = 0; x < xSize; x++) {
		for (int z = 0; z < zSize; z++) {
			h = calcHeight(x, z, radii, heights, circleCenters, circles);
			heightMap[x][z] += h;
			//If the height value is greater than the max height, set it equal to the max height
			if (h > top) top = h;
			//If the height value is lower than the min height, set it equal to the min height
			if (h < bot) bot = h;
			
		}
	}
}

//Displace points around a single circle with given radius, height, and (x, z) position
//Used to alter the terrain after it has been generated
void addCircleToHeightMap(int radius, float height, int cX, int cZ) {
	//Position, radius, and height of the single circle
	//Stored as arrays so they can be passed to the calcHeight function
	point2D circleCenters[1] = {point2D(cX, cZ)};
	int radii[1] = {radius};
	float heights[1] = {height};
	float h;

	//Get the height values for each (x, z) coordinate, using the circle algorithm
	for (int x = 0; x < xSize; x++) {
		for (int z = 0; z < zSize; z++) {
			h = calcHeight(x, z, radii, heights, circleCenters, 1);
			heightMap[x][z] += h;
			//If the height value is greater than the max height, set it equal to the max height
			if (h > top) top = h;
			//If the height value is lower than the min height, set it equal to the min height
			if (h < bot) bot = h;
			
		}
	}

}

//Each iteration draws a line that crosses through two points in the heightmap, 
//Points on one side of the line are displaced down, and ones on the other upwards
void faultHeightMap(int iterations, float maxDisplacement, float minDisplacement, int minI) {

	//Use maxDisplacement initially
	float disp = maxDisplacement;
	//Scaling factor for displacements after the first
	float scale = (maxDisplacement - minDisplacement) / (iterations - minI);
	int x1, x2, z1, z2;
	
	//Seed the random generator
	srand(time(NULL));
	//Set the displacement based on the iteration number using the scaling factor
	//If the number of iterations is greater than minI, set it equal to the minimum displacement
	for (int i = 0; i < iterations; i++) {
		if (i < minI) {
			disp = disp - scale;
		} else {
			disp = minDisplacement;
		}


		//Randomly pick two (x,z) points
		x1 = rand() % xSize;
		x2 = rand() % xSize;
		z1 = rand() % zSize;
		z2 = rand() % zSize;


		//Use the sign of the y component of the crossproduct of the vector ((x2 - x1), (z2 - z1)) with ((tx - x1), (tz - z1)) 
		//to determine whether to displace each point (tx, tz) upwards or downwards
		for (int tx = 0; tx < xSize; tx++) {
			for (int tz = 0; tz < zSize; tz++) {
				if (((x2 - x1) * (tz - z1) - (z2 - z1) * (tx - x1)) > 0) {
					heightMap[tx][tz] += disp;
					if (heightMap[tx][tz] > top) top = heightMap[tx][tz];
				} else {
					heightMap[tx][tz] -= disp;
					if (heightMap[tx][tz] < bot) bot = heightMap[tx][tz];

				}
			}
		}
		
	}
}




//Calculate normal vectors for each trianglular face in the heightmap, along with each individual vertex
void normVectors() {

	//Three points to make up a triangle
	point3D A, B, C;
	//BmA = B minus A, CmA = C minus A, fnorm is the normalized vector for a face
	vec3D BmA, CmA, fnorm;
	for (int i = 0; i < xSize - 1; i++) {
		for (int j = 0; j < zSize - 1; j++) {

			//Define a triangle starting at (i,j), connecting two other succesive points in ccw order
			A = point3D(i, heightMap[i][j], j);
			B = point3D(i, heightMap[i][j+1], j+1);
			C = point3D(i + 1, heightMap[i + 1][j + 1], j + 1);

			//Calculate the edge vectors between two sets of points in the triangle
			BmA = BmA.createVector(B, A);
			CmA = CmA.createVector(C, A);

			//Calculate the crossproduct of the two edges, then normalize that 
			fnorm = BmA.crossProduct(CmA);
			fnorm = fnorm.normalize();	

			//Store the normalized crossproduct as the 1st face normal for that coordinate
			f1Normals[i][j] = fnorm;
		}
	}



	for (int i = 0; i < xSize - 1; i++) {
		for (int j = 0; j < zSize - 1; j++) {

			//Define a triangle starting at (i,j), connecting two other points in ccw order
			A = point3D(i, heightMap[i][j], j);
			B = point3D(i + 1, heightMap[i + 1][j + 1], j + 1);
			C = point3D(i + 1, heightMap[i + 1][j], j);

			//Calculate the edge vectors between two sets of points in the triangle
			BmA = BmA.createVector(B, A);
			CmA = CmA.createVector(C, A);

			//Calculate the crossproduct of the two edges, then normalize that 
			fnorm = BmA.crossProduct(CmA);
			fnorm = fnorm.normalize();

			//Store the normalized crossproduct as the 2nd face normal for that coordinate
			f2Normals[i][j] = fnorm;
		}
	}

	vec3D s, t, sum ;

	for (int i = 0; i < xSize - 1; i++) {
		for (int j = 0; j < zSize - 1; j++) {
			//Take the two face normals for triangles with their origin at each coordinate
			s = f1Normals[i][j];
			t = f2Normals[i][j];
			//Add the two normals together
			sum = s.addVector(t);
			//Add any other face normals of triangles who have an edge at i, j
			if (i > 0) { 
				if (j > 0) {
					sum = sum.addVector(f1Normals[i - 1][j - 1]);
					sum = sum.addVector(f2Normals[i - 1][j - 1]);
				}
				sum = sum.addVector(f2Normals[i - 1][j]);
			
			}
			if (j > 0) {
				sum = sum.addVector(f1Normals[i][j - 1]);
			}

			//Normalize the sum of all face normals for triangles with an edge at i,j
			sum = sum.normalize();
			//Store that value as the vertex normal for that coordinate
			normals[i][j] = sum;
		}
	}

}
		

//Draw function for the main window
void drawTerrain() {

	glPushMatrix();
	glRotatef(10 * yRotation, 0, 1, 0);
	glRotatef(10 * xRotation, 1, 0, 0);

	glTranslatef(-xSize/2, 0, -zSize/2);

	//Normal vectors for each face in th
	vec3D norm1, norm2, norm3, norm4;
	GLint previous[2];
	glGetIntegerv(GL_POLYGON_MODE, previous);
	float col = 1.0;
	if (previous[1] == GL_FILL) {
		col = 0.0;
	}

	//Enables lighting
	if (lighting) {
		glEnable(GL_LIGHTING);
		//Allows glColor() calls to set the ambient lighting colour for materials
		glColorMaterial(GL_FRONT, GL_AMBIENT);
		glEnable(GL_COLOR_MATERIAL);
	}
	

	//Disables lighting
	if (!lighting) {
		glDisable(GL_LIGHTING);
	}


	//Scale for determining the colour of a vertex based on its' height
	float scale = 1.0 / (top - bot);

	if (!quads) {

		//Render the terrain using triangles
		glBegin(GL_TRIANGLES);

		for (int i = 0; i < xSize - 1; i++) {
			for (int j = 0; j < zSize - 1; j++) {
				//Per-Face normal vectors for the three points in the triangle
				if (perFace) {
					norm1 = f1Normals[i][j];
					norm2 = f1Normals[i][j + 1];
					norm3 = f1Normals[i + 1][j + 1];
				}
				//Per-Vertex normal vectors for the three points in the triangle
				else {
					norm1 = normals[i][j];
					norm2 = normals[i][j + 1];
					norm3 = normals[i + 1][j + 1];
				}
				
				//Set the normals for, colour, and then draw the the triangle
				glNormal3f(norm1.x, norm1.y, norm1.z);
				glColor3f((heightMap[i][j] * scale) - (bot * scale), col, col);
				glVertex3f(i, heightMap[i][j], j);

				glNormal3f(norm2.x, norm2.y, norm2.z);
				glColor3f((heightMap[i][j+1] * scale) - (bot * scale), col, col);
				glVertex3f(i, heightMap[i][j + 1], j + 1);

				glNormal3f(norm3.x, norm3.y, norm3.z);
				glColor3f((heightMap[i+1][j+1] * scale) - (bot * scale), col, col);
				glVertex3f(i + 1, heightMap[i + 1][j + 1], j + 1); 

			}

		}
		for (int i = 0; i < xSize - 1; i++) {
			for (int j = 0; j < zSize - 1; j++) {
				//Per-Face normal vectors for the three points in the triangle
				if (perFace) {
					norm1 = f1Normals[i][j];
					norm2 = f1Normals[i][j + 1];
					norm3 = f1Normals[i + 1][j + 1];
				}

				//Per-Vertex normal vectors for the three points in the triangle
				else {
					norm1 = normals[i][j];
					norm2 = normals[i + 1][j];
					norm3 = normals[i + 1][j + 1];
				}

				//Set the normals for, colour, and then draw the the triangle
				glNormal3f(norm1.x, norm1.y, norm1.z);
				glColor3f((heightMap[i][j] * scale) - (bot * scale), col, col);
				glVertex3f(i, heightMap[i][j], j); 

				glNormal3f(norm3.x, norm3.y, norm3.z);
				glColor3f((heightMap[i+1][j+1] * scale) - (bot * scale), col, col);
				glVertex3f(i + 1, heightMap[i + 1][j + 1], j + 1);

				glNormal3f(norm2.x, norm2.y, norm2.z);
				glColor3f((heightMap[i+1][j] * scale) - (bot * scale), col, col);
				glVertex3f(i + 1, heightMap[i + 1][j], j);

			}


		}

	glEnd();

	} else {

		//Render the terrain using quad primitives
		glBegin(GL_QUADS);
		for (int i = 0; i < xSize - 1; i++) {
			for (int j = 0; j < zSize - 1; j++) {
				//Per-Vertex normal vectors for the four points in the quads
				norm1 = normals[i][j];
				norm2 = normals[i][j + 1];
				norm3 = normals[i + 1][j + 1];
				norm4 = normals[i + 1][j];


				//Set the normals for colour and draw the quad
				glNormal3f(norm1.x, norm1.y, norm1.z);
				glColor3f(heightMap[i][j] * scale, col, col);
				glVertex3f(i, heightMap[i][j], j);

				glNormal3f(norm2.x, norm2.y, norm2.z);
				glColor3f(heightMap[i][j+1] * scale, col, col);
				glVertex3f(i, heightMap[i][j + 1], j + 1);

				glNormal3f(norm3.x, norm3.y, norm3.z);
				glColor3f(heightMap[i+1][j+1] * scale, col, col);
				glVertex3f(i + 1, heightMap[i + 1][j + 1], j + 1);

				glNormal3f(norm4.x, norm4.y, norm4.z);
				glColor3f(heightMap[i+1][j] * scale, col, col);
				glVertex3f(i + 1, heightMap[i + 1][j], j);
			}

		}

	glEnd();

	}

	glPopMatrix();
	
}

//Initialize variables for the main window
void init() {

	//Enable lighting for the two light sources in the scene
	glEnable(GL_LIGHTING); 
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	//Set the lighting of the first light using global variables
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0); 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0); 
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

	//Set the lighting of the second light using global variables
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb1); 
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1); 
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);

	//Reset the terrain along with the max and min values for shading 
	initializeEmptyHeightMap();
	top = 0.0f;
	bot = 0.0f;

	//Draw the terrain either using the circle or fault height map
	if (!fault) {
		circleHeightMap(numCircles, maxCircleHeight, maxRad);
	} else {
		faultHeightMap(numIterations, maxFaultHeight, minFaultHeight, minIterations);
	}
	//Calculat the normal vectors for each face in the terrain
	normVectors();
}

//Initialize variables for the editor window
void initEditor() {

	//Set the coordinate system for the editor window
	gluOrtho2D(0,xSize, 0, zSize);
	glClear(GL_COLOR_BUFFER_BIT);

}

//GLUT_KEYBOARD_FUNC functions for the main window
void kbd(unsigned char key, int x, int y)
{
	switch(key) {
		//Quit the program
		case 'q':
		case 'Q':
			exit(0);
			break;

		//Reset the terrain
		case 'r':
		case 'R':

			//Refresh the main window
			init();
			glutPostRedisplay();

			//Refresh the second window
			glutSetWindow(editor_id);
			glutPostRedisplay();

			break;

		//Draw terrain using triangles
		case 't':
		case 'T':
			if (quads) {
				quads = false;
				glutPostRedisplay();
			}
			break;

		//Draw terrain using quads
		case 'y':
		case 'Y':
			if (!quads) {
				quads = true;
				glutPostRedisplay();
			}
			break;


		//Toggle wireframe mode
		case 'w':
		case 'W':
			if (wired && solid) {
				wired = false;
				solid = true;
			} else if (solid) {
				wired = true;
				solid = false;
			} else {
				solid = true;
				wired = true;
			}
			glutPostRedisplay();
			break;

		//Toggle lighting
		case 'l':
		case 'L':
			if (lighting && !(perFace)) {
				perFace = true;
				printf("Using per-face lighting\n");
			} else if (lighting) {
				lighting = false;
				printf("Lighting is off \n");
			} else {
				lighting = true;
				perFace = false;
				printf("Using per-vertex lighting\n");
			}
			glutPostRedisplay();
			break;

		//Changing between circle algorithm and fault algorithm heightmap generation
		case 'u':
		case 'U':
			if (fault) {
				printf("New terrain rendered using circle algorithm\n");
				fault = false;
			} else {
				printf("New terrain rendered using fault algorithm\n");
				fault = true;
			}

			//Refresh the main window
			init();
			glutPostRedisplay();
			//Refresh the second window()
			glutSetWindow(editor_id);
			glutPostRedisplay();
			break;

		case 'o':
		case 'O':
			if (cam) {
				cam = false;
				light0 = true;
				printf("Controlling the first light's movement");
			} else if (light0) {
				light0 = false;
				light1 = true;
				printf("Controlling the second light's movement");
			} else {
				cam = true;
				light1 = false;
				printf("Controlling the camera's movement");
			}
	}
}
void printCommands() {

printf("~~~~~~~~~~~~~~~~~~~~Controls for main window~~~~~~~~~~~~~~~~~~~~~~\n");	

printf("The main window renders the terrain\n");
printf("\n");
printf("Key 'q' quits the program\n");
printf("Key 'r' replaces the current terrain map with a new random one using the same settings\n");
printf("Key 't' renders the current terrain using triangle primitives, if not already\n");
printf("Key 'y' renders the current terrain using quad primitives, if not already\n");
printf("Key 'w' toggles between rendering filled polygon, rendering a wireframe representation, and rendering both\n");
printf("Key 'l' toggles lighting between using per vertex lighting, per face lighting, and having lighting off\n");
printf("Key 'u' toggles between rendering a new terrain using the circles algorithm and using the fault algorithmn\n");
printf("Key 'o' toggles which object the arrow keys control. Initially the camera rotation is controlled using the arrow keys, hitting 'o' once switches to controlling the position of the first light source, and hitting it a second time switches to controlling the second light source\n");

printf("The left and right arrow keys either rotate the camera around the y axis, or change the x position of the first or second light source\n");
printf("The up and down arrow keys either rotate the camera around the x axis, or change the z position of the first or second light source\n");
printf("The page down and page up keys move the camera, or either of the light sources down");
printf("\n");
printf("\n");

printf("~~~~~~~~~~~~~~~~~~~~Controls for secondary window~~~~~~~~~~~~~~~~~~~~~~\n");
printf("The secondary window is a black and white heightmap of the terrain in 2D, the terrain can be displaced upwards by clicking inside the window using the left mouse button, and displaced downwards as if erasing, using the right mouse button\n");
printf("Note: the secondary window may not be orientated the same as the main window, though the starting orientation should be the same\n");
printf("\n");
printf("Key 'q' quits the program\n");
printf("Key 'v' switches between using the circle algorithm to displace points or displacing single vertices\n");
printf("Key 'p' increases the height a circle displaces the terrain, or a single point is displaced\n");
printf("Key ';' decreases the height a circle displaces the terrain, or a single point is displaced\n");
printf("Key 'o' increases the radius of a circle that acts to displace the terrain, up to a maximum of the entire size of the window\n");
printf("Key 'l' decreases the radius of a circle that acts to displace the terrain, down to a minimum of one pixel\n");

}

//GLUT_KEYBOARD_FUNC functions for the editor window
void kbdEditor(unsigned char key, int x, int y) {
	switch(key) {
		//Quit the program
		case 'q':
		case 'Q':
			exit(0);
			break;

		//Toggle vertex mode between single vertex and circle algorithm displacement
		case 'v':
		case 'V':
			if (singleVertex) {
				singleVertex = false;
			} else {
				singleVertex = true;
			}
		//Increase circle height for drawing or erasing
		case 'p':
		case 'P':
			editCircleHeight += 1;
			break;

		//Decrease circle height for drawing or erasing
		case ';':
			if (editCircleHeight >= 1)
				editCircleHeight -= 1;
			break;

		//Increase circle radius up to the editorHeight / 2 for drawing or erasing
		case 'o':
		case 'O':
			if (!(editCircleRadius > editorHeight / 2)) {
				editCircleRadius += 1;
			}
			break;

		//Decrease circle radius down to 0 for drawing or erasing
		case 'l':
		case 'L':
			if (!(editCircleRadius <= 1)) {
				editCircleRadius -= 1;
			}

	}

	glutPostRedisplay();
}

//GLUT_SPECIAL_FUNC functions for main window
void special(int key, int x, int y)
{
	switch(key)
	{
		//Move the camera left (- x)
		case GLUT_KEY_LEFT: 
			if (cam) {
				yRotation += 1;
			} else if (light0) {
				light0_pos[0] -= 5;
			} else {
				light1_pos[0] -= 5;
			}
			break; 
		//Move the camera right (+ x)
		case GLUT_KEY_RIGHT: 
			if (cam) {
				yRotation -= 1;
			} else if (light0) {
				light0_pos[0] += 5;
			} else {
				light1_pos[0] += 5;
			}
			break; 
		//Move the camera back (- z)
		case GLUT_KEY_DOWN:
			if (cam) {
				xRotation -= 1;
			} else if (light0) {
				light0_pos[2] -= 5;
			} else {
				light1_pos[2] -= 5;
			}
			break;
		//Move the camera forward (+ z)
		case GLUT_KEY_UP:
			if (cam) {
				xRotation += 1;
			} else if (light0) {
				light0_pos[2] += 5;
			} else {
				light1_pos[2] += 5;
			}
			break;
		//Move the camera up (- y)
		case GLUT_KEY_PAGE_UP:
			if (cam) {
				eye[1]+=5;
			} else if (light0) {
				light0_pos[1] += 5;
			} else {
				light1_pos[1] += 5;
			}
			break;
		//Move the camera down (+ y)
		case GLUT_KEY_PAGE_DOWN:
			if (cam) {
				eye[1] -=5;
			} else if (light0) {
				light0_pos[1] -= 5;
			} else {
				light1_pos[1] -= 5;
			}
			break;
    }
	glutPostRedisplay();
}

//GLUT_MOUSE_FUNC for the editor window
void mouse(int btn, int state, int x, int y) {
	//Displace terrain upwards
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (x < editorWidth && y < editorWidth) {
			if (singleVertex) {
				//Displace a single point in the terrain upwards
				heightMap[x][y] += editCircleHeight;

			} 
			else {
				//Displace terrain using the circle algorithm
				addCircleToHeightMap(editCircleRadius, editCircleHeight, x, y);
			}
		}
	}
	
	//Displace terrain downwards, as if erasing
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (x < editorWidth && y < editorWidth) {
			if (singleVertex) {
				//Displace a single point in the terrain downwards
				heightMap[x][y] -= editCircleHeight;
			}
			else {
				//Displace terrain downwards using the circle algorithm
				addCircleToHeightMap(editCircleRadius, -editCircleHeight, x, y);
			}

		}
	}
	//Recalculate normal vectors for the terrain
	normVectors();
	//Refresh the main window
	glutPostRedisplay();

	//Refresh the editor window
	glutSetWindow(main_id);
	glutPostRedisplay();
	
}

//Sets the global cursor position so it can be accessed by the display function for the editor window
void passiveMouseMove(int x, int y) {
	if (x < editorWidth && y < editorWidth) {
		cursorY = y;
		cursorX = x;
	}
	glutPostRedisplay();
}

//GLUT_DISPLAY_FUNC for editor window
void display2D() {

	//Height taken from the heightMap 
	float h;
	//Colour value of a vertex in the window
	float a;
	//Scale for determining the colour of points in the window
	float scale = fabs(1 / (top - bot));
	//Set the number of points in the circle based on its' size, so it looks smooth
	int circlePoints = 10 * editCircleRadius;
	//Used for the circle 
	float angle;

	//Draw each point on-screen at the corresponding x,y location for each x,z value in the heightmap
	//The height of each point being used to determine its' grayscale colour (black for lowest, white for highest)
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int x = 0; x < xSize; x++) {
		for (int y = 0; y < zSize; y++) {
			h = heightMap[x][y];
			a = h * scale - (bot * scale);
			glColor3f(a, a, a);
			glVertex2f(x, (-y + (zSize - 1)));
		}
	}
	//Draw a circle around the cursor,
	//Illustrates how large the editing circle for terrain modfication using the circle algorithmis
	glBegin(GL_LINE_LOOP);
	glColor4f(1,0,0,0.1);
	for (int i = 0; i < circlePoints; i++ ) {
		angle = 2 * M_PI * i / circlePoints;
		glVertex2f(cos(angle) * editCircleRadius + cursorX, sin(angle) * editCircleRadius - cursorY + editorHeight);
	}
	glEnd();

	glutSwapBuffers();
}


void display() {


	//Clearn the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Set the camera position in the scene
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], 0, 0, 0, 0, 1, 0); 


	//Set the posiiton of the lights based on global variables
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);

	//making front and back to have the same material-- if you want front only GL_FRONT
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  m_amb); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  m_diff); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_spec); 
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  shiny);

	if (wired) {
		//Draw wireframe representation of the terrain
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawTerrain();
	}
	if (solid) {
		//Draw the terrain using either triangles or quads
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawTerrain();
	}
	//Double buffer swap
	glutSwapBuffers();
}



int main(int argc, char** argv)
{
	
	//Set the window size, display mode, and create the main window
	glutInit(&argc, argv);
	glutInitWindowSize(mainWidth, mainHeight);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	main_id = glutCreateWindow("Terrain Test");

	//enable Z buffer test, otherwise things appear in the order they're drawn
	glEnable(GL_DEPTH_TEST);
	//Cull the back face of objects
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);


	//Setup the initial view
	//Change to projection matrix mode, set the extents of our viewing volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
       
	//Set clear colour to white
	glClearColor(0.5, 0.5, 0.5, 0);
	//Look down from a 90 deg. angle
	gluPerspective(90,1,1,10000);

	//Print user commands
	printCommands();
	//Initalize main window
	init();

	//Register glut callbacks for keyboard, special keys and display function for main window
	glutKeyboardFunc(kbd);
	glutDisplayFunc(display);
	glutSpecialFunc(special);


	//Create the second editor window
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(editorWidth, editorHeight);
	editor_id = glutCreateWindow("Editor");


	//Register glut callbacks for second window
	glutDisplayFunc(display2D);
	glutMouseFunc(mouse);
	glutKeyboardFunc(kbdEditor);
	glutPassiveMotionFunc(passiveMouseMove);
	initEditor();

	//Enter the main loop
	glutMainLoop();

	return 0;
}
