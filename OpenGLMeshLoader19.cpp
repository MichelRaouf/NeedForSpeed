#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>
#include <stdio.h>
#include <ctime>
#include<time.h>
#include <string.h>
#include <string>
#include <glut.h>
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <MMSystem.h>
#include <mciapi.h>
#include <iostream>
#include<string.h>
#include<string>
#include <math.h>
#include <random>
#include <glut.h>
#include <algorithm>
#include <Mmsystem.h>
#include <mciapi.h>
#include <playsoundapi.h>
#include <queue>
using namespace std;

int WIDTH = 1080;
int HEIGHT = 520;

GLuint tex;
char title[] = "Need For Speed: Lite Edition";

// 3D Projection Options
GLdouble fovy = 100.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 3800;
GLfloat CarAngle = 0;
GLdouble CollectibleRotation = 0;
GLdouble CollectibleHover = 0; // Moving up and down animation
GLboolean isCollectibleHoverUp = true;
GLfloat CarTranslateX = 0;
GLfloat CarTranslateZ = 0;

float accelerate = 0;
float reverseAccelerate = 0;
float accelerateFactor = 0.005;
float speed = 1.0;

int level = 1;
bool isDirt = false;
float dirtSpeed = 0;
float slowingFactor = accelerateFactor;
bool isFPS = false;
bool collideFlag = false;
char* collideDirection = "";

int lab = 0;
bool booster1 = false;
bool booster2 = false;
bool booster3 = false;
bool coin1 = false;
bool coin2 = false;
bool coin3 = false;
bool coin4 = false;
bool lab1 = false;
bool lab2 = false;
int coinsCollected = 0;
bool checkPoint1 = false;
bool checkPoint2 = false;
bool finishLevel1 = false;
bool finishGame = false;

GLdouble offsetX = 0;
GLdouble offsetZ = 0;

bool CarControls[4] = { false, false, false, false };

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

GLfloat AtX = CarTranslateX - offsetX;
GLfloat AtZ = CarTranslateZ;
GLfloat EyeX = AtX + (1.5 * cosf(CarAngle * (M_PI / 180))) - (50 * sinf(CarAngle * (M_PI / 180)));
GLfloat EyeZ = AtZ + (1.5 * sinf(CarAngle * (M_PI / 180))) - (50 * cosf(CarAngle * (M_PI / 180)));

Vector Eye(EyeX, 25.0, EyeZ);
Vector At(AtX, 25.0, AtZ);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_GTR;
Model_3DS model_BMWV1;
Model_3DS model_Nascar;
Model_3DS model_BMWV2;
Model_3DS model_JOF1;
Model_3DS model_HondaTakata;
Model_3DS model_FordFocus;
Model_3DS model_Mercedes;

Model_3DS model_Test;
Model_3DS model_floodLight1;

Model_3DS model_StraightLine;
Model_3DS model_90Turn;
Model_3DS model_60Turn;
Model_3DS model_30Turn;

Model_3DS model_DarkOak;
Model_3DS model_Sprus;

Model_3DS model_GasStation;

Model_3DS model_Bolt;
Model_3DS model_Coin;

Model_3DS model_Raceway;
Model_3DS model_DirtRoad;
Model_3DS model_Stadium;
Model_3DS model_StartLine;

Model_3DS model_TireStack;
Model_3DS model_HayStack;

Model_3DS cars[] = { model_BMWV2, model_FordFocus, model_GTR, model_HondaTakata, model_JOF1, model_Mercedes, model_Nascar };
GLboolean isCarLoaded[] = { false, false, false, false, false, false, false };
GLboolean isCarSelected = false;
int currCar = 10;
int prevCar = 10;

GLdouble dayOffset = 0;
GLdouble dayAngle = 0;

// Textures
GLTexture tex_ground;
GLTexture tex_asphalt;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	glPushMatrix();
	glRotatef(dayAngle, 0, 0, 1);
	GLfloat lightPosition[] = { 250.0f, 0.0f, -75.0f, 1.0f };
	GLfloat diffuse[] = { 1.0f - dayOffset / 1.18, 1.0f - dayOffset, 1.0f - dayOffset, 1.0f };
	GLfloat ambient[] = { 1.0f - dayOffset / 1.18, 1.0f - dayOffset, 1.0f - dayOffset, 1.0f };
	GLfloat specular[] = { 1.0f - dayOffset / 1.18, 1.0f - dayOffset, 1.0f - dayOffset, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	// Define Light source 0 diffuse light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glPopMatrix();

}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glScalef(4.0f, 4.0f, 4.0f);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void drawSky() {
	glPushMatrix();
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glRotated(180, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 2000, 2100, 2000);
	gluDeleteQuadric(qobj);
	glPopMatrix();
}


void translateModelsInnerObjects(Model_3DS model, int index, float x, float y, float z) {
	model.Objects[index].pos.x = x;
	model.Objects[index].pos.y = y;
	model.Objects[index].pos.z = z;
}

//=======================================================================
// Cars' Functions
//=======================================================================
void drawBMW() {
	offsetZ = -7.0f;
	offsetX = 0.0f;
	glPushMatrix();
	glTranslatef(CarTranslateX, 0.0f, CarTranslateZ);
	glScalef(0.00325f, 0.00325f, 0.00325f);
	glRotatef(90 + CarAngle, 0, 1, 0);
	cars[0].Draw();
	glPopMatrix();
	//glPushMatrix();
	//glTranslatef(CarTranslateX, 0.0f, CarTranslateZ);
	//glScalef(0.005f, 0.005f, 0.005f);
	//glRotatef(CarAngle - 90, 0, 1, 0);
	//cars[0].Draw();
	//glPopMatrix();
}
void drawFordFocus() {
	offsetZ = -10.0f;
	offsetX = 0.0f;
	glPushMatrix();
	glTranslatef(CarTranslateX, 0.0f, CarTranslateZ);
	glScalef(4.25f, 4.25f, 4.25f);
	glRotatef(180 + CarAngle, 0, 1, 0);
	cars[1].Draw();
	glPopMatrix();
}
void drawGTR() {
	offsetZ = -20.0f;
	offsetX = 0.0f;
	glPushMatrix();
	glTranslatef(CarTranslateX, 12.5f, CarTranslateZ);
	glScalef(0.6f, 0.6f, 0.6f);
	glRotatef(CarAngle, 0, 1, 0);
	cars[2].Draw();
	glPopMatrix();
}
void drawHondaTakata() {
	offsetZ = -5.0f;
	offsetX = 0.0f;
	glPushMatrix();
	glTranslatef(CarTranslateX, 5.0f, CarTranslateZ);
	glScalef(3.0f, 3.0f, 3.0f);
	glRotatef(180 + CarAngle, 0, 1, 0);
	cars[3].Draw();
	glPopMatrix();
}
void drawJOF1() {
	offsetZ = -10.0f;
	offsetX = 0.0f;
	glPushMatrix();
	glTranslatef(CarTranslateX, 7.0f, CarTranslateZ);
	glScalef(0.006f, 0.006f, 0.006f);
	glRotatef(CarAngle, 0, 1, 0);
	cars[4].Draw();
	glPopMatrix();
}
void drawMercedes() {
	offsetZ = -16.0f;
	offsetX = 0.0f;
	glPushMatrix();
	glTranslatef(CarTranslateX, 10.0f, CarTranslateZ);
	glScalef(8.0f, 8.0f, 8.0f);
	glRotatef(180 + CarAngle, 0, 1, 0);
	cars[5].Objects[25].pos.y = 1000;
	cars[5].Draw();
	glPopMatrix();
}
void drawNascar() {
	offsetZ = -11.0f;
	offsetX = 3.5f;
	glPushMatrix();
	glTranslatef(CarTranslateX, 8.0f, CarTranslateZ);
	glScalef(0.00225f, 0.00225f, 0.00225f);
	glRotatef(180 + CarAngle, 0, 1, 0);
	cars[6].Draw();
	glPopMatrix();
}

