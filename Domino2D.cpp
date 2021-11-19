#include <windows.h>  
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <random>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <sstream>

#include "glm/glm/glm.hpp"  
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#include "loadShaders.h"

const float C_PI = 3.141592f;
const glm::mat4 C_IDENTITY_MATRIX = glm::mat4(1);

struct RectangleShape {
	float x;
	float y;
	float height;
	float width;

	RectangleShape(const float& x, const float& y, const float& height, const float& width) : x(x), y(y), height(height), width(width) {}
	RectangleShape() : x(0.0F), y(0.0F), height(0.0F), width(0.0F) {}
};

// Window params
const int C_WINDOW_WIDTH = 800;
const int C_WINDOW_HEIGHT = 600;

//Draw params
const int C_REFRESH_TIME = 25;
const int C_NUM_MAX_PIECES = 14;

//Angles variables
const float C_STOP_ROTATION_ANGLE = 75.0F;
const float C_STOP_ROTATION_ANGLE_LAST_PIECE = 90.0F;
const float C_ANGLE_TO_ACTIVATE_NEXT_PIECE = 30.0F;

// Domino piece params
const float C_SPACE_BETWEEN_PIECES = 80.0F; // includes the width of the domino
const float C_DOMINO_ANGLE_OFFSET = 5.0F;

// Table params
const float C_TABLE_WIDTH = 40.0F;
const float C_TABLE_OFFSET = 50.0F;

// Pendul params
const float C_SUPORT_PENDUL_X_COORD_OFFSET = 30.0F;
const float C_SUPORT_PENDUL_Y_COORD_OFFSET = 250.0F;
const float C_PENDUL_X_COORD_OFFSET = 150.0F;
const float C_PENDUL_ANGLE_OFFSET = 5.0F;
const float C_PENDUL_ANGLE_STOP = 90.0F;
const float C_PENDUL_RESIZE = 1.0F;

//////////////////////////////////////

RectangleShape domino(0.0F, -100.0F, 120.0F, 20.0F);
RectangleShape table;


GLuint VaoId, VboId, ColorBufferId, ProgramId;
glm::mat4 resizeMatrix, matrRot, trans;
glm::vec3 pieceRotatePoint;
glm::vec3 pendulRotatePoint;

float suportPendulXPos;
float suportPendulYPos;
glm::vec3 PendulRotatePoint;


float angles[C_NUM_MAX_PIECES];
float pendulSize = 30.0F;
int numOfPieces;
bool startDomino = false;
bool startPendul = false;
float pendulAngle = 0.0F;

enum Colors {
	Normal = 0,
	Blue,
	Red,
	Green,
	White,
	Black,
	Gradient,
};

void Initialize();

void RenderFunction();

void MouseAction(int, int, int, int);

void CreateVBO(void);

void Cleanup();

bool ValidInput(const std::string& input);
 
void AnglesUpdate(int input);

void PendulUpdate(int input);

template<class T>
void LoadUniformVar(const GLuint& programId, const std::string& name, T value);

void Load4x4MatrixToVertShader(const std::string& iName, const glm::mat4& iMatrix);

int main(int argc, char* argv[]) {
	
	std::string input;
	std::cout << "Introduceti numarul de piese: (" << C_NUM_MAX_PIECES << " piese maxim)\n";
	std::cin >> input;
	while (!ValidInput(input)) {
		std::cout << "Numarul introdus este invalid, incercati din nou\n";
		std::cin >> input;
	}
	numOfPieces = std::stoi(input);


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Domino 2D");
	glutDisplayFunc(RenderFunction);
	glutMouseFunc(MouseAction);
	glewInit();
	Initialize();
	glutTimerFunc(C_REFRESH_TIME, PendulUpdate, 0);
	glutMainLoop();
	glutCloseFunc(Cleanup);

}

bool ValidInput(const std::string& input) {

	for (const char& chr : input) {
		if (!std::isdigit(chr)) {
			return false;
		}
	}

	int number = std::stoi(input);
	if (number <= 0 || number > C_NUM_MAX_PIECES) {
		return false;
	}

	return true;
}

