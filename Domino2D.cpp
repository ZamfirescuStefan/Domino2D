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

// Apps parameters
const int C_NUM_MAX_PIECES = 15;
const int C_WIDTH = 800;
const int C_HEIGHT = 600;
const int C_REFRESH_TIME = 25;
const float C_ANGLE_OFFSET = 5.0F;
const float C_STOP_ROTATION_ANGLE = 75.0F;
int num_of_pieces;
//////////////////////////////////////

GLuint
	VaoId,
	VboId,
	ColorBufferId,
	ProgramId;

glm::mat4 resizeMatrix, matrRot, trans;
glm::vec3 scalePoint = glm::vec3(55.0f, 45.0f, 0.0f);
glm::vec3 rotatePoint;

float angles[20];
float angle1 = 0.0F;
float angle2 = 0.0F;
float angle3 = 0.0F;
bool start = false;

void Initialize();

void RenderFunction();

void MouseAction(int, int, int, int);

void Cleanup();

void AnglesUpdate(int value);

bool ValidInput(const std::string& input) {

	for (const char& chr: input) {
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

int main(int argc, char* argv[]) {
	
	std::string input;
	std::cout << "Introduceti numarul de piese: (15 piese maxim)\n";
	std::cin >> input;
	while (!ValidInput(input)) {
		std::cout << "Numarul introdus este invalid, incercati din nou\n";
		std::cin >> input;
	}
	num_of_pieces = std::stoi(input);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Domino 2D");
	glutDisplayFunc(RenderFunction);
	glutMouseFunc(MouseAction);
	glewInit();
	Initialize();
    glutTimerFunc(0, AnglesUpdate, 0);
	glutMainLoop();
	glutCloseFunc(Cleanup);

}

void CreateVBO(void) {
	GLfloat Vertices[] = {
		// background  
	   -1600.0f, -1200.0f, 0.0f, 1.0f,
		1600.0f, 1200.0f, 0.0f, 1.0f,
		1600.0f, -1200.0f, 0.0f, 1.0f,
		//
	   -1600.0f, -1200.0f, 0.0f, 1.0f,
		1600.0f, 1200.0f, 0.0f, 1.0f,
	   -1600.0f, 1200.0f, 0.0f, 1.0f,
	  // varfuri pentru axe
	   -1600.0f, 0.0f, 0.0f, 1.0f,
	   1600.0f,  0.0f, 0.0f, 1.0f,
	   0.0f, -1200.0f, 0.0f, 1.0f,
	   0.0f, 1200.0f, 0.0f, 1.0f,
	   // Prima piesa de domino
	   -200.0F, -100.0F, 0.0F, 1.0F,
	   -180.0F, -100.0F, 0.0F, 1.0F,
	   -180.0F,  20.0F, 0.0F, 1.0F,
	   -200.0F,  20.0F, 0.0F, 1.0F,
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
	{
		return glm::translate(glm::mat4(1.0f), -vec);
		break;
	}
	case 1:
	{
		return glm::translate(glm::mat4(1.0f), vec);
		break;
	}
	default:
		return glm::mat4(1);
		std::cout << "[INFO] Translatia a intrat pe default case\n";
		break;

	}
}

void Initialize(void) {
	resizeMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.f / C_WIDTH, 1.f / C_HEIGHT, 1.0));

	// pentru ex 3 
	matrRot = glm::rotate(glm::mat4(1.0f), -C_PI / 24, glm::vec3(0.0, 0.0, 1.0));
	//

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	CreateVBO();
	CreateShaders();
}

template <class T>
void LoadUniformVar(const GLuint& programId, const std::string& name, T value) {
	GLuint codColLocation = glGetUniformLocation(programId, name.c_str());
	if (codColLocation == GL_FALSE) {
		std::cout << "[ERROR] Fail to get the location!\n";
	}
	glUniform1i(codColLocation, value);
}

// Wrapper for this operation 
void Load4x4MatrixToVertShader(const std::string& iName, const glm::mat4& iMatrix) {
	GLuint myMatrixLocation = glGetUniformLocation(ProgramId, iName.c_str());
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &iMatrix[0][0]);
}

void RenderFunction(void) {
	rotatePoint = glm::vec3(-180.0F, -100.0F, 0.0F);
	trans = glm::mat4(1);
	glClear(GL_COLOR_BUFFER_BIT);

	Load4x4MatrixToVertShader("resizeMatrix", resizeMatrix);
	Load4x4MatrixToVertShader("myMatrix", C_IDENTITY_MATRIX);


	LoadUniformVar<int>(ProgramId, "codCol", 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 3, 3);


	LoadUniformVar<int>(ProgramId, "codCol", 1);
	glPointSize(10.0);
	// axes 
	// glDrawArrays(GL_LINES, 6, 4);

	// First domino piece 
	for (int i = 0; i < num_of_pieces; i++) {
		if (i == 0) {
			if (angles[i] == 0) {
				LoadUniformVar<int>(ProgramId, "codCol", 1);
			}
			else {
				LoadUniformVar<int>(ProgramId, "codCol", 2);
			}
			matrRot = glm::rotate(glm::mat4(1.0f), glm::radians(-angles[i]), glm::vec3(0.0, 0.0, 1.0));
			Load4x4MatrixToVertShader("myMatrix", TranslThePoint(rotatePoint, 1) * matrRot * TranslThePoint(rotatePoint, 0));
			glDrawArrays(GL_TRIANGLE_FAN, 10, 4);

		}
		else {
			if (angles[i] == 0) {
				LoadUniformVar<int>(ProgramId, "codCol", 1);
			}
			else {
				LoadUniformVar<int>(ProgramId, "codCol", 2);
			}

			rotatePoint += glm::vec3(80.0F, 0.0F, 0.0F);
			matrRot = glm::rotate(glm::mat4(1.0f), glm::radians(-angles[i]), glm::vec3(0.0, 0.0, 1.0));
			trans *= glm::translate(glm::mat4(1.0f), glm::vec3(80.0F, 0.0F, 0.0F));
			Load4x4MatrixToVertShader("myMatrix", TranslThePoint(rotatePoint, 1) * matrRot * TranslThePoint(rotatePoint, 0) * trans);
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
		start = true;
		std::cout << "Left click pressed\n";
	}
}

void AnglesUpdate(int value) {
	glutPostRedisplay();
	for (int i = 0; i < num_of_pieces; i++) {
		if (
			(1 == start && 0 == i && angles[i] < C_STOP_ROTATION_ANGLE) ||       // for first piece 
			(num_of_pieces - 1 == i && num_of_pieces > 1 && angles[i - 1] > 30 && angles[i] < 90) ||	 // for last piece
			(start == 1 && num_of_pieces == 1 && angles[i] < 90) ||				 // edge case, when it's a single piece 
			(angles[i - 1] > 30 && angles[i] < C_STOP_ROTATION_ANGLE))           // for the others pieces 
		{
			angles[i] += C_ANGLE_OFFSET;
		}
	}

	glutTimerFunc(C_REFRESH_TIME, AnglesUpdate, 0);
}