//=======================================================================
// Roads' Functions
//======================================================================= 
void drawRaceway() {
	glPushMatrix();
	glTranslatef(0.0f, 50.25f, -47.5f);
	glScalef(2.0f, 3.0f, 3.15f);
	glRotatef(-90, 1, 0, 0);
	model_Raceway.Draw();
	glPopMatrix();
}
void drawDirtRoad() {
	glPushMatrix();
	glTranslatef(410.0f, 0.5f, 225.0f);
	glScalef(-0.25f, 0.05f, 0.375f);
	glRotatef(180, 0, 0, 1);
	glRotatef(-92, 0, 1, 0);
	model_DirtRoad.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(110.0f, 0.5f, 225.0f);
	glScalef(-0.25f, 0.05f, 0.375f);
	glRotatef(180, 0, 0, 1);
	glRotatef(-92, 0, 1, 0);
	model_DirtRoad.Draw();
	glPopMatrix();
}
void drawDirtRaceway() {
	glPushMatrix();
	glTranslatef(-229.0f, 0.0f, -400.0f);
	glScalef(7.5f, 10.0f, 10.0f);

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 50.0f);
	glScalef(20.0f, 10.0f, 30.1f);
	model_StraightLine.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 175.0f);
	glScalef(20.0f, 10.0f, -20.0f);
	model_90Turn.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(112.5f, 0.0f, 188.5f);
	glScalef(20.0f, 10.0f, -20.0f);
	glRotated(-90, 0, 1, 0);
	model_60Turn.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(163.0f, 0.0f, 106.0f);
	glScalef(20.0f, 10.0f, 16.0f);
	glRotatef(-22.5f, 0, 1, 0);
	model_StraightLine.Draw();
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.25f, 0.0f, -75.0f);
	glScalef(20.0f, 10.0f, 20.0f);
	model_90Turn.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(107.5f, 0.0f, -88.0f);
	glScalef(17.5f, 10.0f, 20.0f);
	glRotated(90, 0, 1, 0);
	model_StraightLine.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(195.0f, 0.0f, -88.0f);
	glScalef(20.0f, 10.0f, 20.0f);
	glRotated(-90, 0, 1, 0);
	model_90Turn.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(207.5f, 0.0f, 25.0f);
	glScalef(20.0f, 10.0f, 20.0f);
	glRotated(-180, 0, 1, 0);
	model_30Turn.Draw();
	glPopMatrix();

	glPopMatrix();
}

//=======================================================================
// Collectibles' Functions
//======================================================================= 
void drawBooster(float x, float z) {
	glPushMatrix();
	glTranslatef(x, 14.0f + CollectibleHover, z);
	glScalef(0.025f, 0.025f, 0.025f);
	glRotatef(CollectibleRotation, 0, 1, 0);
	model_Bolt.Draw();
	glPopMatrix();
}
void drawCoin(float x, float z) {
	glPushMatrix();
	glTranslatef(x, 10.0f + CollectibleHover, z);
	glScalef(10.0f, 10.0f, 10.0f);
	glRotatef(CollectibleRotation, 0, 1, 0);
	glRotatef(-25, 0, 0, 1);
	model_Coin.Draw();
	glPopMatrix();
}

