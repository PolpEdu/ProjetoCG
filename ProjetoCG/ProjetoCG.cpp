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
int random(int max) { return rand() % max; }

//================================================================================
//------------------------------------------------------------ Sistema Coordenadas 
GLint		wScreen = 700, hScreen = 600;		//.. janela - pixeis
GLfloat		SKY_SIZE = 500;
GLfloat		SKY_HEIGHT = 50;


//=========================================================== Variaveis e constantes
GLfloat		padRay = 15;
char		texto[30];
char		texto2[30];
GLint		msec = 50;	//.. tempo entre frames - milisegundos
GLfloat		xC = 2500.0;  // tamanho chão
GLint		vel = 5;		// vel de movimento
GLint		dim = 256;   //numero divisoes da grelha
GLint		materialCube = random(18);
GLint		materialRoof = random(18);

GLfloat H_vertices_quads[12][3] = {
	//middle GL_QUADS
	{ -2.5, GROUND_LEVEL + 2, -2.5 },
	{ 2.5,	GROUND_LEVEL + 2, -2.5 },
	{ 2.5,	GROUND_LEVEL + 2, 2.5 },
	{ -2.5, GROUND_LEVEL + 2, 2.5 },

	// left GL_QUADS
	{ -10,	GROUND_LEVEL + 2, 10 },
	{ -10,	GROUND_LEVEL + 2, -10 },
	{ -2.5, GROUND_LEVEL + 2, -10 },
	{ -2.5,	GROUND_LEVEL + 2, 10 },

	// right GL_QUADS
	{ 10,	GROUND_LEVEL + 2, -10 },
	{ 10,	GROUND_LEVEL + 2, 10 },
	{ 2.5,	GROUND_LEVEL + 2, 10 },
	{ 2.5,	GROUND_LEVEL + 2, -10 }
};

//=========================================================== Flags
int is_forward = 0, is_left = 0, is_right = 0, is_back = 0;

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
GLUquadricObj* quadric = gluNewQuadric();

GLint   Dia = 1;     //:::   'D' - Day
GLfloat intensidadeDia = 1.0;
GLfloat luzGlobalCorAmb[4] = { intensidadeDia*0.9, intensidadeDia*0.9,intensidadeDia, 1.0 }; // cor ambiente da luz global

//============================================================= Observador
GLfloat  rVisao =	8, aVisao = -0.5 * PI, incVisao = 1;
GLfloat  obsP[] =	{ 0.0, INITIAL_FRONTLEN, INITIAL_BACKLEN }; //rVisão é o FOV
GLfloat	 at[]	=	{ 0.0,  0.0,  0.0 };
GLfloat	 anguloZ=	35;

//============================================================= Luz
// luz lampada
GLfloat   luzR = 1.0;		 		//:::   'R'  
GLfloat   luzG = 0.972;				//:::   'G'  
GLfloat   luzB = 0.196;				//:::   'B'  
GLfloat	  intensidadeT = 0.85;		//:::   'I'

// luz lampada
GLfloat   luzPadR = 0.892;		 	 //:::   'R'  
GLfloat   luzPadG = 0.176;			 //:::   'G'  
GLfloat   luzPadB = 0.08;			 //:::   'B'  
GLfloat	  intensidadePadT = 0.75;	 //:::   'I'

// luz helicoptero
GLfloat   luzHeliR = 0.892;		 	 //:::   'R'  
GLfloat   luzHeliG = 0.876;			 //:::   'G'  
GLfloat   luzHeliB = 0.852;			 //:::   'B'  
GLfloat	  luzHeliIntensity = 0.85;	 //:::   'I'

GLfloat	lightpos[]		=	{ 8, 35, 90, 1.0 };	// posicao da luz candeiro
GLfloat CorHeliDif[4]	=	{ luzHeliR * luzHeliIntensity, luzHeliG * luzHeliIntensity, luzHeliB * luzHeliIntensity, 1.0 };
GLfloat CorHeliAmb[4]	=	{ luzHeliR * luzHeliIntensity, luzHeliG * luzHeliIntensity, luzHeliB * luzHeliIntensity, 1.0 };
GLfloat CorHeliEsp[4] =		{ luzHeliR * luzHeliIntensity, luzHeliG * luzHeliIntensity, luzHeliB * luzHeliIntensity, 1.0 };

