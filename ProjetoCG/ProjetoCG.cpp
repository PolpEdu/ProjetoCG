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
#define PI		 3.14159

//--------------------------------- Definir cores
#define BLUE     0.0, 0.0, 1.0, 1.0
#define YELLOW	 1.0, 1.0, 0.0, 1.0
#define GREEN    0.0, 1.0, 0.0, 1.0
#define ORANGE   1.0, 0.5, 0.1, 1.0
#define CYAN     0.0, 1.0, 1.0, 1.0
#define WHITE    1.0, 1.0, 1.0, 1.0
#define BLACK    0.0, 0.0, 0.0, 1.0
#define GRAY     0.55, 0.55, 0.55, 1.0
#define RED		 0.651, 0.1725, 0.1686, 1.0
#define RED_H	 1.0, 0.0, 0.0, 1.0
#define PURPLE	 0.5, 0.0, 0.5, 1.0

#define C_GRAS   0.4862745098, 0.98823529411, 0.0

#define INITIAL_BACKLEN -50.0
#define INITIAL_FRONTLEN 50.0
#define GROUND_LEVEL	-11.0

// An array with all the colors
GLfloat colorsrandom[10][4] = { {BLUE}, {YELLOW}, {GREEN}, {ORANGE}, {CYAN}, {WHITE}, {BLACK}, {GRAY}, {RED}, {PURPLE} };

//function that returns a random color
GLfloat* randomColor() { return colorsrandom[rand() % 10]; }

//================================================================================
//------------------------------------------------------------ Sistema Coordenadas 
GLint		wScreen = 700, hScreen = 600;		//.. janela - pixeis
GLfloat		SKY_SIZE = 500;
GLfloat		SKY_HEIGHT = 50;

//========================================================= Animation & Movement Coords
GLfloat	  self_x = 0.0, self_y = 0.0, self_z = 0.0;
GLfloat	  rotation = 0;
GLfloat	  rotor_angle = 0.0;
// GLfloat	  door_angle = 0.0;
GLfloat	  heli_tilt = 0.0;
GLfloat	  animacaoJanela = 0, invert = -1;
GLfloat   altura = 0;
GLfloat   incALT = 0.1;

GLfloat   heliRadius = 5.5;


//coordinates of cube for sky
GLfloat vertices_sky[8][3] = {
	{-SKY_SIZE, -SKY_HEIGHT, -SKY_SIZE},
	{ SKY_SIZE, -SKY_HEIGHT, -SKY_SIZE},
	{ SKY_SIZE, SKY_SIZE, -SKY_SIZE},
	{-SKY_SIZE, SKY_SIZE, -SKY_SIZE},
	{-SKY_SIZE, -SKY_HEIGHT,  SKY_SIZE},
	{ SKY_SIZE, -SKY_HEIGHT,  SKY_SIZE},
	{ SKY_SIZE, SKY_SIZE,  SKY_SIZE},
	{-SKY_SIZE, SKY_SIZE,  SKY_SIZE}
};

//colors of skycube
GLdouble colors[8][4] = {
	{0.8862745098039216,1.0,1.0,0.5},
	{0.8862745098039216,1.0,1.0,0.5},
	{0.4,0.8784313725490196,1.0,0.5},
	{0.4,0.8784313725490196,1.0,0.5},
	{0.8862745098039216,1.0,1.0,0.5},
	{0.8862745098039216,1.0,1.0,0.5},
	{0.4,0.8784313725490196,1.0,0.5},
	{0.4,0.8784313725490196,1.0,0.5}
};


//============================================================= Observador
GLfloat  rVisao =	8, aVisao = -0.5 * PI, incVisao = 1;
GLfloat  obsP[] =	{ 0.0, INITIAL_FRONTLEN, INITIAL_BACKLEN }; //rVisão é o FOV
GLfloat	 at[]	=	{ 0.0,  0.0,  0.0 };
GLfloat	 anguloZ= 35;