//=======================================================================
// Accessories' Functions
//======================================================================= 
void drawGasStation() {
	glPushMatrix();
	glTranslatef(-41.0f, 0.0f, 0.0f);
	glScalef(0.05f, 0.03f, 0.05f);
	model_GasStation.Draw();
	glPopMatrix();
}
void drawStadium() {
	glPushMatrix();
	glTranslatef(0.0f, 80.0f, 0.0f);
	glScalef(0.02f, 0.02f, 0.0205f);
	glRotatef(-90, 0, 1, 0);
	model_Stadium.Draw();
	glPopMatrix();
}
void drawStartLine() {
	glPushMatrix();
	glTranslatef(-203.5f, 35.0f, 40.0f);
	glScalef(0.55f, 0.5f, 0.5f);
	glRotatef(180, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	model_StartLine.Draw();
	glPopMatrix();
}
void drawTireStack(float x, float z, int rotate) {
	glPushMatrix();
	glTranslatef(x, 0.0f, z);
	glScalef(0.125f, 0.125f, 0.125f);
	glRotatef(rotate, 0, 1, 0);
	model_TireStack.Draw();
	glPopMatrix();
}
void drawOuterBorder() {
	for (int i = -24; i < 76; i++) {
		drawTireStack(-65.0f, i * 10.0f - 300.0f, -90);
		drawTireStack(520.0f, i * 10.0f - 300.0f, 90);
		if (i > 1 && i < 48) {
			drawTireStack(119.0f, i * 10.0f - 300.0f, 90);
			drawTireStack(388.0f, i * 10.0f - 300.0f, -90);
		}
	}

	for (int i = -18; i < 41; i++) {
		drawTireStack(i * 10.0f + 117.5f, 445.0f, 0);
		drawTireStack(i * 10.0f + 117.5f, -540.0f, -180);
		if (i > 2 && i <= 23) {
			drawTireStack(i * 10.0f + 117.5f, 245.0f, -180);
			drawTireStack(i * 10.0f + 117.5f, -340.0f, 0);
		}
	}

	/*for (int i = 0; i < 90; i += 3) {
	//	// Top Right Corner
	//	glPushMatrix();
	//	glScalef(0.95f, 1.0f, 1.47f);
	//	glTranslatef(115.0f, 0.0f, 120.5f);
	//	glRotatef(-90, 0, 1, 0);
	//	drawTireStack(182.5f * cosf(i * (M_PI / 180)), 182.5f * sinf(i * (M_PI / 180)), 0);
	//	glPopMatrix();
	//	// Bottom Right Corner
	//	glPushMatrix();
	//	glScalef(0.95f, 1.0f, -1.47f);
	//	glTranslatef(115.0f, 0.0f, 185.0f);
	//	glRotatef(-90, 0, 1, 0);
	//	drawTireStack(182.5f * cosf(i * (M_PI / 180)), 182.5f * sinf(i * (M_PI / 180)), 0);
	//	glPopMatrix();
	//	// Bottom Left Corner
	//	glPushMatrix();
	//	glScalef(-0.95f, 1.0f, -1.47f);
	//	glTranslatef(-364.0f, 0.0f, 185.0f);
	//	glRotatef(-90, 0, 1, 0);
	//	drawTireStack(182.5f * cosf(i * (M_PI / 180)), 182.5f * sinf(i * (M_PI / 180)), 0);
	//	glPopMatrix();
	//	// Top Left Corner
	//	glPushMatrix();
	//	glScalef(-0.95f, 1.0f, 1.47f);
	//	glTranslatef(-364.0f, 0.0f, 120.5f);
	//	glRotatef(-90, 0, 1, 0);
	//	drawTireStack(182.5f * cosf(i * (M_PI / 180)), 182.5f * sinf(i * (M_PI / 180)), 0);
	//	glPopMatrix();
	}*/
}
void drawInnerBorder() {
	// Top Right Inner Border
	glPushMatrix();
	drawTireStack(119.0f, 180.0f, 90);
	drawTireStack(119.0f, 190.0f, 90);
	drawTireStack(120.5f, 200.0f, 90);
	drawTireStack(123.5f, 210.0f, 90);
	drawTireStack(127.0f, 220.0f, 90);
	drawTireStack(130.0f, 230.0f, 90);
	drawTireStack(136.0f, 237.0f, 70);
	drawTireStack(139.0f, 241.0f, 70);
	glPopMatrix();
	// Bottom Right Inner Border
	glPushMatrix();
	glScalef(1.0f, 1.0f, -1.0f);
	glTranslatef(0.0f, 0.0f, 1.0f);
	drawTireStack(119.0f, 275.0f, 90);
	drawTireStack(119.0f, 285.0f, 90);
	drawTireStack(120.5f, 295.0f, 90);
	drawTireStack(123.5f, 305.0f, 90);
	drawTireStack(127.0f, 315.0f, 90);
	drawTireStack(130.0f, 325.0f, 90);
	drawTireStack(136.0f, 332.0f, 70);
	drawTireStack(139.0f, 336.0f, 70);
	glPopMatrix();
	// Top Left Inner Border
	glPushMatrix();
	glScalef(1.0f, 1.0f, 1.25f);
	glTranslatef(337.5f, 0.0f, 145.0f);
	for (int i = 0; i < 90; i += 10) {
		drawTireStack(50.0f * cosf(i * (M_PI / 180)), 50.0f * sinf(i * (M_PI / 180)), 180);
	}
	glPopMatrix();
	// Bottom Left Inner Border
	glPushMatrix();
	glScalef(1.0f, 1.0f, -1.25f);
	glTranslatef(337.5f, 0.0f, 222.0f);
	for (int i = 0; i < 90; i += 10) {
		drawTireStack(50.0f * cosf(i * (M_PI / 180)), 50.0f * sinf(i * (M_PI / 180)), 180);
	}
	glPopMatrix();
}
void drawHaystack(float x, float z, float rotateY) {
	glPushMatrix();
	glScalef(0.25f, 0.25f, 0.25f);
	glTranslatef(x, 0.0f, z); // -175
	glRotatef(-90, 0, 0, 1);
	glRotatef(rotateY, 0, 1, 0);
	model_HayStack.Draw();
	glPopMatrix();
}
void drawSecondBorder() {
	glPushMatrix();
	drawHaystack(1550.0f, 700.0f, 90);
	drawHaystack(1550.0f, 520.0f, 90);
	drawHaystack(1550.0f, 340.0f, 90);
	drawHaystack(1550.0f, 160.0f, 90);
	drawHaystack(1550.0f, -20.0f, 90);
	drawHaystack(1550.0f, -200.0f, 90);
	drawHaystack(1550.0f, -380.0f, 90);

	drawHaystack(2082.5f, 700.0f, 90);
	drawHaystack(2082.5f, 520.0f, 90);
	drawHaystack(2082.5f, 340.0f, 90);
	drawHaystack(2082.5f, 160.0f, 90);
	drawHaystack(2082.5f, -20.0f, 90);
	drawHaystack(2082.5f, -200.0f, 90);
	drawHaystack(2082.5f, -380.0f, 90);
	glPopMatrix();
}

void drawSprusTree(float x, float z) {
	glPushMatrix();
	glTranslatef(x, 0.0f, z);
	glScalef(20.0f, 20.0f, 20.0f);
	model_Sprus.Draw();
	glPopMatrix();
}

void drawSelectedCar(int currCar) {
	switch (currCar)
	{
	case 0: drawBMW(); break;
	case 1: drawFordFocus(); break;
	case 2: drawGTR(); break;
	case 3: drawHondaTakata(); break;
	case 4: drawJOF1(); break;
	case 5: drawMercedes(); break;
	case 6: drawNascar(); break;
	default:
		break;
	}
}
//=======================================================================
// Display Function
//=======================================================================

void print(int x, int y, char* string)
{
	glColor3f(0, 0, 0);
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}
void displayText(double x, double y, double z, std::string text)
{

	glRasterPos3f(x, y, z);
	for (int i = 0; i < text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}

}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Draw Car */
	if (isCarLoaded[currCar]) {
		drawSelectedCar(currCar);
	}

	/* Collectibles */
	//int coinZ = -100;
	//for (int i = 0; i < 10; i++)
	//{
	//	drawBooster(450.0f, coinZ);
	//	drawCoin(0.0f, coinZ);
	//	coinZ += 30.0f;
	//}

	/* Accessories */
	//drawGasStation();
	//drawSecondBorder();


	//glPushMatrix();
	//glTranslatef(320.0f, 0.0f, -100.0f);
	//glRotatef(-90, 0, 1, 0);
	//drawHaystack(1380.0f, 540.0f, 90);
	//drawHaystack(1380.0f, 360.0f, 90);
	//drawHaystack(1380.0f, 180.0f, 90);
	//drawHaystack(1380.0f, 0.0f, 90);

	//drawHaystack(2180.0f, 540.0f, 90);
	//drawHaystack(2180.0f, 360.0f, 90);
	//drawHaystack(2180.0f, 180.0f, 90);
	//drawHaystack(2180.0f, 0.0f, 90);
	//glPopMatrix();

	glDisable(GL_DEPTH_TEST);

	// Display Current Lap and Level with Coins Collected
	//glColor3f(0, 0, 0);
	displayText(50, 10, 10, "level: " + to_string(level));
	displayText(0, 10, 10, "Lap: " + to_string(lab));
	displayText(-50, 10, 10, to_string(coinsCollected) + " out of 10 Coins Collected.");
	glFlush();

	// Display Race Result
	if (finishGame) {
		if (coinsCollected > 10) {
			//glColor3f(0, 0, 0);
			displayText(At.x, 0, At.z, "You Win.");
			glFlush();
		}
		else {
			//glColor3f(0, 0, 0);
			displayText(At.x, 0, At.z, "You Lost.");
			glFlush();
		}
	}
	glEnable(GL_DEPTH_TEST);

	if (level == 1) {

		if (lab == 1) {
			if (booster1)
				drawBooster(-7.056591, 145.551453);
			if (booster2)
				drawBooster(337.503204, -453.960266);
			if (coin1)
				drawCoin(120.382889, 328.962158);
			if (coin2)
				drawCoin(430.686890, 224.100311);
			if (coin3)
				drawCoin(451.506470, -272.716431);
			if (coin4)
				drawCoin(-8.580368, -214.319901);


		}
		else {
			if (lab == 2) {
				if (booster1)
					drawBooster(414.807281, 209.060715);
				if (booster2)
					drawBooster(211.647736, -469.350891);
				if (coin1)
					drawCoin(70.950050, 410.340576);
				if (coin2)
					drawCoin(491.379150, -131.777695);
				if (coin3)
					drawCoin(349.642853, -378.586975);
				if (coin4)
					drawCoin(-8.580368, -214.319901);
			}
		}


	}
	else {
		if (lab == 1) {
			if (booster1)
				drawBooster(27.743643, 308.688599);
			if (booster2)
				drawBooster(1010.109009, 1128.980957);
			if (booster3)
				drawBooster(995.942200, -1284.786011);
			if (coin1)
				drawCoin(12.992612, 700.309082);
			if (coin2)
				drawCoin(382.885620, 1432.870850);
			if (coin3)
				drawCoin(1489.242188, -958.257385);
			if (coin4)
				drawCoin(121.138977, -1071.335449);

		}
		else {
			if (lab == 2) {
				if (booster1)
					drawBooster(-0.614531, 933.230225);
				if (booster2)
					drawBooster(1352.001953, 348.702209);
				if (booster3)
					drawBooster(385.628235, -1266.510010);
				if (coin1)
					drawCoin(-27.369238, 142.551834);
				if (coin2)
					drawCoin(621.480164, 1477.840698);
				if (coin3)
					drawCoin(1262.777954, -1222.242065);
				if (coin4)
					drawCoin(683.224182, -1321.621948);
			}
		}

	}

	/* Environment */
	glPushMatrix();
	glTranslatef(227.5f, 0.0f, 0.0f);
	if (level == 1) {
		glPushMatrix();
		glTranslatef(-227.5f, 0.0f, 0.0f);
		drawOuterBorder();
		drawInnerBorder();
		glPopMatrix();

		drawStartLine();
		drawStadium();
		drawRaceway();
	}
	else {
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f);
		glScalef(25.0f, 1.0f, 25.0f);
		RenderGround();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-25.0f, 0.0f, 0.0f);
		drawStartLine();
		glPopMatrix();

		for (int i = -10; i < 18; i++)
		{
			drawSprusTree(-125.0f, i * 60);
			drawSprusTree(-325.0f, i * 60);
		}

		drawDirtRaceway();
	}
	drawSky();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(5, 0, 505);
	glRotated(215, 0, 1, 0);
	model_floodLight1.Draw();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(580, 0, 360);
	glRotated(-45, 0, 1, 0);
	model_floodLight1.Draw();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(492.5, 0, -132.5);
	model_floodLight1.Draw();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, 30.0f);
	glRotated(180, 0, 1, 0);
	model_floodLight1.Draw();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(400.0f, 0.0f, -572.0f);
	glRotated(45, 0, 1, 0);
	model_floodLight1.Draw();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(-138.0f, 0, -472.0f);
	glRotated(135, 0, 1, 0);
	model_floodLight1.Draw();
	glPopMatrix();


	glutSwapBuffers();
}