GLfloat localCorDif[4]	=	{ luzR * intensidadeT, luzG * intensidadeT, luzB * intensidadeT, 0 };

GLfloat	redPadCorDif[4] =	{ luzPadR * intensidadePadT, luzPadG * intensidadePadT, luzPadB * intensidadePadT, 0 };

GLdouble normalGround[] =	{ 0.0, 1.0, 0.0 };

GLfloat l1_pad[4] = { padRay, GROUND_LEVEL + 4, padRay, 1.0 };
GLfloat l2_pad[4] = { -padRay, GROUND_LEVEL + 4, padRay, 1.0 };
GLfloat l3_pad[4] = { padRay, GROUND_LEVEL + 4, -padRay, 1.0 };
GLfloat l4_pad[4] = { -padRay, GROUND_LEVEL + 4, -padRay, 1.0 };

//pontual
GLfloat light_att_Const = 0.01f;
GLfloat light_att_lin = 0.005f;
GLfloat light_att_quad = 0.001f;

//pontual para o Pad
GLfloat light_att_Const_pad = 0.01f;
GLfloat light_att_lin_pad = 0.05f;
GLfloat light_att_quad_pad = 0.001f;

//focal
GLfloat spot_open = 30.0f;
GLfloat spot_dir[] = { 0, -0.5, 0, 0}; // y axis -45 degress
GLfloat spot_exp = 2.0f;


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

GLfloat grassAmb[] = { 0.7,0.95,0.7,1 };
GLfloat grassDif[] = { 0.7568 ,0.91424 ,0.7568 };
GLfloat grassSpec[] = { 0.833 ,0.927811 ,0.833 };
GLint   grassCoef = 0.8 * 128;

GLfloat skyAmb[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat skyDif[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat skySpec[]= { 1.0, 1.0, 1.0, 1.0 };
GLint   skyCoef = 0.8 * 128;

GLfloat transparentPlasticDif[] = { 0.5 ,0.5 ,0.5, 0.3 };

GLboolean transparentmode = false;


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
void d_light_pole();
void d_sky();
void fly_effect();

void d_light_pole() {
	GLfloat radius = 1;
	GLfloat height = 35;
	initMaterials(2);

	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	glTranslatef(lightpos[0] + 10, height-8, lightpos[2]);
	glPushMatrix();
		glPushMatrix();
			gluSphere(quadric, radius, 20, 20);
		glPopMatrix();
	
		glPushMatrix();
			glRotatef(90, 1, 0, 0);
			gluCylinder(quadric, radius, radius, height, 20, 20);
		glPopMatrix();

		glPushMatrix();
			glRotatef(-90, 0, 1, 0);
			gluCylinder(quadric, radius, radius, height/4, 20, 20);
		glPopMatrix();

		glPushMatrix();
			glDisable(GL_LIGHTING);
			glColor3d(luzR, luzG, luzB);
			glTranslatef(-8, 0, 0);
			// get the current coords after translation
			
			gluSphere(quadric, radius * 1.10, 20, 20);
			glEnable(GL_LIGHTING);
		glPopMatrix();
	glPopMatrix();
}

//========================================================= Helicopter
void d_helicopter()
{
	if (transparentmode) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, transparentPlasticDif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, transparentPlasticDif);
	}
	glPushMatrix();
	
		glTranslatef(self_x, self_y, self_z);
		fly_effect();
	
		glRotatef(rotation, 0.0, 1.0, 0.0);  //rotation
		glRotatef(heli_tilt, 0.0, 0.0, 1.0);  //tilting

		d_body();

		d_rotor();

		d_tail();

		d_legs();

		d_tail_rotor();

		d_window();

		translate_window();

	glPopMatrix();
	glDisable(GL_BLEND);
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
	initMaterials(8);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -28.0);
	glRotatef(-90.0, 0.0, 0.0, 1.0);
	glScalef(0.25, 0.11, 0.25);
	d_rotor();
	glPopMatrix();

}

