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
void gerarcurvas(boolean interna);
void gerarCurva();
int desenhofinal();
void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
void movimentar(vector<float> * vert1, int contador);


// settings
const unsigned int SCR_WIDTH = 860;
const unsigned int SCR_HEIGHT = 640;
boolean des = false;
boolean fim = false;
boolean sel = false;
vector<GLfloat>* vert = new vector<GLfloat>();
vector<GLfloat>* curva = new vector<GLfloat>();
vector<GLfloat>* curvaint = new vector<GLfloat>();
vector<GLfloat>* curvaext = new vector<GLfloat>();
unsigned int VBO1, VAO1, VBO2, VAO2, VAO3, VBO3, VAO4, VBO4;
vector<GLfloat>* aux = new vector<GLfloat>();
GLfloat r=1.0, g=1.0, b=1.0;
Mesh * mesh = new Mesh();

vector<GLfloat>* vert2 = new vector<GLfloat>();

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	glGenVertexArrays(1, &VAO3);
	glBindVertexArray(VAO3);
	glGenBuffers(1, &VBO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curvaint->size(), curvaint->data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

	glGenVertexArrays(1, &VAO4);
	glBindVertexArray(VAO4);
	glGenBuffers(1, &VBO4);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curvaext->size(), curvaext->data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glUseProgram(shaderProgram);
	
	
	glm::mat4 ProjectionMatrix(1.f);

	ProjectionMatrix = glm::ortho(0.0f, 860.0f, 640.0f, 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	

	glUseProgram(shaderProgram);

	glUseProgram(0);


	while (!glfwWindowShouldClose(window))
	{

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(shaderProgram);

				
		if (des) {
			glBindVertexArray(VAO1);
			glPointSize(15);
			glDrawArrays(GL_POINTS, 0, vert->size()/6);
		}
		if (fim) {
			//cout << "teste";
			glBindVertexArray(VAO2);
			glLineWidth(15);
			glDrawArrays(GL_LINE_STRIP, 0, curva->size()/6);
			glBindVertexArray(VAO3);
			glDrawArrays(GL_LINE_STRIP, 0, curva->size() / 6);
			glBindVertexArray(VAO4);
			glDrawArrays(GL_LINE_STRIP, 0, curva->size() / 6);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		glBindVertexArray(0);
		glUseProgram(0);
		
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
	for (int cont = 0; cont < curvaint->size() / 6; cont++) {
		mesh->addV(glm::vec3(curvaint->at(cont * 6), curvaint->at((cont * 6)+1), curvaint->at((cont * 6)+2)));
	}
	for (int cont = 0; cont < curvaext->size() / 6; cont++) {
		mesh->addV(glm::vec3(curvaext->at(cont * 6), curvaext->at((cont * 6) + 1), curvaext->at((cont * 6) + 2)));
	}
	mesh->addT(glm::vec2(0.0, 1.0));
	mesh->addT(glm::vec2(1.0, 1.0));
	mesh->addT(glm::vec2(1.0, 0.0));
	mesh->addT(glm::vec2(0.0, 0.0));
	mesh->addN(glm::vec3(0.0, 0.0, 1.0));
	mesh->setnomematerial("pista.mtl");
	Group * group = new Group();
	group->setNome("Direto");
	group->setId(0);
	Face * temp;
	for (int cont = 0; cont < (curvaext->size() / 6); cont++) {
		temp = new Face();
		temp->addV(cont * 4);
		temp->addV(cont * 4 + 1);
		temp->addV(cont * 4 + 3);
		group->addFaces(temp);
		temp = new Face();
		temp->addV(cont * 4 + 1);
		temp->addV(cont * 4 + 2);
		temp->addV(cont * 4 + 3);
		group->addFaces(temp);
	}

	mesh->addGrupo(group);
	ObjWriter * write = new ObjWriter();
	write->arquivo(mesh, curva);

	desenhofinal();
	/*int iç;
	cout << curvaint->size()/6;
	cin >> iç;*/
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (r > 0 && r <= 1.0) {
			r = r - 0.02;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		if (g > 0 && g <= 1.0) {
			g = g - 0.02;
		}
	}if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		if (b > 0 && b <= 1.0) {
			b = b - 0.02;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (r >= 0 && r < 1.0) {
			r = r + 0.02;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		if (g >= 0 && g < 1.0) {
			g = g + 0.02;
		}
	}if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		if (b >= 0 && b < 1.0) {
			b = b + 0.02;
		}
	}if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		sel = true;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void gerarcurvas(boolean interna) {
	//cout << curva->size() / 6 << " ";
	if (interna) {
		curvaint->clear();
		for (int k = 0; k < (curva->size() /6); k++) {
			//cout << k << " ";
			GLfloat temp1 = curva->at(k*6);
			GLfloat temp2 = curva->at((k*6)+1);
			GLfloat temp3;
			GLfloat temp4;
			if (k == (curva->size()/6) - 1) {
				temp3 = curva->at(0);
				temp4 = curva->at(0+1);
			}
			else {
				temp3 = curva->at(((k+1)*6));
				temp4 = curva->at(((k + 1) * 6) + 1);
			}
			GLfloat dx = temp3 - temp1;
			GLfloat dy = temp4 - temp2;

			if (dx == 0 || dy == 0) {
				dx = temp3 - curva->at(((k - 1) * 6));
				dy = temp4 - curva->at(((k - 1) * 6) + 1);
			}

			GLfloat angulo = glm::atan(dy, dx);
			
			angulo = angulo - (3.14159265359 / 2.0);

			GLfloat sX = glm::cos(angulo) * 20;
			GLfloat sY = glm::sin(angulo) * 20;

			//cout << sX << " " << sY << " " << curva->at(k*6 + 0) << " " << curva->at(k*6 + 1) << "\n";


			curvaint->push_back(curva->at(k * 6 + 0)+sX);
			curvaint->push_back(curva->at(k * 6 + 1)+sY);
			curvaint->push_back(curva->at(k*6 + 2));
			curvaint->push_back(1.0f);
			curvaint->push_back(0.0f);
			curvaint->push_back(0.0f);
			//cout << curvaint->at(k * 6 + 0) << " " << curvaint->at(k * 6 + 1) << " " << curva->at(k * 6 + 0) << " " << curva->at(k * 6 + 1) << "\n";
			
		}

		curvaint->push_back(curvaint->at(0));
		curvaint->push_back(curvaint->at(1));
		curvaint->push_back(curvaint->at(2));
		curvaint->push_back(1.0f);
		curvaint->push_back(0.0f);
		curvaint->push_back(0.0f);

		glBindVertexArray(VAO3);
		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curvaint->size(), curvaint->data(), GL_STATIC_DRAW);
	}
	else {
		curvaext->clear();
		for (int k = 0; k < (curva->size() / 6); k++) {
			//cout << k << " ";
			GLfloat temp1 = curva->at(k * 6);
			GLfloat temp2 = curva->at((k * 6) + 1);
			GLfloat temp3;
			GLfloat temp4;
			if (k == (curva->size() / 6) - 1) {
				temp3 = curva->at(0);
				temp4 = curva->at(0 + 1);
			}
			else {
				temp3 = curva->at(((k + 1) * 6));
				temp4 = curva->at(((k + 1) * 6) + 1);
			}
			GLfloat dx = temp3 - temp1;
			GLfloat dy = temp4 - temp2;

			if (dx == 0 || dy == 0) {
				dx = temp3 - curva->at(((k - 1) * 6));
				dy = temp4 - curva->at(((k - 1) * 6) + 1);
			}

			GLfloat angulo = glm::atan(dy, dx);

			angulo = angulo + (3.14159265359 / 2.0);

			GLfloat sX = glm::cos(angulo) * 20;
			GLfloat sY = glm::sin(angulo) * 20;

			//cout << sX << " " << sY << " " << curva->at(k*6 + 0) << " " << curva->at(k*6 + 1) << "\n";


			curvaext->push_back(curva->at(k * 6 + 0) + sX);
			curvaext->push_back(curva->at(k * 6 + 1) + sY);
			curvaext->push_back(curva->at(k * 6 + 2));
			curvaext->push_back(0.0f);
			curvaext->push_back(0.0f);
			curvaext->push_back(1.0f);
			//cout << curvaint->at(k * 6 + 0) << " " << curvaint->at(k * 6 + 1) << " " << curva->at(k * 6 + 0) << " " << curva->at(k * 6 + 1) << "\n";

		}

		curvaext->push_back(curvaext->at(0));
		curvaext->push_back(curvaext->at(1));
		curvaext->push_back(curvaext->at(2));
		curvaext->push_back(0.0f);
		curvaext->push_back(0.0f);
		curvaext->push_back(1.0f);

		glBindVertexArray(VAO4);
		glBindBuffer(GL_ARRAY_BUFFER, VBO4);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curvaext->size(), curvaext->data(), GL_STATIC_DRAW);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (sel) {
			if (vert->size() > 0) {
				vector<GLfloat>* pas = new vector<GLfloat>();
				for (int i = 0; i < vert->size(); i++) {
					pas->push_back(vert->at(i));
				}
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);
				GLfloat x = xpos;
				GLfloat y = ypos;
				int posição = 0;
				float distancia = sqrt(pow(x - vert->at(0), 2) + pow(y - vert->at(1), 2));
				for (int i = 1; i < vert->size() / 6; i++) {
					double dis = sqrt(pow(x - vert->at(i * 6), 2) + pow(y - vert->at(i * 6 + 1), 2));
					if (dis < distancia) {
						distancia = dis;
						posição = i;
					}
				}
				vert->clear();
				posição = posição * 6;
				for (int i = 0; i < pas->size(); i++) {
					if (posição == i) {
						vert->push_back(pas->at(i++));
						vert->push_back(pas->at(i++));
						vert->push_back(pas->at(i++));
						vert->push_back(r);
						i++;
						vert->push_back(g);
						i++;
						vert->push_back(b);
					}
					else {
						vert->push_back(pas->at(i));
					}					
				}
				gerarCurva();
			}
			
			sel = false;
		}
		else {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			//cout << xpos << " " << ypos << "\n";

			GLfloat x = xpos;
			GLfloat y = ypos;

			//cout << x << " " << y;

			vert->push_back(x);
			vert->push_back(y);
			vert->push_back(0.0);

			vert->push_back(r);
			vert->push_back(g);
			vert->push_back(b);

			des = true;

			if (vert->size() / 6 >= 4) {
				fim = true;
				gerarCurva();
				//cout << "tete";
				gerarcurvas(true);
				gerarcurvas(false);
				//cout << curvaext->size() << " " << curvaint->size();
			}

		
		}
		
		glBindVertexArray(VAO1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert->size(), vert->data(), GL_STATIC_DRAW);

	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		//cout << "teste";
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		cout << xpos << " " << ypos;
	}
}

void gerarCurva() {
	aux->clear();
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
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*aux->at(3 * (i + 3))) / 6.0);

			GLfloat y = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*aux->at((3 * i)+1) +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*aux->at((3 * (i + 1))+1) +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*aux->at((3 * (i + 2))+1) +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*aux->at((3 * (i + 3))+1)) / 6.0);

			//cout << x << " " << y << "\n"; 
			curva->push_back(x);
			curva->push_back(y);
			curva->push_back(vert->at(6 * i + 2));
			curva->push_back(vert->at(6 * i + 3));
			curva->push_back(vert->at(6 * i + 4));
			curva->push_back(vert->at(6 * i + 5));
		}
		cout << " ";
		
	}
	curva->push_back(curva->at(0));
	curva->push_back(curva->at(1));
	curva->push_back(curva->at(2));
	curva->push_back(curva->at(3));
	curva->push_back(curva->at(4));
	curva->push_back(curva->at(5));
	//cout << aux->size() << "/";
	//cout << curva->size();
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curva->size(), curva->data(), GL_STATIC_DRAW);
}