void moveAroundScene() {
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	GLfloat CarOffsetX = offsetX * cosf(CarAngle * (M_PI / 180)) + offsetZ * sinf(CarAngle * (M_PI / 180));
	GLfloat CarOffsetZ = offsetX * sinf(CarAngle * (M_PI / 180)) - offsetZ * cosf(CarAngle * (M_PI / 180));

	GLfloat CameraOffsetX = 0.0;
	GLfloat CameraOffsetZ = 0.0;

	if (!isFPS) {
		CameraOffsetX = -50 * sinf(CarAngle * (M_PI / 180)) + 3 * sinf(CarAngle * (M_PI / 180)) * cosf(CarAngle * (M_PI / 180));
		CameraOffsetZ = -50 * cosf(CarAngle * (M_PI / 180)) + 3 * sinf(CarAngle * (M_PI / 180)) * cosf(CarAngle * (M_PI / 180));
	}
	else {
		CameraOffsetX = (offsetZ + 4) * sinf(CarAngle * (M_PI / 180));
		CameraOffsetZ = (offsetZ + 4) * cosf(CarAngle * (M_PI / 180));
	}

	At.x = CarTranslateX - CarOffsetX;
	At.z = CarTranslateZ + CarOffsetZ;

	Eye.x = CameraOffsetX + At.x;
	Eye.z = CameraOffsetZ + At.z;

	if (!isFPS) {
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	}
	else {
		gluLookAt(Eye.x, Eye.y - 5, Eye.z, At.x, At.y - 5, At.z, Up.x, Up.y, Up.z);
	}
}

void loadCar(int index, char* path) {
	if (!isCarLoaded[index]) {
		cars[index].Load(path);
		isCarLoaded[index] = true;
	}

	prevCar = currCar;
	currCar = index;
}

void selectCar(unsigned char button) {
	if (!isCarSelected) {
		switch (button)
		{
		case 'b':
			loadCar(0, "Models/cars/Working/BMW/V1/BMW_M3_GTR.3ds");
			speed = 1.75;
			accelerateFactor = 0.0075;
			break;
		case 'f':
			loadCar(1, "Models/cars/Working/Ford Focus Dirt/Ford_Focus.3ds");
			speed = 1.25;
			accelerateFactor = 0.0025;
			break;
		case 'g':
			loadCar(2, "Models/cars/Working/GTR/GTR.3ds");
			speed = 1.5;
			accelerateFactor = 0.005;
			break;
		case 'h':
			loadCar(3, "Models/cars/Working/HondaTakata/car.3DS");
			speed = 2.75;
			accelerateFactor = 0.02;
			break;
		case 'j':
			loadCar(4, "Models/cars/Working/JOF1/JOF1_L.3DS");
			speed = 4.0;
			accelerateFactor = 0.03;
			break;
		case 'm':
			loadCar(5, "Models/cars/Working/Mercedes CLK GTR/clkgtr.3ds");
			speed = 2.0;
			accelerateFactor = 0.005;
			break;
		case 'n':
			loadCar(6, "Models/cars/Working/Nascar/nascar#2 xform.3DS");
			speed = 2.5;
			accelerateFactor = 0.0085;
			break;
		default:
			break;
		}
		isCarSelected = true;
	}
}

