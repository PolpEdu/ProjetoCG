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
#define GRAY     0.3, 0.3, 0.3, 1.0
#define RED		 1.0, 0.0, 0.0, 1.0
#define PURPLE	 0.5, 0.0, 0.5, 1.0

#define C_GRAS   0.4862745098, 0.98823529411, 0.0



//================================================================================
//------------------------------------------------------------ Sistema Coordenadas 
GLint		wScreen = 700, hScreen = 600;		//.. janela - pixeis
GLfloat		SKY_SIZE = 500;
GLfloat		SKY_HEIGHT = 50;

//========================================================= Animation & Movement Coords
GLfloat	  self_x = 0.0, self_y = 0.0, self_z = 0.0;
GLfloat	  rotation = 0;
GLfloat	  rotor_angle = 0.0;
GLfloat	  door_angle = 0.0;
GLfloat	  heli_tilt = 0.0;
GLint     animacao = 0;
GLfloat   altura = 0;
GLfloat   incALT = 0.1;


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
GLfloat  rVisao = 8, aVisao = -0.5 * PI, incVisao = 1;
GLfloat  obsP[] =	{ 0.0,60.0,-5.0 }; //rVisão é o FOV
GLfloat	 at[] =		{ 0.0, 0.0, 0.0 };
GLfloat	 anguloZ = 35;


//=========================================================== Variaveis e constantes
GLint     msec = 50;	//.. tempo entre frames - milisegundos
GLint	  GRASS_VOLUME = 2500;

//=========================================================== Flags



//========================================================= Objects
// Note: d_... -> draw_..
void d_polygon(int a, int b, int c, int d);
void d_helicopter();
void d_body();
void d_rotor();
void d_tail();
void d_leg();
void d_tail_rotor();
void d_window();
void d_grass();
void d_pad();
void d_building();
void d_houses();
void d_mountain();
void d_sky();
void d_clouds();
void d_road();
// void d_door(); not sure if this is going to be implemented

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
	
	// could have drawn a simple plane like this
	/*glBegin(GL_QUADS);
	glVertex3f(GRASS_VOLUME, 0, GRASS_VOLUME);
	glVertex3f(-GRASS_VOLUME, 0, GRASS_VOLUME);
	glVertex3f(-GRASS_VOLUME, 0, -GRASS_VOLUME);
	glVertex3f(GRASS_VOLUME, 0, -GRASS_VOLUME);
	glEnd();*/
		
	glBegin(GL_LINES); //draw mesh -> Collection of vertices
	// draw so many lines that it looks like a plane
	for (GLfloat i = 0; i < GRASS_VOLUME; i += 0.5) {
		glVertex3d(-GRASS_VOLUME, -11, i);
		glVertex3d(GRASS_VOLUME, -11, i);
		glVertex3d(i, -11, -GRASS_VOLUME);
		glVertex3d(i, -11, GRASS_VOLUME);
	}
	glEnd();
	glPopMatrix();
}
 
//========================================================== Lighting & Shading
//TODO


//========================================================== Movement Coords




//=========================================================================== Init
void initialize(void)
{
	glEnable(GL_DEPTH_TEST);	//������������������������������Profundidade

	//PARA SE VER TUDO, dou disable a esconder overall:
	// glDisable(GL_CULL_FACE);		//������������������������������Faces visiveis
	
	
	glEnableClientState(GL_COLOR_ARRAY);
}

//======================================
void drawScene() {
	d_grass();

	//==================================== Animanacao do ceu, este precisa de se mover com o observador
	glTranslatef(obsP[0] + 2, obsP[1] + 2, obsP[2] + 2);
	d_sky();
	glTranslatef(-obsP[0] + 2, -obsP[1] + 2, -obsP[2] + 2);
	

}




//====================================================== 
//====================================================== 
void display(void) {

	//================================================================= APaga 
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
		case 'a':
			obsP[0] += ammount;
			break;
		case 'd':
			obsP[0] -= ammount;
			break;
		case 'A':
			obsP[1] += ammount;
			break;
		case 'D':
			obsP[1] -= ammount;
			break;
		case 'w':
			obsP[2] += ammount;
			break;
		case 's':
			obsP[2] -= ammount;
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
		
	}
	if (key == GLUT_KEY_DOWN)  // Movimento para baixo
	{
		
	}
	if (key == GLUT_KEY_LEFT)  // Movimento para a esquerda
	{
		
	}
	if (key == GLUT_KEY_RIGHT) // Movimento para a direira
	{
		
	}

	glutPostRedisplay();
}


//======================================================= MAIN
//======================================================= MAIN
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wScreen, hScreen);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("ProjetoCG - Eduardo Nunes");

	initialize();

	glutSpecialFunc(teclasNotAscii);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(teclasNotAscii);

	
	
	
	glutTimerFunc(msec, Timer, 1);

	glutMainLoop();

	return 0;
}