void d_legs()
{
	initMaterials(9);
	glPushMatrix();
	
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
	
	initMaterials(8);
	

	glPushMatrix();
	
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
	glDisable(GL_BLEND);
}

void d_body() {
	initMaterials(11);
	glPushMatrix();
	glScalef(1.0, 1.0, 2.0);
	glutSolidSphere(heliRadius, 32, 32);
	glScalef(1.0, 1.0, 0.5);
	glPopMatrix();
}

void d_rotor() {
	initMaterials(8);
	glPushMatrix();
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
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(-padRay, GROUND_LEVEL + 1, -padRay);
			glVertex3f(padRay, GROUND_LEVEL + 1, -padRay);
			glVertex3f(padRay, GROUND_LEVEL + 1, padRay);
			glVertex3f(-padRay, GROUND_LEVEL + 1, padRay);
		glEnd();
	glPopMatrix();

	glDisable(GL_LIGHTING);
	// draw a circle in each padRay position
	glPushMatrix();
		glColor3d(luzPadR, luzPadG, luzPadB);
		glTranslatef(l1_pad[0], l1_pad[1], l1_pad[2]);
		glutSolidSphere(1, 10, 10);
		glTranslatef(0, 0, -padRay*2);
		glutSolidSphere(1, 10, 10);
		glTranslatef(-padRay*2, 0, 0);
		glutSolidSphere(1, 10, 10);
		glTranslatef(0, 0, padRay*2);
		glutSolidSphere(1, 10, 10);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	
	for (int i = 0; i < 12; i += 4) {
		initMaterials(2);
			glPushMatrix();
			glBegin(GL_QUADS);
				glNormal3f(0.0, 1.0, 0.0);
				glVertex3fv(H_vertices_quads[i]);
				glVertex3fv(H_vertices_quads[i + 1]);
				glVertex3fv(H_vertices_quads[i + 2]);
				glVertex3fv(H_vertices_quads[i + 3]);
			glEnd();
		glPopMatrix();
	}
}

void d_house()
{
	glPushMatrix();
		initMaterials(materialCube);

		glTranslatef(0.0, 5.0, 0.0);
		glutSolidCube(25.0);
		glTranslatef(0.0, -5.0, 0.0);

		initMaterials(materialRoof);
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
			

	d_pad();

	glPopMatrix();
}

// Sky is intended to move with the "camera"
void d_sky() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[!Dia]);
		glPushMatrix();
			glRotatef(250, 0, 1, 0);
			gluQuadricDrawStyle(esfera, GLU_FILL);
			gluQuadricNormals(esfera, GLU_SMOOTH);
			gluQuadricTexture(esfera, GL_TRUE);
			gluSphere(esfera, 800.0, 100, 100);
		glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

// simple plane has grass
void d_grass() {
	
	int				i, j;
	float			med_dim = (float)dim / xC;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, grassAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, grassDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, grassSpec);

	glPushMatrix();
		glTranslatef(-xC/2, GROUND_LEVEL, xC/2);
		glRotatef(90, -1, 0, 0);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_QUADS);
			for (i = 0; i < dim; i++) {
				for (j = 0; j < dim; j++) {
					glTexCoord2f((float)j / dim, (float)i / dim);
					glVertex3d((float)j / med_dim, (float)i / med_dim, 0);
					glTexCoord2f((float)(j + 1) / dim, (float)i / dim);
					glVertex3d((float)(j + 1) / med_dim, (float)i / med_dim, 0);
					glTexCoord2f((float)(j + 1) / dim, (float)(i + 1) / dim);
					glVertex3d((float)(j + 1) / med_dim, (float)(i + 1) / med_dim, 0);
					glTexCoord2f((float)j / dim, (float)(i + 1) / dim);
					glVertex3d((float)j / med_dim, (float)(i + 1) / med_dim, 0);
				}
			}
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void desenhaTexto(char* string, GLfloat x, GLfloat y, GLfloat z) {
	glRasterPos3f(x, y, z);
	while (*string)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *string++);
}
 