void resetGame() {
	CarTranslateX = 0;
	CarTranslateZ = 0;
	CarAngle = 0;
	accelerate = 0;
	reverseAccelerate = 0;
	accelerateFactor = 0.005;
	speed = 1.0;
	offsetX = 0;
	offsetZ = 0;
	isCarSelected = false;
	prevCar = 10;
	currCar = 10;
	lab = 0;
	level = 1;
	booster1 = false;
	booster2 = false;
	booster3 = false;
	coin1 = false;
	coin2 = false;
	coin3 = false;
	coin4 = false;
	lab1 = false;
	lab2 = false;
	coinsCollected = 0;
	checkPoint1 = false;
	checkPoint2 = false;
	finishLevel1 = false;
	finishGame = false;
}
//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 'q':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'e':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27:
		exit(0);
		break;
	case 'w':
		Eye.z += 5;
		break;
	case 's':
		Eye.z -= 5;
		break;
	case 'a':
		Eye.z += 5;
		Eye.x += 5;
		break;
	case 'd':
		Eye.z -= 5;
		Eye.x -= 5;
		break;
	case 'r':
		resetGame();
		break;
	case '0': isFPS = !isFPS; break;
	case 'b':
	case 'f':
	case 'g':
	case 'h':
	case 'j':
	case 'm':
	case 'n': selectCar(button); break;
	default:
		break;
	}
}

void myKeyboardUp(unsigned char button, int x, int y) {

}
void carMovement(int key, int x, int y) {
	if (!finishGame) {
		if (key == GLUT_KEY_UP) {
			CarControls[0] = true;
		}
		if (key == GLUT_KEY_DOWN) {
			CarControls[1] = true;
		}
		if (key == GLUT_KEY_LEFT) {
			CarControls[2] = true;
		}
		if (key == GLUT_KEY_RIGHT) {
			CarControls[3] = true;
		}
	}
}

void carMovementUp(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		CarControls[0] = false;
	}
	if (key == GLUT_KEY_DOWN) {
		CarControls[1] = false;
	}
	if (key == GLUT_KEY_LEFT) {
		CarControls[2] = false;
	}
	if (key == GLUT_KEY_RIGHT) {
		CarControls[3] = false;
	}
}
//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		//Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		//Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================