int desenhofinal() {
	ObjReader * obj = new ObjReader();
	Mesh * malha = obj->read("teste2.obj");

	vector<GLfloat>* vert = new vector<GLfloat>();
	vector<GLint>* indi = new vector<GLint>();

	
	vector<GLint>* indi2 = new vector<GLint>();

	indi2->push_back(0);
	indi2->push_back(1);
	indi2->push_back(3);
	indi2->push_back(1);
	indi2->push_back(2);
	indi2->push_back(3);

	for (int k = 0; k < malha->getGroup(0)->getFaces().size(); k++) {
		indi->push_back(malha->getGroup(0)->getFace(k)->getV(0));
		indi->push_back(malha->getGroup(0)->getFace(k)->getV(1));
		indi->push_back(malha->getGroup(0)->getFace(k)->getV(2));
	}

	for (int k = 0; k < malha->getVector().size() / 2; k++) {
		vert->push_back(malha->getIndV(k).x / 860.0);
		vert->push_back(malha->getIndV(k).y / 640.0);
		vert->push_back(malha->getIndV(k).z);
		vert->push_back(malha->getIndT(0).x);
		vert->push_back(malha->getIndT(0).y);
		vert->push_back(malha->getIndN(0).x);
		vert->push_back(malha->getIndN(0).y);
		vert->push_back(malha->getIndN(0).z);

		vert->push_back(malha->getIndV(k + 1).x / 860.0);
		vert->push_back(malha->getIndV(k + 1).y / 640.0);
		vert->push_back(malha->getIndV(k + 1).z);
		vert->push_back(malha->getIndT(1).x);
		vert->push_back(malha->getIndT(1).y);
		vert->push_back(malha->getIndN(0).x);
		vert->push_back(malha->getIndN(0).y);
		vert->push_back(malha->getIndN(0).z);

		vert->push_back(malha->getIndV((k + (malha->getVector().size() / 2) + 1) % malha->getVector().size()).x / 860.0);
		vert->push_back(malha->getIndV((k + (malha->getVector().size() / 2) + 1) % malha->getVector().size()).y / 640.0);
		vert->push_back(malha->getIndV((k + (malha->getVector().size() / 2) + 1) % malha->getVector().size()).z);
		vert->push_back(malha->getIndT(2).x);
		vert->push_back(malha->getIndT(2).y);
		vert->push_back(malha->getIndN(0).x);
		vert->push_back(malha->getIndN(0).y);
		vert->push_back(malha->getIndN(0).z);

		vert->push_back(malha->getIndV(k + (malha->getVector().size() / 2)).x / 860.0);
		vert->push_back(malha->getIndV(k + (malha->getVector().size() / 2)).y / 640.0);
		vert->push_back(malha->getIndV(k + (malha->getVector().size() / 2)).z);
		vert->push_back(malha->getIndT(3).x);
		vert->push_back(malha->getIndT(3).y);
		vert->push_back(malha->getIndN(0).x);
		vert->push_back(malha->getIndN(0).y);
		vert->push_back(malha->getIndN(0).z);
	}

	vert2->push_back(vert->at(0));
	vert2->push_back(vert->at(1));
	vert2->push_back(0.01);
	vert2->push_back(1.0);
	vert2->push_back(1.0);
	vert2->push_back(malha->getIndN(0).x);
	vert2->push_back(malha->getIndN(0).y);
	vert2->push_back(malha->getIndN(0).z);

	vert2->push_back(vert->at(8));
	vert2->push_back(vert->at(9));
	vert2->push_back(0.01);
	vert2->push_back(1.0);
	vert2->push_back(0.0);
	vert2->push_back(malha->getIndN(0).x);
	vert2->push_back(malha->getIndN(0).y);
	vert2->push_back(malha->getIndN(0).z);

	vert2->push_back(vert->at(16));
	vert2->push_back(vert->at(17));
	vert2->push_back(0.01);
	vert2->push_back(0.0);
	vert2->push_back(0.0);
	vert2->push_back(malha->getIndN(0).x);
	vert2->push_back(malha->getIndN(0).y);
	vert2->push_back(malha->getIndN(0).z);

	vert2->push_back(vert->at(24));
	vert2->push_back(vert->at(25));
	vert2->push_back(0.01);
	vert2->push_back(0.0);
	vert2->push_back(1.0);
	vert2->push_back(malha->getIndN(0).x);
	vert2->push_back(malha->getIndN(0).y);
	vert2->push_back(malha->getIndN(0).z);

	vector<Material*> materiais;
	obj->readermaterial(malha->getnomematerial(), materiais);

	

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const char* vertexShaderSource =
		"#version 410 core\n"
		"layout(location = 0) in vec3 aPos;"
		"layout (location = 1) in vec2 aTexCoord;"
		"layout (location = 2) in vec3 aNormal;"
		"out vec3 ourPos;"
		"out vec2 TexCoord;"
		"out vec3 ourNormal;"
		"uniform mat4 ModelMatrix;"
		"uniform mat4 ViewMatrix;"
		"uniform mat4 ProjectionMatrix;"
		"void main() {"
		"   ourPos = vec4(ModelMatrix * vec4(aPos, 1.f)).xyz;"
		"   TexCoord = vec2(aTexCoord.x, aTexCoord.y * -1.0f);"
		"   ourNormal = mat3(ModelMatrix) * aNormal;"
		"   gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.f);"
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
		"in vec3 ourPos;"
		"in vec2 TexCoord;"
		"in vec3 ourNormal;"
		"out vec4 FragColor;"
		"uniform sampler2D texture1;"
		"uniform sampler2D texture2;"
		"uniform vec3 lightPos0;"
		"uniform vec3 cameraPos;"
		"uniform vec3 kambiente;"
		"uniform vec3 kdifusao;"
		"uniform vec3 kespecular;"
		"uniform vec3 shiny;"
		"void main()	{"
		"   vec3 ambientLight = kambiente;"			//ambiente
		"   vec3 posToLightDirVec = normalize(lightPos0 - ourPos);"	//difusa
		"   vec3 diffuseColor = kdifusao;"
		"   float diffuse = clamp(dot(posToLightDirVec, ourNormal), 0, 1);"
		"   vec3 diffuseFinal = diffuseColor * diffuse;"
		"   vec3 lightToPosDirVec = normalize(ourPos - lightPos0);"		//especular
		"   vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(ourNormal)));"
		"   vec3 posToViewDirVec = normalize(cameraPos - ourPos );"
		"   float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), shiny.x);"
		"   vec3 specularFinal = kespecular * specularConstant;"
		"   FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * (vec4(ambientLight, 1.f) + vec4(diffuseFinal, 1.0f) + vec4(specularFinal, 1.f));"	//saida do resultado
		"}";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert->size(), vert->data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint)*indi->size(), indi->data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


	unsigned int VBO7, VAO7, EBO7;
	glGenVertexArrays(1, &VAO7);
	glGenBuffers(1, &VBO7);
	glGenBuffers(1, &EBO7);

	glBindVertexArray(VAO7);

	glBindBuffer(GL_ARRAY_BUFFER, VBO7);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert2->size(), vert2->data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO7);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint)*indi2->size(), indi2->data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


	glBindVertexArray(0);

	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2, texture3;
	int width, height, nrChannels;
	// texture 1
	// ---------
	//unsigned char *data = SOIL_load_image("container.jpg", &width, &height, &nrChannels, SOIL_LOAD_RGBA);

	unsigned char *data = stbi_load(materiais.at(0)->getArquivo().c_str(), &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	data = stbi_load(materiais.at(1)->getArquivo().c_str(), &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// load image, create texture and generate mipmaps

	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	data = stbi_load("carro.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
	// or set it via the texture class
	glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
	//ourShader->setInt("texture2", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glm::vec3 position(0.f);
	glm::vec3 rotation(0.f);
	glm::vec3 scale(1.f);

	glm::mat4 ModelMatrix(1.f);
	ModelMatrix = glm::translate(ModelMatrix, position);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
	ModelMatrix = glm::scale(ModelMatrix, scale);

	glm::vec3 camPosition(0.f, 0.f, 1.f);
	glm::vec3 worldUp(0.f, 1.f, 0.f);
	glm::vec3 camFront(0.f, 0.f, -1.f);

	glm::mat4 ViewMatrix(1.f);
	ViewMatrix = glm::lookAt(camPosition, camPosition + camFront, worldUp);

	float fov = 90.f;
	float nearPlane = 0.1f;
	float farPlane = 1000.f;
	glm::mat4 ProjectionMatrix(1.f);

	ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

	//cout << materiais.at(0)->getKA().x << " " << materiais.at(0)->getKD().x << " " << materiais.at(0)->getKS().x << " " << materiais.at(0)->getShiny().x << "\n";

	glm::vec3 lightPos0(0.0f, 0.f, 1.f);
	glm::vec3 kambiente = materiais.at(0)->getKA();
	glm::vec3 kdifusao = materiais.at(0)->getKD();
	glm::vec3 kespecular = materiais.at(0)->getKS();
	glm::vec3 shiny = materiais.at(0)->getShiny();

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos0"), 1, glm::value_ptr(lightPos0));
	glUniform3fv(glGetUniformLocation(shaderProgram, "kambiente"), 1, glm::value_ptr(kambiente));
	glUniform3fv(glGetUniformLocation(shaderProgram, "kdifusao"), 1, glm::value_ptr(kdifusao));
	glUniform3fv(glGetUniformLocation(shaderProgram, "kespecular"), 1, glm::value_ptr(kespecular));
	glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(camPosition));
	glUniform3fv(glGetUniformLocation(shaderProgram, "shiny"), 1, glm::value_ptr(shiny));

	glUseProgram(0);
	int verif = 0;

	while (!glfwWindowShouldClose(window))
	{
		//cout << vert->size()/32 << " ";
		processInput(window);
		processInput(window, position, rotation, scale);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

		movimentar(vert, verif);
		verif = verif + 1;
		verif = verif % (vert->size()/32);

		glBindVertexArray(VAO7);
		glBindBuffer(GL_ARRAY_BUFFER, VBO7);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert2->size(), vert2->data(), GL_STATIC_DRAW);


		ModelMatrix = glm::mat4(1.f);
		ModelMatrix = glm::translate(ModelMatrix, position);
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
		ModelMatrix = glm::scale(ModelMatrix, scale);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));

		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

		ProjectionMatrix = glm::mat4(1.f);
		ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, malha->getVector().size() * 4, GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3); 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture3);

		glBindVertexArray(VAO7);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
		glBindVertexArray(0);
		glUseProgram(0);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwDestroyWindow(window);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position.z += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position.z -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position.x += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position.x -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		rotation.y -= 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		rotation.y += 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		rotation.x -= 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		rotation.x += 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		scale += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		scale -= 0.1f;
	}
}