void CreateVBO(void) {
	GLfloat Vertices[] = {
		// background  
	   -C_WINDOW_WIDTH, -C_WINDOW_HEIGHT, 0.0f, 1.0f,
		C_WINDOW_WIDTH,  C_WINDOW_HEIGHT, 0.0f, 1.0f,
		C_WINDOW_WIDTH, -C_WINDOW_HEIGHT, 0.0f, 1.0f,
		//
	   -C_WINDOW_WIDTH, -C_WINDOW_HEIGHT, 0.0f, 1.0f,
		C_WINDOW_WIDTH,  C_WINDOW_HEIGHT, 0.0f, 1.0f,
	   -C_WINDOW_WIDTH,  C_WINDOW_HEIGHT, 0.0f, 1.0f,
	  // varfuri pentru axe
	   -C_WINDOW_WIDTH,  0.0f,			  0.0f, 1.0f,
		C_WINDOW_WIDTH,  0.0f,			  0.0f, 1.0f,
	    0.0f,		    -C_WINDOW_HEIGHT, 0.0f, 1.0f,
	    0.0f,			 C_WINDOW_HEIGHT, 0.0f, 1.0f,

	   // first domino piece
	   domino.x,				  domino.y,				      0.0F, 1.0F,
	   domino.x + domino.width, domino.y,				      0.0F, 1.0F,
	   domino.x + domino.width, domino.y + domino.height, 0.0F, 1.0F,
	   domino.x,				  domino.y + domino.height, 0.0F, 1.0F,

	   // suport pendul center point
	   suportPendulXPos,          suportPendulYPos,           0.0F, 1.0F,
	   // pendul center point 
	   suportPendulXPos - C_PENDUL_X_COORD_OFFSET, suportPendulYPos, 0.0F, 1.0F,
 	   // Draw the table
	   table.x,					table.y,				  0.0F, 1.0F,
	   table.x + table.width, table.y,				  0.0F, 1.0F,
	   table.x + table.width, table.y + table.height, 0.0F, 1.0F,
	   table.x,				    table.y + table.height, 0.0F, 1.0F,
	};


	GLfloat Colors[] = {
	  1.0f, 0.0f, 0.0f, 1.0f,
	  0.0f, 1.0f, 0.0f, 1.0f,
	  0.0f, 0.0f, 1.0f, 1.0f,
	  1.0f, 0.0f, 0.0f, 1.0f,
	  0.0f, 1.0f, 0.0f, 1.0f,
	  0.0f, 0.0f, 1.0f, 1.0f,
	};

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void DestroyVBO(void) {
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void CreateShaders(void) {
	ProgramId = LoadShaders("temaVert.shader", "temaFrag.shader");
	glUseProgram(ProgramId);
}


void DestroyShaders(void) {
	glDeleteProgram(ProgramId);
}

glm::mat4 TranslThePoint(const glm::vec3& vec, const int& type = 0) { 
	switch (type) {
	case 0:
		return glm::translate(glm::mat4(1.0f), -vec);
		break;

	case 1:
		return glm::translate(glm::mat4(1.0f), vec);
		break;
	
	default:
		std::cout << "[WARNING] Translatia a intrat pe default case\n";
		return C_IDENTITY_MATRIX;
	}
}

void Initialize(void) {

	resizeMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.f / C_WINDOW_WIDTH, 1.f / C_WINDOW_HEIGHT, 1.0));

	// Calculate the position for the first piece  
	domino.x = (2 * C_WINDOW_WIDTH - (numOfPieces * C_SPACE_BETWEEN_PIECES)) / 2 - C_WINDOW_WIDTH;

	suportPendulYPos = domino.y + C_SUPORT_PENDUL_Y_COORD_OFFSET;
	suportPendulXPos = domino.x - C_SUPORT_PENDUL_X_COORD_OFFSET;
	pendulRotatePoint = glm::vec3(suportPendulXPos, suportPendulYPos, 0.0F);

	//table.x = domino.x - C_TABLE_OFFSET;
	// table.width = (numOfPieces - 1) * C_SPACE_BETWEEN_PIECES + (2 * C_TABLE_OFFSET) + (domino.height + domino.width);
	table.x = -C_WINDOW_WIDTH;
	table.y = domino.y - C_TABLE_WIDTH;
	table.height = C_TABLE_WIDTH;
	table.width = 2 * C_WINDOW_WIDTH;


	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	CreateVBO();
	CreateShaders();
}

template <class T>
void LoadUniformVar(const GLuint& programId, const std::string& name, T value) {

	GLuint codColLocation = glGetUniformLocation(programId, name.c_str());
	if (codColLocation == GL_FALSE) {
		std::cout << "[ERROR] Fail to get the location!\n";
		return;
	}
	glUniform1i(codColLocation, value);
}


// Wrapper for this operation 
void Load4x4MatrixToVertShader(const std::string& iName, const glm::mat4& iMatrix) {
	GLuint myMatrixLocation = glGetUniformLocation(ProgramId, iName.c_str());
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &iMatrix[0][0]);
}

