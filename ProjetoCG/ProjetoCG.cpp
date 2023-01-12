/* ===================================================================================
	Departamento Eng. Informatica - FCTUC
	Computacao Grafica - 2022/23
	................................................ Eduardo Nunes
	ProjetoCG
======================================================================================= */

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
// Escolher:
//	   - Qual é o veículo?
//	   - Como é a geometria?
//     - Quais são as animações?
// 
// A implementar pelos alunos Meta 1
//     - Acabar a geometria -> Veiculo
//     - Acabar a animação
//			- Uma translação de um objeto especifico (e.g portas a abrir, corda a cair)
//			- Uma rotação de elementos diferentes
//     - Tentar fazer as cameras
// 
// 
// I intent to draw a helicopter that moves in a space with the helicopter rotor rotating.
// The helicopter will land and open a door at the end of the animation
// 
// The helicopter's body can be a sphere (?)
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include "RgbImage.h"

#define PI		 3.14159

//--------------------------------- Definir cores
#define WHITE    1.0, 1.0, 1.0, 1.0

#define INITIAL_BACKLEN -50.0
#define INITIAL_FRONTLEN 50.0
#define GROUND_LEVEL	-11.0


void initMaterials(int material);


//================================================================================
//------------------------------------------------------------ Sistema Coordenadas 
GLint		wScreen = 700, hScreen = 600;		//.. janela - pixeis
GLfloat		SKY_SIZE = 500;
GLfloat		SKY_HEIGHT = 50;

//========================================================= Animation & Movement Coords
GLfloat	  self_x = 0.0, self_y = 0.0, self_z = 0.0;
GLfloat	  rotation = 0;
GLfloat	  rotor_angle = 0.0;
GLfloat	  heli_tilt = 0.0;
GLfloat	  animacaoJanela = 0, invert = -1;
GLfloat   altura = 0;
GLfloat   incALT = 0.1;

GLfloat   heliRadius = 5.5;

GLUquadricObj* esfera = gluNewQuadric();

GLint   Dia = 1;     //:::   'D' - Day
GLfloat intensidadeDia = 1.0;
GLfloat luzGlobalCorAmb[4] = { intensidadeDia, intensidadeDia,intensidadeDia, 1.0 }; // cor ambiente da luz global

//============================================================= Observador
GLfloat  rVisao =	8, aVisao = -0.5 * PI, incVisao = 1;
GLfloat  obsP[] =	{ 0.0, INITIAL_FRONTLEN, INITIAL_BACKLEN }; //rVisão é o FOV
GLfloat	 at[]	=	{ 0.0,  0.0,  0.0 };
GLfloat	 anguloZ= 35;


GLfloat   luzR = 1.0;		 	 //:::   'R'  
GLfloat   luzG = 1.0;			 //:::   'G'  
GLfloat   luzB = 1.0;			 //:::   'B'  
GLfloat localPos[4] = { 0.0, 5.0, 0.0, 1.0 };
GLfloat localCorAmb[4] = { 1, 1, 1, 0.0 };
GLfloat localCorDif[4] = { luzR, luzG, luzB, 1.0 };
GLfloat localCorEsp[4] = { luzR, luzG, luzB, 1.0 };

char     texto[30];

//=========================================================== Variaveis e constantes
GLint     msec = 50;	//.. tempo entre frames - milisegundos
GLfloat  xC = 2500.0;
//=========================================================== Flags
int is_forward = 0, is_left = 0, is_right = 0, is_back = 0;

//=========================================================== Textures
GLuint   texture[5];
RgbImage imag;
static GLfloat texturas[] = {
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1 };

GLdouble normalGround[] = { 0.0, 1.0, 0.0 };


//========================================================= Objects
void d_helicopter();
void d_body();
void d_rotor();
void d_tail();
void d_legs();
void d_tail_rotor();
void d_window();
void d_grass();
void d_pad();
void d_buildings();
void d_house();
void translate_window();
// void d_mountain();
void d_sky();
// void d_clouds();
void fly_effect();

//========================================================= Helicopter
void d_helicopter()
{
	
	glPushMatrix();
	
		glTranslatef(self_x, self_y, self_z);
		fly_effect();
	
		glRotatef(rotation, 0.0, 1.0, 0.0);  //rotation
		glTranslatef(-self_x, -self_y, -self_z);
		glTranslatef(self_x, self_y, self_z);
		glRotatef(heli_tilt, 0.0, 0.0, 1.0);  //tilting

		initMaterials(11);
		glPushMatrix();
		d_body();
		glPopMatrix();

		d_rotor();

		d_tail();

		d_legs();

		d_tail_rotor();

		d_window();

		translate_window();

	glPopMatrix();
}