void updateLights(void) {
	GLfloat currentPosArr[4] = { self_x, self_y - 9, self_z + 10 , 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, localCorDif);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, light_att_Const);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light_att_lin);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, light_att_quad);

	
	// draw a yellow circle in this position
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glColor3d(luzR, luzG, luzB);
		glTranslatef(currentPosArr[0], currentPosArr[1], currentPosArr[2]);
		gluSphere(quadric, 2 * 1.10, 20, 20);
		glEnable(GL_LIGHTING);
	glPopMatrix();
	
	printf("%f %f %f\n", currentPosArr[0], currentPosArr[1], currentPosArr[2]);
	glLightfv(GL_LIGHT1, GL_POSITION, currentPosArr); // in the helicopter
	glLightfv(GL_LIGHT1, GL_DIFFUSE, CorHeliDif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, CorHeliEsp);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, light_att_Const_pad*0.01);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, light_att_lin_pad * 0.01);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, light_att_quad_pad*0.01);
	glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION, spot_dir);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot_open);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spot_exp);
	

	glLightfv(GL_LIGHT2, GL_POSITION, l1_pad);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, redPadCorDif);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, light_att_Const_pad);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, light_att_lin_pad);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, light_att_quad_pad);

	glLightfv(GL_LIGHT3, GL_POSITION, l2_pad);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, redPadCorDif);
	glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, light_att_Const_pad);
	glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, light_att_lin_pad);
	glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, light_att_quad_pad);
	
	glLightfv(GL_LIGHT4, GL_POSITION, l3_pad);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, redPadCorDif);
	glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, light_att_Const_pad);
	glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, light_att_lin_pad);
	glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, light_att_quad_pad);

	glLightfv(GL_LIGHT5, GL_POSITION, l4_pad);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, redPadCorDif);
	glLightf(GL_LIGHT5, GL_CONSTANT_ATTENUATION, light_att_Const_pad);
	glLightf(GL_LIGHT5, GL_LINEAR_ATTENUATION, light_att_lin_pad);
	glLightf(GL_LIGHT5, GL_QUADRATIC_ATTENUATION, light_att_quad_pad);

	if (!Dia) {
		// enable lights 0, 1, 2, 3, 4, 5
		for (int i = 0; i < 6; i++) {
			glEnable(GL_LIGHT0 + i);
		}
	}
	else {
		for (int i = 0; i < 6; i++) {
			glDisable(GL_LIGHT0 + i);
		}
	}
}

void initLights(void) {
	//����������������������������������������������������� Ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalCorAmb);
	glEnable(GL_LIGHTING);

	updateLights();
}