void RenderFunction(void) {
	pieceRotatePoint = glm::vec3(domino.x + domino.width, domino.y, 0.0F);
	trans = C_IDENTITY_MATRIX;
	glClear(GL_COLOR_BUFFER_BIT);

	LoadUniformVar<int>(ProgramId, "windowHeight", C_WINDOW_HEIGHT);
	LoadUniformVar<int>(ProgramId, "windowWidth", C_WINDOW_WIDTH);

	Load4x4MatrixToVertShader("resizeMatrix", resizeMatrix);
	Load4x4MatrixToVertShader("myMatrix", C_IDENTITY_MATRIX);


	LoadUniformVar<int>(ProgramId, "codCol", Gradient);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 3, 3);

	// Draw table
	LoadUniformVar<int>(ProgramId, "codCol", Black);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	// Draw pendul 
	if (pendulSize > 0)
	{
		if (pendulAngle) {
			LoadUniformVar<int>(ProgramId, "codCol", White);
		}
		else {
			LoadUniformVar<int>(ProgramId, "codCol", Black);
		}
		matrRot = glm::rotate(glm::mat4(1.0f), glm::radians(pendulAngle), glm::vec3(0.0, 0.0, 1.0));
		Load4x4MatrixToVertShader("myMatrix", TranslThePoint(pendulRotatePoint, 1) * matrRot * TranslThePoint(pendulRotatePoint, 0));
		glEnable(GL_POINT_SMOOTH);
		glPointSize(pendulSize);
		glDrawArrays(GL_POINTS, 15, 1);
	}

	// Draw domino pieces
	for (int i = 0; i < numOfPieces; i++) {
		if (angles[i] == 0) {
			LoadUniformVar<int>(ProgramId, "codCol", Blue);
		}
		else {
			LoadUniformVar<int>(ProgramId, "codCol", Red);
		}
		if (i == 0) {
			matrRot = glm::rotate(glm::mat4(1.0f), glm::radians(-angles[i]), glm::vec3(0.0, 0.0, 1.0));
			Load4x4MatrixToVertShader("myMatrix", TranslThePoint(pieceRotatePoint, 1) * matrRot * TranslThePoint(pieceRotatePoint, 0));
			glDrawArrays(GL_TRIANGLE_FAN, 10, 4);

		}
		else {
			pieceRotatePoint += glm::vec3(C_SPACE_BETWEEN_PIECES, 0.0F, 0.0F);
			matrRot = glm::rotate(glm::mat4(1.0f), glm::radians(-angles[i]), glm::vec3(0.0, 0.0, 1.0));
			trans *= glm::translate(glm::mat4(1.0f), glm::vec3(C_SPACE_BETWEEN_PIECES, 0.0F, 0.0F));
			Load4x4MatrixToVertShader("myMatrix", TranslThePoint(pieceRotatePoint, 1) * matrRot * TranslThePoint(pieceRotatePoint, 0) * trans);
			glDrawArrays(GL_TRIANGLE_FAN, 10, 4);
		}
	}
	glutPostRedisplay();
	glFlush();
}

void Cleanup(void) {
	DestroyShaders();
	DestroyVBO();
}

void MouseAction(int button, int state, int x, int y) {
	if (GLUT_LEFT_BUTTON == button && GLUT_DOWN == state)
	{
		startPendul = true;
	}
}

void AnglesUpdate(int input) {
	glutPostRedisplay();
	for (int i = 0; i < numOfPieces; i++) {
		if (
			(true == startDomino && 0 == i && angles[i] < C_STOP_ROTATION_ANGLE) ||       // for first piece 
			(numOfPieces - 1 == i && numOfPieces > 1 && angles[i - 1] > C_ANGLE_TO_ACTIVATE_NEXT_PIECE
									&& angles[i] < C_STOP_ROTATION_ANGLE_LAST_PIECE) ||	 // for last piece
			(startDomino == 1 && numOfPieces == 1 && angles[i] < C_STOP_ROTATION_ANGLE_LAST_PIECE) ||				 // edge case, when it's a single piece 
			(angles[i - 1] > C_ANGLE_TO_ACTIVATE_NEXT_PIECE && angles[i] < C_STOP_ROTATION_ANGLE)) {          // for the others pieces 
			angles[i] += C_DOMINO_ANGLE_OFFSET;
		}
	}
	if (startDomino == true && pendulSize > 0) {
		pendulSize -= C_PENDUL_RESIZE;
	}
	glutTimerFunc(C_REFRESH_TIME, AnglesUpdate, 0);
}
 
void PendulUpdate (int input) {
	if (startPendul == true && pendulAngle < C_STOP_ROTATION_ANGLE_LAST_PIECE) {
		pendulAngle += C_PENDUL_ANGLE_OFFSET;
	} 
	if (pendulAngle == C_PENDUL_ANGLE_STOP) {
		startDomino = true;
		glutTimerFunc(C_REFRESH_TIME, AnglesUpdate, 0);
		return;
	}
	glutPostRedisplay();
	glutTimerFunc(C_REFRESH_TIME, PendulUpdate, 0);

}