//=========================================================== Variaveis e constantes
GLint     msec = 50;	//.. tempo entre frames - milisegundos
GLint	  GRASS_VOLUME = 2500;

//=========================================================== Flags
int is_forward = 0, is_left = 0, is_right = 0, is_back = 0;


//========================================================= Objects
void d_polygon(int a, int b, int c, int d);
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
// void d_road();
void fly_effect();

// void d_door(); not sure if this is going to be implemented

//========================================================= Helicopter
void d_helicopter()
{
	glPushMatrix();
		
	glColor4f(RED);
	
		glTranslatef(self_x, self_y, self_z);
		fly_effect();
	
		glRotatef(rotation, 0.0, 1.0, 0.0);  //rotation
		glTranslatef(-self_x, -self_y, -self_z);
		glTranslatef(self_x, self_y, self_z);
		glRotatef(heli_tilt, 0.0, 0.0, 1.0);  //tilting

		d_body();

		d_rotor();

		d_tail();

		d_legs();

		d_tail_rotor();

		d_window();

		translate_window();

	glPopMatrix();
}

void d_window() {
	glColor4f(PURPLE);//grey
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

	glColor4f(0.7, 0.7, 0.7, 1.0);//grey

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
	glColor4f(GRAY);//grey

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

	glColor4f(GRAY);//silver
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
	glColor4f(YELLOW);
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex3f(-10.0, -10.0, -10.0);
	glVertex3f(10.0, -10.0, -10.0);
	glVertex3f(10.0, -10.0, 10.0);
	glVertex3f(-10.0, -10.0, 10.0);
	glEnd();
	glPopMatrix();
}

void d_house()
{

	glPushMatrix();

	glColor4f(RED_H);
	glTranslatef(0.0, 5.0, 0.0);
	glutSolidCube(25.0);
	glTranslatef(0.0, -5.0, 0.0);

	glColor4f(BLUE);
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

// Function to create a polygon
void d_polygon(int a, int b, int c, int d) {
	glPushMatrix();
	
		glBegin(GL_POLYGON);
		glColor4dv(colors[a]);
		glVertex3fv(vertices_sky[a]);
		glColor4dv(colors[b]);
		glVertex3fv(vertices_sky[b]);
		glColor4dv(colors[c]);
		glVertex3fv(vertices_sky[c]);
		glColor4dv(colors[d]);
		glVertex3fv(vertices_sky[d]);
		glEnd();
		
	glPopMatrix();
}

// Sky is intended to move with the "camera"
void d_sky() {
	glPushMatrix();
	//regra da mão direita
	d_polygon(0, 3, 2, 1);
	d_polygon(0,4,7,3);
	d_polygon(2,3,7,6);
	d_polygon(1,2,6,5);
	d_polygon(4,5,6,7);
	glPopMatrix();
}

void d_grass() {
	glPushMatrix();
	glColor3f(C_GRAS);
	
	// simple plane has grass
	glBegin(GL_QUADS);
	glVertex3f(GRASS_VOLUME, GROUND_LEVEL, GRASS_VOLUME);
	glVertex3f(-GRASS_VOLUME, GROUND_LEVEL, GRASS_VOLUME);
	glVertex3f(-GRASS_VOLUME, GROUND_LEVEL, -GRASS_VOLUME);
	glVertex3f(GRASS_VOLUME, GROUND_LEVEL, -GRASS_VOLUME);
	glEnd();
		
	glPopMatrix();
}
 
//=========================================================================== Init
void initialize(void)
{
	glEnable(GL_DEPTH_TEST);	//������������������������������Profundidade

	// PARA SE VER TUDO, dou disable a esconder overall:
	// glDisable(GL_CULL_FACE);		//������������������������������Faces visiveis
	
	
	glEnableClientState(GL_COLOR_ARRAY);
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

	//======================================================
	//  <><><><><><><>         OBSERVADOR NAO EST� FIXO ????
	//======================================================
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