void d_window() {
	initMaterials(2);
	//point upwards
	glRotated(-90.0, 1.0, 0.0, 0.0);

	glTranslatef(-3.0, -3.0, animacaoJanela);
	//window needs to be also spherical
	glPushMatrix();
	
		// draw a cilinder with a sphere on top
		glScaled(0.3, 0.3, 0.4);
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.0);
		glRotatef(0.0, 1.0, 0.0, 0.0);
		glutSolidCone(5, 30, 20, 20);
		glPopMatrix();
		
	
	
		glTranslatef(0.0, 0.0, 30.0);
		//DRAW A SPHERE ON TOP
		glPushMatrix();
		glutSolidSphere(3, 20, 20);
		glPopMatrix();
		
	glPopMatrix();
}

void translate_window() {
	
	if (animacaoJanela < -1) {
		invert = 1;
	}
	else if (animacaoJanela > 4) {
		invert = -1;
	}

	animacaoJanela += invert;
}

// rotor rotation logic
void rotation_rotor() {
	// first animaton rotor_angle
	if (rotor_angle < 100) rotor_angle += 200.0;
	else if (rotor_angle < 200) rotor_angle += 20.5;
	else if (rotor_angle < 300) rotor_angle += 300.0;
	else if (rotor_angle < 400) rotor_angle += 300.5;
	else rotor_angle += 500.0;
}


void fly_effect() {
	if (is_forward)
		glRotatef(10.0, 1.0, 0.0, 0.0);

	if (is_right)
		glRotatef(30.0, 0.0, 0.0, 1.0);

	if (is_left)
		glRotatef(-30.0, 0.0, 0.0, 1.0);

	if (is_back)
		glRotatef(-10.0, 1.0, 0.0, 0.0);


}

void d_tail_rotor()
{
	glPushMatrix();
	glTranslatef(0.0, 0.0, -28.0);
	glRotatef(-90.0, 0.0, 0.0, 1.0);
	glScalef(0.25, 0.11, 0.25);
	d_rotor();
	glPopMatrix();

}

void d_legs()
{
	glPushMatrix();

	initMaterials(9);
	
	glBegin(GL_LINES);
	//first leg
	glVertex3f(5, -7, 7);
	glVertex3f(5, -7, -7);

	glVertex3f(5, -7, 7);
	glVertex3f(5, -6, 8);

	glVertex3f(5, -7, -7);
	glVertex3f(5, -6, -8);

	glVertex3f(0, 0.0, 5);
	glVertex3f(5, -7.0, 5);

	glVertex3f(0, 0.0, -5);
	glVertex3f(5, -7.0, -5);

	glVertex3f(-5, -7, 7);
	glVertex3f(-5, -7, -7);

	//second leg
	glVertex3f(-5, -7, 7);
	glVertex3f(-5, -6, 8);

	glVertex3f(-5, -7, -7);
	glVertex3f(-5, -6, -8);

	glVertex3f(0, 0.0, 5);
	glVertex3f(-5, -7.0, 5);

	glVertex3f(0, 0.0, -5);
	glVertex3f(-5, -7.0, -5);

	glEnd();

	glPopMatrix();
}

void d_tail()
{
	//tail of heli
	glPushMatrix();
	initMaterials(1);
	
	glBegin(GL_LINE_LOOP);
	glVertex3f(1.2, -2.0, -9.0);
	glVertex3f(1.2, 2.0, -9.0);
	glVertex3f(0.2, 0.2, -29.0);
	glVertex3f(0.2, -0.2, -29.0);
	glEnd();


	glBegin(GL_LINE_LOOP);
	glVertex3f(-1.2, -2.0, -9.0);
	glVertex3f(-1.2, 2.0, -9.0);
	glVertex3f(-0.2, 0.2, -29.0);
	glVertex3f(-0.2, -0.2, -29.0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(1.2, 2.0, -9.0);
	glVertex3f(-1.2, 2.0, -9.0);
	glVertex3f(-0.2, 0.2, -29.0);
	glVertex3f(0.2, 0.2, -29.0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(1.2, -2.0, -9.0);
	glVertex3f(-1.2, -2.0, -9.0);
	glVertex3f(-0.2, -0.2, -29.0);
	glVertex3f(0.2, -0.2, -29.0);
	glEnd();

	glPopMatrix();
}

void d_body() {
	
	glPushMatrix();
	glScalef(1.0, 1.0, 2.0);
	glutSolidSphere(heliRadius, 32, 32);
	glScalef(1.0, 1.0, 0.5);
	glPopMatrix();
}

void d_rotor() {
	glPushMatrix();

	initMaterials(1);
	glLineWidth(5.0);

	glBegin(GL_LINES);//fan stick
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10.0, 0.0);
	glEnd();
	glScalef(2.0, 1.0, 2.0);


	//rotor blades
	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotatef(rotor_angle, 0.0, 1.0, 0.0);


		glTranslatef(0.0, 10.0, 0.0);
		glRotatef(30.0, 1.0, 0.0, 0.0);
		glTranslatef(0.0, -10.0, 0.0);

		glBegin(GL_QUADS);

		glVertex3f(0.0, 10.0, -0.5);
		glVertex3f(10.0, 10.0, -1.0);
		glVertex3f(10.0, 10.0, 1.0);
		glVertex3f(0.0, 10.0, 0.5);

		glEnd();
		glPopMatrix();

		glRotatef(90.0, 0.0, 1.0, 0.0);
	}

	glPopMatrix();
}

