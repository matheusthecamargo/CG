#include <Windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include "Shader.h"
#include "stb_image.h"
#include <stdio.h>
#include <GL/glew.h> /* include GLEW and new version of GL on Windows */
#include <GLFW/glfw3.h> /* GLFW helper library */
#include <GLM/glm.hpp>
#include <GLM/vec2.hpp>
#include <GLM/vec3.hpp>
#include <GLM/vec4.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <Soil.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void conversao(double &x, double &y);
void gerarCurva();

// settings
const unsigned int SCR_WIDTH = 860;
const unsigned int SCR_HEIGHT = 640;
boolean des = false;
boolean fim = false;
vector<GLfloat>* vert = new vector<GLfloat>();
vector<GLfloat>* curva = new vector<GLfloat>();
vector<GLfloat>* curvaint = new vector<GLfloat>();
vector<GLfloat>* curvaext = new vector<GLfloat>();
unsigned int VBO1, VAO1, VBO2, VAO2, VAO3, VBO3, VAO4, VBO4;

int main()
{
	
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	glfwSetMouseButtonCallback(window, mouse_button_callback);

	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const char* vertexShaderSource =
		"#version 410 core\n"
		"layout(location = 0) in vec3 apos;"
		"layout(location = 1) in vec3 acolor;"
		"out vec3 color;"
		"uniform mat4 ProjectionMatrix;"
		"void main() {"
		"   color = acolor;"
		"	gl_Position = ProjectionMatrix * vec4(apos, 1.0);"
		"}";

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint sucess;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &sucess);
	if (!sucess) {
		cout << "Error Vertex";
	}

	const char* fragmentShaderSource =
		"#version 410 core\n"
		"in vec3 color;"
		"out vec4 fragColor;"
		"void main()	{"
		"   fragColor = vec4(color, 1.0f);"	//saida do resultado
		"}";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &sucess);
	if (!sucess) {
		cout << "Error Fragment";
	}

	int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	glGenVertexArrays(1, &VAO1);
	glBindVertexArray(VAO1);
	glGenBuffers(1, &VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert->size(), vert->data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	
	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curva->size(), curva->data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

	//glBindVertexArray(0);

	glUseProgram(shaderProgram);
	
	
	glm::mat4 ProjectionMatrix(1.f);

	ProjectionMatrix = glm::ortho(0.0f, 860.0f, 640.0f, 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	

	glUseProgram(shaderProgram);

	glUseProgram(0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);
		//processInput(window, position, rotation, scale);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render the triangle
		//ourShader->use();
		glUseProgram(shaderProgram);

				
		if (des) {
			glBindVertexArray(VAO1);
			glPointSize(15);
			glDrawArrays(GL_POINTS, 0, vert->size()/6);
		}
		if (fim) {
			//cout << "teste";
			glBindVertexArray(VAO2);
			glDrawArrays(GL_LINE_STRIP, 0, curva->size()/6);
		}
		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
		//glBindVertexArray(0);
		//glUseProgram(0);
		
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	//int i;
	cout << "teste";
	//cin >> i;
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void conversao(double &x, double &y) {
	if (x > (SCR_WIDTH / 2)) {
		x = x - (SCR_WIDTH / 2);
		x = x / (SCR_WIDTH / 2);
		x = x + (x*0.35);
	}
	else if (x == (SCR_WIDTH / 2)) {
		x = 0;
	}
	else {
		x = (-(((SCR_WIDTH / 2) - x) / (SCR_WIDTH / 2)));
		x = x+(x*0.35);
	}

	if (y > (SCR_HEIGHT / 2)) {
		y = y - (SCR_HEIGHT / 2);
		y = y / (SCR_HEIGHT / 2);
		y = y * (-1);
	}
	else if (y == (SCR_HEIGHT / 2)) {
		y = 0;
	}
	else {
		y = -(((SCR_HEIGHT / 2) - y) / (SCR_HEIGHT / 2));
		y = y * (-1);
	}
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		//cout << xpos << " " << ypos << "\n";
		
		//conversao(xpos, ypos);
		
		GLfloat x = xpos;
		GLfloat y = ypos;
		
		//cout << x << " " << y;

		vert->push_back(x);
		vert->push_back(y);
		vert->push_back(1.0);

		vert->push_back(1.0f);
		vert->push_back(1.0f);
		vert->push_back(1.0f);
		
		des = true;

		if (vert->size()/6 >= 4) {
			fim = true;
			gerarCurva();
		}
		
		glBindVertexArray(VAO1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert->size(), vert->data(), GL_STATIC_DRAW);

	}

}

void gerarCurva() {
	vector<GLfloat>* aux = new vector<GLfloat>();
	for (int i = 0; i < vert->size()/6; i++) {
		aux->push_back(vert->at(6 * i + 0));
		aux->push_back(vert->at(6 * i + 1));
		aux->push_back(vert->at(6 * i + 2));
		//cout << " "<<i << "/";
	}
	curva->clear();
	aux->push_back(vert->at(0));
	aux->push_back(vert->at(1));
	aux->push_back(vert->at(2));
	aux->push_back(vert->at(6));
	aux->push_back(vert->at(7));
	aux->push_back(vert->at(8));

	aux->push_back(vert->at(12));
	aux->push_back(vert->at(13));
	aux->push_back(vert->at(14));
	
	for (int i = 0; i < (aux->size()/3 - 3); i++)  {
		//cout << " " << i << "/";
		for (GLfloat t = 0; t <= 1.0; t += 0.05f) {
			GLfloat x = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*aux->at(3 * i) +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*aux->at(3 * (i + 1)) +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*aux->at(3 * (i + 2)) +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*aux->at(3 * (i + 3))) / 6);

			GLfloat y = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*aux->at((3 * i)+1) +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*aux->at((3 * (i + 1))+1) +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*aux->at((3 * (i + 2))+1) +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*aux->at((3 * (i + 3)))+1) / 6);

			cout << x << " " << y << "\n"; 
			curva->push_back(x);
			curva->push_back(y);
			curva->push_back(1.0); 
			curva->push_back(1.0);
			curva->push_back(1.0);
			curva->push_back(1.0);
		}
		
	}
	curva->push_back(curva->at(0));
	curva->push_back(curva->at(1));
	curva->push_back(1.0);
	curva->push_back(1.0);
	curva->push_back(1.0);
	curva->push_back(1.0);
	cout << aux->size() << "/";
	cout << curva->size();
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curva->size(), curva->data(), GL_STATIC_DRAW);

	
}