void LoadAssets()
{

	//Loading Shared Collectibles
	model_Bolt.Load("Models/Collectibles/Boost/Lightning bolt export 1.3ds");
	model_Coin.Load("Models/Collectibles/Coin/GoldCoinBlank.3ds");
	model_floodLight1.Load("Models/Accessories/Flood_Light.3DS");

	// Loading Shared Accessories
	model_StartLine.Load("Models/Accessories/Starting Line/starting_line.3ds");

	//Loading Cars
	//model_GTR.Load("Models/cars/Working/GTR/GTR.3ds");
	//model_BMWV1.Load("Models/cars/Working/BMW/V1/BMW_M3_GTR.3ds");
	//model_BMWV2.Load("Models/cars/Working/BMW/V2/BMW_M3_GTR1.3DS");
	//model_Nascar.Load("Models/cars/Working/Nascar/nascar#2 xform.3DS");
	//model_JOF1.Load("Models/cars/Working/JOF1/JOF1_L.3DS");
	//model_HondaTakata.Load("Models/cars/Working/HondaTakata/car.3DS");
	//model_FordFocus.Load("Models/cars/Working/Ford Focus Dirt/Ford_Focus.3ds");
	//cars[5].Load("Models/cars/Working/Mercedes CLK GTR/clkgtr.3ds");

	/* Level 1 */
	// Loading Asphalt Track
	model_Raceway.Load("Models/Race Tracks/Asphalt Raceway/Building N171218.3DS");
	model_DirtRoad.Load("Models/Race Tracks/Dirt/rock10_3ds.3ds");

	//Loading Accessories
	model_GasStation.Load("Models/Accessories/Filling Machine.3DS");
	model_Stadium.Load("Models/Accessories/Stadium/Stadium N210517.3DS");

	// Loading Obstacles
	model_TireStack.Load("Models/Obstacles/Tire Stack/TireStack_1.3DS");


	/* Level 2 */
	// Loading Dirt Track
	model_StraightLine.Load("Models/Race Tracks/Dirt Raceway/Straight Line.3ds");
	model_90Turn.Load("Models/Race Tracks/Dirt Raceway/Quarter Circle.3ds");
	model_30Turn.Load("Models/Race Tracks/Dirt Raceway/path02_r30.3ds");
	model_60Turn.Load("Models/Race Tracks/Dirt Raceway/path02_r60.3ds");

	// Loading Obstacles
	model_HayStack.Load("Models/Obstacles/Haystack/11543_BundleOfStraw_V2_l1 (1).3ds");
	model_DarkOak.Load("Models/Obstacles/Trees/Dark Oak/tree2.3ds");
	model_Sprus.Load("Models/Obstacles/Trees/Sprus/firtree3.3ds");

	// Loading Ground
	tex_ground.Load("Textures/ground.bmp");

	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

void Drive() {
	if (CarControls[0] == true) {
		CarTranslateZ += speed * accelerate * cosf(CarAngle * (M_PI / 180));
		CarTranslateX += speed * accelerate * sinf(CarAngle * (M_PI / 180));
		if (accelerate <= 1) {
			accelerate += accelerateFactor;
		}
	}
	else if (CarControls[0] == false && accelerate >= 0) {
		CarTranslateZ += speed * accelerate * cosf(CarAngle * (M_PI / 180));
		CarTranslateX += speed * accelerate * sinf(CarAngle * (M_PI / 180));
		accelerate -= accelerateFactor / 1.5;
	}

}
void Reverse() {
	if (CarControls[1] == true) {
		CarTranslateZ -= (speed / 1.5) * reverseAccelerate * cosf(CarAngle * (M_PI / 180));
		CarTranslateX -= (speed / 1.5) * reverseAccelerate * sinf(CarAngle * (M_PI / 180));
		if (reverseAccelerate <= 1) {
			reverseAccelerate += accelerateFactor / 2;
		}

	}
	else if (CarControls[1] == false && reverseAccelerate >= 0) {
		CarTranslateZ -= (speed / 1.5) * reverseAccelerate * cosf(CarAngle * (M_PI / 180));
		CarTranslateX -= (speed / 1.5) * reverseAccelerate * sinf(CarAngle * (M_PI / 180));
		reverseAccelerate -= accelerateFactor;
	}
}
void Turn() {
	// Left
	if (CarControls[2] == true) {
		if (accelerate > 0) {
			CarAngle += (speed / 2) * accelerate;
		}

		if (reverseAccelerate > 0) {
			CarAngle -= (speed / 2) * reverseAccelerate;
		}
	}
	// Right
	if (CarControls[3] == true) {
		if (accelerate > 0) {
			CarAngle -= (speed / 2) * accelerate;
		}

		if (reverseAccelerate > 0) {
			CarAngle += (speed / 2) * reverseAccelerate;
		}
	}

	if (CarAngle >= 360) {
		CarAngle = 0;
	}
}

void CollectibleAnimation() {

	CollectibleRotation += 0.5;

	if (CollectibleRotation >= 360) {
		CollectibleRotation = 0;
	}

	if (isCollectibleHoverUp) {
		if (CollectibleHover > 3) {
			isCollectibleHoverUp = false;
		}

		CollectibleHover += 0.05;
	}
	else {
		if (CollectibleHover < 0) {
			isCollectibleHoverUp = true;
		}

		CollectibleHover -= 0.05;
	}
}


float DiagonalInside(float x) {
	return 3643 - 2.5 * x;
}

float DiagonalOutside(float x) {
	return 3801.5 - 2.5 * x;
}

void collideCheck1() {
	if (CarTranslateZ >= -400 && CarTranslateZ <= 350) {
		if (CarTranslateX <= -50) {
			collideFlag = true;
			collideDirection = "Right";
		}
		if (CarTranslateX > 500) {
			collideFlag = true;
			collideDirection = "Left";
		}
	}
	if (CarTranslateX >= 80 && CarTranslateX <= 395) {
		if (CarTranslateZ > 425) {
			collideFlag = true;
			collideDirection = "Top";
		}
		if (CarTranslateZ < -530) {
			collideFlag = true;
			collideDirection = "Bottom";
		}
	}

	if (CarTranslateZ >= -270 && CarTranslateZ <= 170) {
		if (CarTranslateX >= 100 && CarAngle <= 90) {
			collideFlag = true;
			collideDirection = "RightIn";
		}
		if (CarTranslateX <= 405 && CarTranslateX >= 100 && CarAngle >= 90) {
			collideFlag = true;
			collideDirection = "LeftIn";
		}
	}

	if (CarTranslateX >= 160 && CarTranslateX <= 350) {
		if (CarTranslateZ >= -350 && CarAngle >= 270) {
			collideFlag = true;
			collideDirection = "BottomIn";
		}
		if (CarTranslateZ <= 260 && CarAngle >= 90 && CarAngle <= 180) {
			collideFlag = true;
			collideDirection = "TopIn";
		}
	}
}
void collideCheck2() {
	//bool firstStraightTrack = (CarTranslateZ <= 908 && CarTranslateZ >= -750) && (CarTranslateX <= 30 && CarTranslateX >= -30);
	//bool secondStraightTrack = (CarTranslateZ >= -1328 && CarTranslateZ <= -1236) && (CarTranslateX >= 419 && CarTranslateX <= 1116);

	if (CarTranslateZ <= 908 && CarTranslateZ >= -750) {
		if (CarTranslateX >= 30 && CarTranslateX <= 40) {
			collideFlag = true;
			collideDirection = "FirstL";
		}
		if (CarTranslateX <= -30 && CarTranslateX >= -40) {
			collideFlag = true;
			collideDirection = "FirstR";
		}
	}
	if (CarTranslateX >= 419 && CarTranslateX <= 1116) {
		if (CarTranslateZ <= -1328 && CarTranslateZ >= -1338) {
			collideFlag = true;
			collideDirection = "SecondL";
		}
		if (CarTranslateZ >= -1236 && CarTranslateZ <= -1226) {
			collideFlag = true;
			collideDirection = "SecondR";
		}
	}
	if ((CarTranslateX <= 1402 && CarTranslateX >= 1127) && (CarTranslateZ >= 136 && CarTranslateZ <= 1113)) {
		float leftDiagonal = DiagonalOutside(CarTranslateX);
		float rightDiagonal = DiagonalInside(CarTranslateX);

		if (CarTranslateZ > leftDiagonal) {
			collideFlag = true;
			collideDirection = "ThirdL";
		}

		if (CarTranslateZ < rightDiagonal) {
			collideFlag = true;
			collideDirection = "ThirdR";
		}
	}
}

void speedDown(int x) {
	speed = speed / 2;
}

bool isDay = true;
void idle() {
	//printf("current z %f \n", CarTranslateZ);
	//printf("current x %f \n", CarTranslateX);

	// Driver Controls / Obstacles
	if (level == 1) {
		collideCheck1();
		if (!collideFlag) {
			Drive();
			Reverse();
			Turn();
		}
		else {
			accelerate /= 2;
			if (collideDirection == "Right") {
				CarAngle = 0;
				CarTranslateX += 10;
			}
			else {
				if (collideDirection == "Left") {
					CarAngle = 180;
					CarTranslateX -= 10;
				}
				else {
					if (collideDirection == "Top") {
						CarAngle = 90;
						CarTranslateZ -= 10;
					}
					else {
						if (collideDirection == "Bottom") {
							CarAngle = 270;
							CarTranslateZ += 10;
						}
						else {
							if (collideDirection == "RightIn") {
								CarAngle = 0;
								CarTranslateX -= 10;
							}
							else {
								if (collideDirection == "LeftIn") {
									CarAngle = 180;
									CarTranslateX += 10;
									printf("Here");
								}
								else {
									if (collideDirection == "BottomIn") {
										CarTranslateZ -= 10;
										CarAngle = 270;
									}
									else {
										if (collideDirection == "TopIn") {
											CarTranslateZ += 10;
											CarAngle = 90;
										}
									}
								}
							}
						}
					}
				}
			}
			collideFlag = false;
		}
	}

	if (level == 2) {
		collideCheck2();
		if (!collideFlag) {
			Drive();
			Reverse();
			Turn();
		}
		else {
			accelerate /= 1.25;
			if (collideDirection == "FirstL") {
				CarTranslateX -= 5;
				CarAngle = 0;
			}
			else {
				if (collideDirection == "FirstR") {
					CarTranslateX += 5;
					CarAngle = 0;
				}
				else {
					if (collideDirection == "SecondL") {
						CarTranslateZ += 5;
						CarAngle = 270;
					}
					else {
						if (collideDirection == "SecondR") {
							CarTranslateZ -= 5;
							CarAngle = 270;
						}
						else {
							if (collideDirection == "ThirdL") {
								CarAngle = 160;
								CarTranslateX -= 5;
								CarTranslateZ += 1;
							}
							else {
								if (collideDirection == "ThirdR") {
									CarAngle = 160;
									CarTranslateX += 5;
									CarTranslateZ -= 1;
								}
							}
						}
					}
				}
			}
			collideFlag = false;
		}
	}

	// Collectible Conditions
	if (level == 1) {
		if (CarTranslateZ >= 60 && CarTranslateZ <= 70 && CarTranslateX >= -45 && CarTranslateX <= 100 && lab < 1 && !checkPoint1) {

			lab = 1;
			booster1 = true;
			booster2 = true;
			coin1 = true;
			coin2 = true;
			coin3 = true;
			coin4 = true;
		}
		if (CarTranslateZ >= -110 && CarTranslateZ <= -100 && CarTranslateX >= 400 && CarTranslateX <= 500 && lab == 1 && !checkPoint1) {
			checkPoint1 = true;

		}
		if (CarTranslateZ >= 60 && CarTranslateZ <= 70 && CarTranslateX >= -45 && CarTranslateX <= 100 && lab == 1 && checkPoint1) {

			lab = 2;
			booster1 = true;
			booster2 = true;
			coin1 = true;
			coin2 = true;
			coin3 = true;
			coin4 = true;
		}
		if (CarTranslateZ >= -110 && CarTranslateZ <= -100 && CarTranslateX >= 400 && CarTranslateX <= 500 && lab == 2 && checkPoint1 && !checkPoint2) {
			checkPoint2 = true;

		}
		if (CarTranslateZ >= 60 && CarTranslateZ <= 70 && CarTranslateX >= -45 && CarTranslateX <= 100 && lab == 2 && checkPoint1 && checkPoint2) {
			finishLevel1 = true;
			level = 2;
			CarAngle = 0;
			CollectibleRotation = 0;
			CollectibleHover = 0; // Moving up and down animation
			isCollectibleHoverUp = true;
			CarTranslateX = 0;
			CarTranslateZ = 0;

			accelerate = 0;
			reverseAccelerate = 0;
			accelerateFactor = 0.005;
			speed = 20.0;
			lab = 0;
			booster1 = false;
			booster2 = false;
			booster3 = false;
			coin1 = false;
			coin2 = false;
			coin3 = false;
			coin4 = false;
			lab1 = false;
			lab2 = false;

			checkPoint1 = false;
			checkPoint2 = false;


		}


		//Level 1 lab 1
		if (lab == 1 && booster1 && CarTranslateZ >= (145.551453 - 10) && CarTranslateZ <= (145.551453 + 10) && CarTranslateX <= (-7.056591 + 10) && CarTranslateX >= (-7.056591 - 10)) {
			booster1 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 1 && booster2 && CarTranslateZ >= (-453.960266 - 10) && CarTranslateZ <= (-453.960266 + 10) && CarTranslateX <= (337.503204 + 10) && CarTranslateX >= (337.503204 - 10)) {
			booster2 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 1 && coin1 && CarTranslateZ >= (328.962158 - 10) && CarTranslateZ <= (328.962158 + 10) && CarTranslateX <= (120.382889 + 10) && CarTranslateX >= (120.382889 - 10)) {
			coin1 = false;
			coinsCollected++;
			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 1 && coin2 && CarTranslateZ >= (224.100311 - 10) && CarTranslateZ <= (224.100311 + 10) && CarTranslateX <= (430.686890 + 10) && CarTranslateX >= (430.686890 - 10)) {
			coin2 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 1 && coin3 && CarTranslateZ >= (-272.716431 - 10) && CarTranslateZ <= (-272.716431 + 10) && CarTranslateX <= (451.506470 + 10) && CarTranslateX >= (451.506470 - 10)) {
			coin3 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 1 && coin4 && CarTranslateZ >= (-214.319901 - 10) && CarTranslateZ <= (-214.319901 + 10) && CarTranslateX <= (-8.580368 + 10) && CarTranslateX >= (-8.580368 - 10)) {
			coin4 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}

		//Level 1 lab 2


		if (lab == 2 && booster1 && CarTranslateZ >= (209.060715 - 10) && CarTranslateZ <= (209.060715 + 10) && CarTranslateX <= (414.807281 + 10) && CarTranslateX >= (414.807281 - 10)) {
			booster1 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 2 && booster2 && CarTranslateZ >= (-469.350891 - 10) && CarTranslateZ <= (-469.350891 + 10) && CarTranslateX <= (211.647736 + 10) && CarTranslateX >= (211.647736 - 10)) {
			booster2 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 2 && coin1 && CarTranslateZ >= (410.340576 - 10) && CarTranslateZ <= (410.340576 + 10) && CarTranslateX <= (70.950050 + 10) && CarTranslateX >= (70.950050 - 10)) {
			coin1 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 2 && coin2 && CarTranslateZ >= (-131.777695 - 10) && CarTranslateZ <= (-131.777695 + 10) && CarTranslateX <= (491.379150 + 10) && CarTranslateX >= (491.379150 - 10)) {
			coin2 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 2 && coin3 && CarTranslateZ >= (-378.586975 - 10) && CarTranslateZ <= (-378.586975 + 10) && CarTranslateX <= (349.642853 + 10) && CarTranslateX >= (349.642853 - 10)) {
			coin3 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 2 && coin4 && CarTranslateZ >= (-214.319901 - 10) && CarTranslateZ <= (-214.319901 + 10) && CarTranslateX <= (-8.580368 + 10) && CarTranslateX >= (-8.580368 - 10)) {
			coin4 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
	}
	if (level == 2 && !finishGame) {
		if (CarTranslateZ >= 55 && CarTranslateZ <= 65 && CarTranslateX >= -35 && CarTranslateX <= 30 && lab < 1 && !checkPoint1) {

			lab = 1;
			booster1 = true;
			booster2 = true;
			coin1 = true;
			coin2 = true;
			coin3 = true;
			coin4 = true;
		}
		if (CarTranslateZ >= 746.712280 && CarTranslateZ <= 756.712280 && CarTranslateX >= 1097.304199 && CarTranslateX <= 1225.029175 && lab == 1 && !checkPoint1) {
			checkPoint1 = true;

		}
		if (CarTranslateZ >= 55 && CarTranslateZ <= 65 && CarTranslateX >= -35 && CarTranslateX <= 30 && lab == 1 && checkPoint1) {

			lab = 2;
			booster1 = true;
			booster2 = true;
			coin1 = true;
			coin2 = true;
			coin3 = true;
			coin4 = true;
		}
		if (CarTranslateZ >= 746.712280 && CarTranslateZ <= 756.712280 && CarTranslateX >= 1097.304199 && CarTranslateX <= 1225.029175 && lab == 2 && checkPoint1 && !checkPoint2) {
			checkPoint2 = true;

		}
		if (CarTranslateZ >= 55 && CarTranslateZ <= 65 && CarTranslateX >= -35 && CarTranslateX <= 30 && lab == 2 && checkPoint1 && checkPoint2) {
			finishGame = true;
		}
		//level 2 lab 1

		if (lab == 1 && booster1 && CarTranslateZ >= (308.688599 - 10) && CarTranslateZ <= (308.688599 + 10) && CarTranslateX <= (27.743643 + 10) && CarTranslateX >= (27.743643 - 10)) {
			booster1 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 1 && booster2 && CarTranslateZ >= (1128.980957 - 10) && CarTranslateZ <= (1128.980957 + 10) && CarTranslateX <= (1010.109009 + 10) && CarTranslateX >= (1010.109009 - 10)) {
			booster2 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 1 && booster3 && CarTranslateZ >= (-1284.786011 - 10) && CarTranslateZ <= (-1284.786011 + 10) && CarTranslateX <= (995.942200 + 10) && CarTranslateX >= (995.942200 - 10)) {
			booster3 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 1 && coin1 && CarTranslateZ >= (700.309082 - 10) && CarTranslateZ <= (700.309082 + 10) && CarTranslateX <= (12.992612 + 10) && CarTranslateX >= (12.992612 - 10)) {
			coin1 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 1 && coin2 && CarTranslateZ >= (1432.870850 - 10) && CarTranslateZ <= (1432.870850 + 10) && CarTranslateX <= (382.885620 + 10) && CarTranslateX >= (382.885620 - 10)) {
			coin2 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 1 && coin3 && CarTranslateZ >= (-958.257385 - 10) && CarTranslateZ <= (-958.257385 + 10) && CarTranslateX <= (1489.242188 + 10) && CarTranslateX >= (1489.242188 - 10)) {
			coin3 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 1 && coin4 && CarTranslateZ >= (-1071.335449 - 10) && CarTranslateZ <= (-1071.335449 + 10) && CarTranslateX <= (121.138977 + 10) && CarTranslateX >= (121.138977 - 10)) {
			coin4 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		//level 2 lab 2
		if (lab == 2 && booster1 && CarTranslateZ >= (933.230225 - 10) && CarTranslateZ <= (933.230225 + 10) && CarTranslateX <= (-0.614531 + 10) && CarTranslateX >= (-0.614531 - 10)) {
			booster1 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 2 && booster2 && CarTranslateZ >= (348.702209 - 10) && CarTranslateZ <= (348.702209 + 10) && CarTranslateX <= (1352.001953 + 10) && CarTranslateX >= (1352.001953 - 10)) {
			booster2 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 2 && booster3 && CarTranslateZ >= (-1266.510010 - 10) && CarTranslateZ <= (-1266.510010 + 10) && CarTranslateX <= (385.628235 + 10) && CarTranslateX >= (385.628235 - 10)) {
			booster3 = false;
			PlaySound("Nitrous_Burst_Sound_Effect_Short.wav", 0, SND_FILENAME | SND_ASYNC);
			speed = speed * 2;
			glutTimerFunc(4000, speedDown, 0);

		}
		if (lab == 2 && coin1 && CarTranslateZ >= (142.551834 - 10) && CarTranslateZ <= (142.551834 + 10) && CarTranslateX <= (-27.369238 + 10) && CarTranslateX >= (-27.369238 - 10)) {
			coin1 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 2 && coin2 && CarTranslateZ >= (1477.840698 - 10) && CarTranslateZ <= (1477.840698 + 10) && CarTranslateX <= (621.480164 + 10) && CarTranslateX >= (621.480164 - 10)) {
			coin2 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 2 && coin3 && CarTranslateZ >= (-1222.242065 - 10) && CarTranslateZ <= (-1222.242065 + 10) && CarTranslateX <= (1262.777954 + 10) && CarTranslateX >= (1262.777954 - 10)) {
			coin3 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}
		if (lab == 2 && coin4 && CarTranslateZ >= (-1321.621948 - 10) && CarTranslateZ <= (-1321.621948 + 10) && CarTranslateX <= (683.224182 + 10) && CarTranslateX >= (683.224182 - 10)) {
			coin4 = false;
			coinsCollected++;

			PlaySound("coin_sound_effect.wav", 0, SND_FILENAME | SND_ASYNC);

		}




	}

	/* Dirt Conditions */
	//bool enterDirtZ1 = CarTranslateZ >= 265 && CarTranslateZ <= 415;
	//bool enterDirtX1 = CarTranslateX >= 140 && CarTranslateX <= 520;

	//bool enterDirtZ2 = CarTranslateZ >= -145 && CarTranslateZ <= 415;
	//bool enterDirtX2 = CarTranslateX >= 380 && CarTranslateX <= 520;
	///* Enter Dirt */
	//if (((enterDirtX1 && enterDirtZ1) || (enterDirtX2 && enterDirtZ2)) && !isDirt) {
	//	speed = speed / 2.5;
	//	isDirt = true;
	//}

	///* Leave Dirt */
	//if (!((enterDirtX1 && enterDirtZ1) || (enterDirtX2 && enterDirtZ2)) && isDirt) {
	//	speed = 2.5 * speed;
	//	isDirt = false;
	//	slowingFactor = accelerateFactor;
	//}

	if (finishGame) {
		if (coinsCollected > 10) {
			PlaySound("CROWD CHEER SOUND EFFECT.wav", 0, SND_FILENAME | SND_ASYNC);
		}
		else {
			PlaySound("Crowd Boo - Sound Effect HD.wav", 0, SND_FILENAME | SND_ASYNC);
		}
		finishGame = false;
	}

	//if (isDirt && speed > dirtSpeed) {
	//	speed -= slowingFactor;
	//	slowingFactor += accelerateFactor;
	//}

	//if (!isDirt && speed < dirtSpeed) {
	//	speed += slowingFactor;
	//	slowingFactor += accelerateFactor;
	//}

	//printf("%f %f\n", CarTranslateX, CarTranslateZ);

	if (dayOffset > 1) {
		isDay = false;
	}
	else if (dayOffset < 0) {
		isDay = true;
	}

	if (isDay) {
		dayOffset += 0.0025;
	}
	else {
		dayOffset -= 0.0025;
	}

	// Lighting
	if (dayAngle < 180) {
		dayAngle = dayOffset * 180;
	}
	else {
		dayAngle = 0;
	}

	printf("%f", dayOffset);
	InitLightSource();

	// Collectible Animation
	CollectibleAnimation();

	moveAroundScene();

	glutPostRedisplay();
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(80, 120);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(carMovement);

	glutKeyboardUpFunc(myKeyboardUp);
	glutSpecialUpFunc(carMovementUp);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutIdleFunc(idle);

	glutReshapeFunc(myReshape);

	myInit();

	//glutFullScreen();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}