//======================================================

void d_pad() {
	initMaterials(16);

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3dv(normalGround);
	glVertex3f(-10.0, -10.0, -10.0);
	glNormal3dv(normalGround);
	glVertex3f(10.0, -10.0, -10.0);
	glNormal3dv(normalGround);
	glVertex3f(10.0, -10.0, 10.0);
	glNormal3dv(normalGround);
	glVertex3f(-10.0, -10.0, 10.0);
	glEnd();
	glPopMatrix();
}

void d_house()
{

	glPushMatrix();

	initMaterials(15);
	glTranslatef(0.0, 5.0, 0.0);
	glutSolidCube(25.0);
	glTranslatef(0.0, -5.0, 0.0);

	initMaterials(1);
	glTranslatef(0.0, 15.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(gluNewQuadric(), 25.0, 0.0, 25.0, 32, 32);
	glTranslatef(0.0, -15.0, 0.0);

	glPopMatrix();

}


void d_buildings() {
	glPushMatrix();
		float x, z;
		for (z = 100; z <= 1000; z += 150) {
			for (x = -400; x <= 400; x += 150)
			{
				glPushMatrix();

				glTranslatef(x, 0.0, z);
				d_house();
				glTranslatef(-x, 0.0, -z);

				glTranslatef(-x, 0.0, z);
				d_house();
				glTranslatef(x, 0.0, -z);

				glPopMatrix();
			}
		}
			

		glPushMatrix();
			glTranslatef(0.0, 0.0, 0.0);
			d_pad();
			glTranslatef(0.0, 0.0, 0.0);
		glPopMatrix();

	glPopMatrix();
}

// Sky is intended to move with the "camera"
void d_sky() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[!Dia]);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluQuadricDrawStyle(esfera, GLU_FILL);
	gluQuadricNormals(esfera, GLU_SMOOTH);
	gluQuadricTexture(esfera, GL_TRUE);
	gluSphere(esfera, 800.0, 100, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void d_grass() {
	float L = 0.8;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	initMaterials(0);
	glPushMatrix();
	// simple plane has grass
		glBegin(GL_QUADS);
		glNormal3dv(normalGround);			glTexCoord3f(0.0f, 0.0f, 0.0f);		glVertex3i(-L * xC, -11, L * xC); //A 
		glNormal3dv(normalGround);			glTexCoord3f(6.0f, 0.0f, 0.0f);		glVertex3i(L * xC, -11, L * xC);  //B
		glNormal3dv(normalGround);			glTexCoord3f(6.0f, 6.0f, 0.0f);		glVertex3i(L * xC, -11, -L * xC); //C
		glNormal3dv(normalGround);			glTexCoord3f(0.0f, 6.0f, 0.0f);		glVertex3i(-L * xC, -11, -L * xC); //D
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void desenhaTexto(char* string, GLfloat x, GLfloat y, GLfloat z) {
	glRasterPos3f(x, y, z);
	while (*string)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *string++);
}
 
void initLights(void) {
	//����������������������������������������������������� Ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalCorAmb);

	//����������������������������������������������������� Teto
	/*glLightfv(GL_LIGHT0, GL_POSITION, localPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, localCorAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, localCorDif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, localCorEsp);*/
}

void initTexturas()
{
	//----------------------------------------- Chao - tapete
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	imag.LoadBmpFile("sky.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	imag.LoadBmpFile("noite.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	glGenTextures(1, &texture[2]);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	imag.LoadBmpFile("grass.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, Dia ? GL_MODULATE : GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

}


//=========================================================================== Init
void initialize(void)
{
	glClearColor(WHITE);
	glShadeModel(GL_SMOOTH);
	initTexturas();

	glEnable(GL_DEPTH_TEST);	//������������������������������Profundidade
	glEnable(GL_LIGHTING);

	
	glNormalPointer(GL_FLOAT, 0, normalGround);
	glEnableClientState(GL_NORMAL_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texturas);   // coordenadas textura
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	initLights();
	initMaterials(11);
}

//======================================
void drawScene() {
	d_grass();
	d_helicopter();
	d_buildings();

	//==================================== Animanacao do ceu, este precisa de se mover com o observador
	glTranslatef(obsP[0], obsP[1], obsP[2]);
	d_sky();
	glTranslatef(-obsP[0], -obsP[1], -obsP[2]);

	sprintf_s(texto, 30, "%d - Dia 'D' ", Dia);
	desenhaTexto(texto, -12, 1, -14);
	
	/*printf("%f\n", obsP[1]);
	printf("%f\n", self_y);*/
}

//====================================================== 
void display(void) {

	//================================================================= Apaga 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, wScreen, hScreen);
	glMatrixMode(GL_PROJECTION);
	
	glLoadIdentity(); // reset the projection matrix
	gluPerspective(100, (GLfloat)wScreen / (GLfloat)hScreen, 0.1, 1500.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();  // Reset the model-view matrix

	gluLookAt(obsP[0], obsP[1], obsP[2], at[0], at[1], at[2], 0, 1, 0); // girar a camera em si, as coordenadas são dadas por obsP e o ponto para apontar é 0 0 0

	//=================================================================
	//����������������������������������������������������������Objectos
	drawScene();
	
	//======================================================= Animations
	rotation_rotor();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Actualizacao
	glutSwapBuffers();
}


void Timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(msec, Timer, 1);
}

//======================================================= EVENTOS
void keyboard(unsigned char key, int x, int y) {

	const GLfloat ammount = 1.0;
	switch (key) {
		//camera
		case 'j':
			obsP[0] += ammount;
			break;
		case 'l':
			obsP[0] -= ammount;
			break;
		case 'I':
			obsP[1] += ammount;
			break;
		case 'K':
			obsP[1] -= ammount;
			break;
		case 'i':
			obsP[2] += ammount;
			break;
		case 'k':
			obsP[2] -= ammount;
			break;

		//movement
		case 's':
		case 'S':
			if (self_y > 0) {
				self_y -= ammount;
				at[1] -= ammount;
			};
			if (obsP[1] > INITIAL_FRONTLEN) obsP[1] -= ammount; // if to prevent going under helicopter
			break;
		case 'w':
		case 'W':
			self_y += ammount;
			obsP[1] += ammount;
			at[1] += ammount;
			break;

		case 'd':case 'D':
			Dia = !Dia;
			if (Dia) {
				luzGlobalCorAmb[0] = 1.0;
				luzGlobalCorAmb[1] = 1.0;
				luzGlobalCorAmb[2] = 1.0;
			}
			else {
				luzGlobalCorAmb[0] = 0.20;
				luzGlobalCorAmb[1] = 0.20;
				luzGlobalCorAmb[2] = 0.20;
			}
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalCorAmb);
			glutPostRedisplay();
			break;
		case 27:
			exit(0);
			break;
		}
	glutPostRedisplay();
}


void teclasNotAscii(int key, int x, int y) {

	//=========================================================
	//  <><><><><><>  Movimento do observador
	//=========================================================
	if (key == GLUT_KEY_UP)	// Movimento para cima
	{
		self_z += 1;
		obsP[2] += 1;
		at[2] += 1;
		is_forward = 1;
	}
	if (key == GLUT_KEY_DOWN)  // Movimento para baixo
	{
		self_z -= 1;
		obsP[2] -= 1;
		at[2] -= 1;
		is_back = 1;
	}
	if (key == GLUT_KEY_LEFT)  // Movimento para a esquerda
	{
		self_x += 1;
		obsP[0] += 1;
		at[0] += 1;
		is_left = 1;
	}
	if (key == GLUT_KEY_RIGHT) // Movimento para a direira
	{
		self_x -= 1;
		obsP[0] -= 1;
		at[0] -= 1;
		is_right = 1;
	}

	glutPostRedisplay();
}

void releasekeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)		{ is_forward = 0; }
	if (key == GLUT_KEY_RIGHT)	{ is_right = 0; }
	if (key == GLUT_KEY_LEFT)	{ is_left = 0; }
	if (key == GLUT_KEY_DOWN)	{ is_back = 0; }
}

//======================================================= MAIN
//======================================================= MAIN
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wScreen, hScreen);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("ProjetoCG - Eduardo Nunes || W: increase altitude, S: decrease altitude, Arrow keys: move helicopter (i,I,k,K,j,l camera)");

	initialize();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(teclasNotAscii);
	glutSpecialUpFunc(releasekeyboard);

	glutTimerFunc(msec, Timer, 1);

	glutMainLoop();

	return 0;
}