void movimentar(vector<float> * vert, int contador) {
	//cout << "teste";
	vert2->clear();
	vert2->push_back(vert->at(contador*32));
	vert2->push_back(vert->at(contador*32+1));
	vert2->push_back(0.01);
	vert2->push_back(1.0);
	vert2->push_back(1.0);
	vert2->push_back(0);
	vert2->push_back(0);
	vert2->push_back(1);

	vert2->push_back(vert->at(contador * 32 + 8));
	vert2->push_back(vert->at(contador * 32 + 9));
	vert2->push_back(0.01);
	vert2->push_back(1.0);
	vert2->push_back(0.0);
	vert2->push_back(0);
	vert2->push_back(0);
	vert2->push_back(1);

	vert2->push_back(vert->at(contador * 32 + 16));
	vert2->push_back(vert->at(contador * 32 + 17));
	vert2->push_back(0.01);
	vert2->push_back(0.0);
	vert2->push_back(0.0);
	vert2->push_back(0);
	vert2->push_back(0);
	vert2->push_back(1);

	vert2->push_back(vert->at(contador * 32 + 24));
	vert2->push_back(vert->at(contador * 32 + 25));
	vert2->push_back(0.01);
	vert2->push_back(0.0);
	vert2->push_back(1.0);
	vert2->push_back(0);
	vert2->push_back(0);
	vert2->push_back(1);
	
}