void initTexturas()
{

	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	imag.LoadBmpFile("sky.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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


	glEnable(GL_DEPTH_TEST);	// ������������������������������Profundidade
	
	glEnable(GL_NORMALIZE);
	
	glNormalPointer(GL_FLOAT, 0, normalGround);
	glEnableClientState(GL_NORMAL_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texturas);   // coordenadas textura
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	initLights();
}

//======================================
void drawScene() {
	d_grass();
	d_helicopter();
	d_buildings();
	d_light_pole();
	rotation_rotor();

	//==================================== Animanacao do ceu, este precisa de se mover com o observador
	glMaterialfv(GL_FRONT, GL_AMBIENT, skyAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, skyDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, skySpec);
	glMaterialf(GL_FRONT, GL_SHININESS, skyCoef);
	glTranslatef(obsP[0], obsP[1], obsP[2]);
	d_sky();
	glTranslatef(-obsP[0], -obsP[1], -obsP[2]);
}

//====================================================== 
void display(void) {

	//================================================================= Apaga 
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, wScreen, hScreen);
	
	glDisable(GL_LIGHTING);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // reset the projection matrix
	gluPerspective(100, (GLfloat)wScreen / (GLfloat)hScreen, 0.1, 1500.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();  // Reset the model-view matrix
	gluLookAt(obsP[0], obsP[1], obsP[2], at[0], at[1], at[2], 0, 1, 0); // girar a camera em si, as coordenadas são dadas por obsP e o ponto para apontar é 0 0 0
	
	glColor3f(1, 1, 1);
	sprintf_s(texto, 30, "%d - Dia 'D'", Dia);
	desenhaTexto(texto, -24, 1, -20);
	
	sprintf_s(texto2, 30, "%d - Dim '+'/'-'", dim);
	desenhaTexto(texto2, -24, -3, -20);
	glEnable(GL_LIGHTING);
	
	//=================================================================
	//����������������������������������������������������������Objectos
	updateLights();
	drawScene();
	
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

	switch (key) {
		//camera
		case 'j':
			obsP[0] += vel;
			break;
		case 'l':
			obsP[0] -= vel;
			break;
		case 'I':
			obsP[1] += vel;
			break;
		case 'K':
			obsP[1] -= vel;
			break;
		case 'i':
			obsP[2] += vel;
			break;
		case 'k':
			obsP[2] -= vel;
			break;

		//movement
		case 's':
		case 'S':
			if (self_y > 0) {
				self_y -= vel;
				at[1] -= vel;
			};
			if (obsP[1] > INITIAL_FRONTLEN) obsP[1] -= vel; // if to prevent going under helicopter
			break;
		case 'w':
		case 'W':
			self_y += vel;
			obsP[1] += vel;
			at[1] += vel;
			break;

		case 'd':case 'D':
			Dia = !Dia;
			if (Dia) {
				luzGlobalCorAmb[0] = 1.0;
				luzGlobalCorAmb[1] = 1.0;
				luzGlobalCorAmb[2] = 1.0;
			}
			else {
				luzGlobalCorAmb[0] = 0.10;
				luzGlobalCorAmb[1] = 0.10;
				luzGlobalCorAmb[2] = 0.10;
			}
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalCorAmb);
			glutPostRedisplay();
			break;
		//grelha
		case '+':
			dim = 2 * dim;
			if (dim > 512) dim = 512;
			glutPostRedisplay();
			return;
		case '-':
			dim = 0.5 * dim;
			if (dim < 1) dim = 1;
			glutPostRedisplay();
			return;
		case 'm':
		case 'M':
			materialCube = random(18);
			materialRoof = random(18);
			
			glutPostRedisplay();
			return;

		case 'T':
		case 't':
			transparentmode = !transparentmode;
			glutPostRedisplay();
			return;
		case 27:
			exit(0);
			break;
		}
}


void teclasNotAscii(int key, int x, int y) {

	//=========================================================
	//  <><><><><><>  Movimento do observador
	//=========================================================
	if (key == GLUT_KEY_UP)	// Movimento para cima
	{
		self_z		+= vel;
		obsP[2]		+= vel;
		at[2]		+= vel;
		is_forward   = vel;
	}
	if (key == GLUT_KEY_DOWN)  // Movimento para baixo
	{
		self_z	-= vel;
		obsP[2] -= vel;
		at[2]	-= vel;
		is_back  = vel;
	}
	if (key == GLUT_KEY_LEFT)  // Movimento para a esquerda
	{
		self_x	+= vel;
		obsP[0] += vel;
		at[0]	+= vel;
		is_left  = vel;
	}
	if (key == GLUT_KEY_RIGHT) // Movimento para a direira
	{
		self_x	-= vel;
		obsP[0] -= vel;
		at[0]	-= vel;
		is_right = vel;
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
	glutCreateWindow("ProjetoCG - Eduardo Nunes || W: ++altitude, S: --altitude, Arrow keys: move helicopter (i,I,k,K,j,l camera) || Divisões Grela (+,-) || Dia/Night (D) || Material (M)");

	initialize();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(teclasNotAscii);
	glutSpecialUpFunc(releasekeyboard);

	glutTimerFunc(msec, Timer, 1);

	glutMainLoop();

	return 0;
}
