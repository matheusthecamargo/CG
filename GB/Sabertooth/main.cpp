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
//inicialização da variaveis e vetores globais e assinaturas dos metodos implementados

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void gerarcurvas(boolean interna);
void gerarCurva();
int desenhofinal();
void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);
void movimentar(vector<float> * vert1, int contador);


//tamanho da tela
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
	//inicializaçao da GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif

	//janela na glfw criada
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl; //erro na criação da janela
		glfwTerminate();
		return -1;
	}
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); //view para redimensionar a janela

	glfwMakeContextCurrent(window); //passa as configurações da janela para glfw
	glewExperimental = GL_TRUE;
	glewInit();						//inicializa a glfw

	glfwSetMouseButtonCallback(window, mouse_button_callback);	//metodo do mouse e resposta do comando

	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);	//armazenar o tamanho da janela
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);													//habilita o alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//a configuração do vertexshader
	const char* vertexShaderSource =
		"#version 410 core\n"
		"layout(location = 0) in vec3 apos;"		//entrada do vertex
		"layout(location = 1) in vec3 acolor;"
		"out vec3 color;"			//saida do vertex
		"uniform mat4 ProjectionMatrix;"
		"void main() {"
		"   color = acolor;"
		"	gl_Position = ProjectionMatrix * vec4(apos, 1.0);"
		"}";

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);	//inicializa o vertexshader

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint sucess;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &sucess);
	if (!sucess) {
		cout << "Error Vertex";
	}

	//configuração do fragment shder
	const char* fragmentShaderSource =
		"#version 410 core\n"
		"in vec3 color;" //parametros de entrada
		"out vec4 fragColor;"	//saida do fragment
		"void main()	{"
		"   fragColor = vec4(color, 1.0f);"	//saida do resultado
		"}";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);			//inicializa o fragment
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &sucess);
	if (!sucess) {
		cout << "Error Fragment";
	}

	int shaderProgram;
	shaderProgram = glCreateProgram();					//linka o frag e vertx no shaderprogram

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	glGenVertexArrays(1, &VAO1);					//inicializa o primeiro VAO
	glBindVertexArray(VAO1);						//faz o bind dele
	glGenBuffers(1, &VBO1);							//o vbo inicializa
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);			//bind dele
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert->size(), vert->data(), GL_STATIC_DRAW);	//passa por parametro o vetor dos pontos e das cores
	glEnableVertexAttribArray(0);	//bind dos vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);	//especifica que são 6 floats para representar o ponto, sendo que os 3 primeiros são os floats representando os pontos
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));	//os proximos 3 floats são as cores representadas naquele vertice
	
	glGenVertexArrays(1, &VAO2);		//idem a de cima só que agora com a curva gerada
	glBindVertexArray(VAO2);
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curva->size(), curva->data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

	glGenVertexArrays(1, &VAO3);
	glBindVertexArray(VAO3);		//a interna
	glGenBuffers(1, &VBO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curvaint->size(), curvaint->data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

	glGenVertexArrays(1, &VAO4);		//externa
	glBindVertexArray(VAO4);
	glGenBuffers(1, &VBO4);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curvaext->size(), curvaext->data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));

	glBindVertexArray(0); //bind no vertex
	glUseProgram(shaderProgram); //usar o shader para sinalizar que vai ser passado mais um parametro	
	glm::mat4 ProjectionMatrix(1.f);			//é a projeção da imagem com a tela
	ProjectionMatrix = glm::ortho(0.0f, 860.0f, 640.0f, 0.0f, -1.0f, 1.0f); //o tamanho dessa representação da tela
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix)); //linka com o parametro passado no fragment
	glUseProgram(shaderProgram);
	glUseProgram(0);


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);	//inputs do teclado
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //limpa a tela
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgram);			//utiliza o shader
		if (des) {								//desenha os pontos de controle na tela
			glBindVertexArray(VAO1);
			glPointSize(15);
			glDrawArrays(GL_POINTS, 0, vert->size()/6);
		}
		if (fim) {					//desenha as curvas na tela
			glBindVertexArray(VAO2);
			glLineWidth(15);
			glDrawArrays(GL_LINE_STRIP, 0, curva->size()/6); //curva normal
			glBindVertexArray(VAO3);
			glDrawArrays(GL_LINE_STRIP, 0, curvaint->size() / 6);		//interna
			glBindVertexArray(VAO4);
			glDrawArrays(GL_LINE_STRIP, 0, curvaext->size() / 6);		//externa
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		glBindVertexArray(0);
		glUseProgram(0);
		
	}

	//desaloca da memoria os vaos e vbos
	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteVertexArrays(1, &VAO3);
	glDeleteBuffers(1, &VBO3);
	glDeleteVertexArrays(1, &VAO4);
	glDeleteBuffers(1, &VBO4);

	//encerra a glfw
	glfwTerminate();
	//escrita do obj e mtl da pista
	for (int cont = 0; cont < curvaint->size() / 6; cont++) {		//vertices da interna
		mesh->addV(glm::vec3(curvaint->at(cont * 6), curvaint->at((cont * 6)+1), curvaint->at((cont * 6)+2)));
	}
	for (int cont = 0; cont < curvaext->size() / 6; cont++) {		//vertices da externa
		mesh->addV(glm::vec3(curvaext->at(cont * 6), curvaext->at((cont * 6) + 1), curvaext->at((cont * 6) + 2)));
	}
	mesh->addT(glm::vec2(0.0, 1.0));		//adiciona informações para mesh, já que ela será passada por parametro para escrita nos arquivos
	mesh->addT(glm::vec2(1.0, 1.0));
	mesh->addT(glm::vec2(1.0, 0.0));
	mesh->addT(glm::vec2(0.0, 0.0));
	mesh->addN(glm::vec3(0.0, 0.0, 1.0));
	mesh->setnomematerial("pista.mtl");
	Group * group = new Group();
	group->setNome("Direto");
	group->setId(0);
	Face * temp;
	for (int cont = 0; cont < (curvaext->size() / 6); cont++) {		//faces
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

	mesh->addGrupo(group); //adiciona o grupo na mesh
	ObjWriter * write = new ObjWriter();	//escrita dos arquivos
	write->arquivo(mesh, curva);

	desenhofinal();		//chamada de metodo para mostrar na tela o carro andando
	return 0;
}

void processInput(GLFWwindow *window)	//metodo de eventos no teclado
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);				//se apertar ESC, encerra a GLFW, para a janela em questao
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {		//diminuir o r da "altura" para mudar de cores da curva normal
		if (r > 0 && r <= 1.0) {
			r = r - 0.02;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {		//g
		if (g > 0 && g <= 1.0) {
			g = g - 0.02;
		}
	}if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {	//b
		if (b > 0 && b <= 1.0) {
			b = b - 0.02;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {	//aumentar o rgb agora, r
		if (r >= 0 && r < 1.0) {
			r = r + 0.02;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {	//g
		if (g >= 0 && g < 1.0) {
			g = g + 0.02;
		}
	}if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) { //b
		if (b >= 0 && b < 1.0) {
			b = b + 0.02;
		}
	}if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {	//habilita a seleção do ponto de controle
		sel = true;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)	//metodo para redimensionar a janela do programa
{
	glViewport(0, 0, width, height);
}

void gerarcurvas(boolean interna) {		//metodo para gerar as curvas externa e interna
	if (interna) {	//se for a interna
		curvaint->clear();	//limpa o vetor da interna para recalculo
		for (int k = 0; k < (curva->size() /6); k++) {
			GLfloat temp1 = curva->at(k*6);
			GLfloat temp2 = curva->at((k*6)+1);		//pega os pontos da curva normal para o calculo
			GLfloat temp3;
			GLfloat temp4;
			if (k == (curva->size()/6) - 1) {		//para não invadir uma posição ivalida
				temp3 = curva->at(0);
				temp4 = curva->at(0+1);
			}
			else {
				temp3 = curva->at(((k+1)*6));
				temp4 = curva->at(((k + 1) * 6) + 1);
			}
			GLfloat dx = temp3 - temp1;			//calculo do vetor
			GLfloat dy = temp4 - temp2;

			if (dx == 0 || dy == 0) {
				dx = temp3 - curva->at(((k - 1) * 6));		//se o vetor zerar, pega uma posição antes para o calculo
				dy = temp4 - curva->at(((k - 1) * 6) + 1);
			}

			GLfloat angulo = glm::atan(dy, dx);				//calculo o angulo formado pelo vetor
			
			angulo = angulo - (3.14159265359 / 2.0);		//calcula o ponto apontado

			GLfloat sX = glm::cos(angulo) * 20;				//multiplica pela distancia que queira entre as curvas
			GLfloat sY = glm::sin(angulo) * 20;

			curvaint->push_back(curva->at(k * 6 + 0)+sX);	//adiciona o ponto agora somando mais a distancia que foi calculada
			curvaint->push_back(curva->at(k * 6 + 1)+sY);
			curvaint->push_back(curva->at(k*6 + 2));
			curvaint->push_back(1.0f);
			curvaint->push_back(0.0f);
			curvaint->push_back(0.0f);
		}
		curvaint->push_back(curvaint->at(0));	//para fechar a curva, colocamos novamente a primeira posição para fechar a pista
		curvaint->push_back(curvaint->at(1));
		curvaint->push_back(curvaint->at(2));
		curvaint->push_back(1.0f);
		curvaint->push_back(0.0f);
		curvaint->push_back(0.0f);

		glBindVertexArray(VAO3);				//atualiza o VAO das curvas internas
		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curvaint->size(), curvaint->data(), GL_STATIC_DRAW);
	}
	else {
		curvaext->clear();
		for (int k = 0; k < (curva->size() / 6); k++) {	//mesma logica das internas
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

			angulo = angulo + (3.14159265359 / 2.0);	//so difere no sinal de inves de menos, é +

			GLfloat sX = glm::cos(angulo) * 20;
			GLfloat sY = glm::sin(angulo) * 20;

			curvaext->push_back(curva->at(k * 6 + 0) + sX);
			curvaext->push_back(curva->at(k * 6 + 1) + sY);
			curvaext->push_back(curva->at(k * 6 + 2));
			curvaext->push_back(0.0f);
			curvaext->push_back(0.0f);
			curvaext->push_back(1.0f);
		}

		curvaext->push_back(curvaext->at(0));
		curvaext->push_back(curvaext->at(1));
		curvaext->push_back(curvaext->at(2));
		curvaext->push_back(0.0f);
		curvaext->push_back(0.0f);
		curvaext->push_back(1.0f);

		glBindVertexArray(VAO4);			//atualiza o VAO das externas
		glBindBuffer(GL_ARRAY_BUFFER, VBO4);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curvaext->size(), curvaext->data(), GL_STATIC_DRAW);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {	//metodo para interrupção do mouse
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {	//botao esquerdo do mouse
		if (sel) {										//se seleção está habilitada
			if (vert->size() > 0) {						//verifica se o vetor não está vazio
				vector<GLfloat>* pas = new vector<GLfloat>();	//cria um vetos auxiliar
				for (int i = 0; i < vert->size(); i++) {		//passa tudo para o auxiliar
					pas->push_back(vert->at(i));
				}
				double xpos, ypos;						//cria variaveis para pegar qual posição foi clicada
				glfwGetCursorPos(window, &xpos, &ypos);
				GLfloat x = xpos;			//armazenar em um float
				GLfloat y = ypos;
				int posição = 0;
				float distancia = sqrt(pow(x - vert->at(0), 2) + pow(y - vert->at(1), 2));	//armazenar a distancia do clique do mouse para o primeiro ponto
				for (int i = 1; i < vert->size() / 6; i++) {	//a partir daqui verifica em todos os outros pontos para ver se a distancia é menor com a do clique do mouse
					double dis = sqrt(pow(x - vert->at(i * 6), 2) + pow(y - vert->at(i * 6 + 1), 2));	//armazena a distancia
					if (dis < distancia) {		//se for a menor distancia
						distancia = dis;		//armazena a posição e atualiza a distancia
						posição = i;
					}
				}
				vert->clear();					//limpa o vetor dos pontos de controle
				posição = posição * 6;			//seta para a posição do menor distancia
				for (int i = 0; i < pas->size(); i++) {	//for para preencher novamente o vetor de controle
					if (posição == i) {
						vert->push_back(pas->at(i++));	//só que se for a posição selecionada, atualiza com as cores setada pelo usuario
						vert->push_back(pas->at(i++));
						vert->push_back(pas->at(i++));
						vert->push_back(r);
						i++;
						vert->push_back(g);
						i++;
						vert->push_back(b);
					}
					else {
						vert->push_back(pas->at(i));	//senão, só copia mesmo
					}					
				}
				gerarCurva();	//gera a curva novamente
			}
			
			sel = false;		//e desabilita a seleção
		}
		else {
			double xpos, ypos;		//aqui é para colocar mais pontos de controle
			glfwGetCursorPos(window, &xpos, &ypos);	//pega a posição clicada

			GLfloat x = xpos;
			GLfloat y = ypos;

			vert->push_back(x);		//seta a posição como vertice de controle
			vert->push_back(y);
			vert->push_back(0.0);

			vert->push_back(r);
			vert->push_back(g);		//adiciona o parametro da cor tambem
			vert->push_back(b);

			des = true;				//e habilita o desenho se for o primeiro ponto clicado

			if (vert->size() / 6 >= 4) {	//se tiver mais que quatro pontos de controle, gerar as curvas normal, interna e externa chamando seus metodos
				fim = true;					//habilita que pode desenhar as curvas agora
				gerarCurva();
				gerarcurvas(true);
				gerarcurvas(false);
			}

		
		}
		
		glBindVertexArray(VAO1);			//atualiza o VAO dos pontos de controle
		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert->size(), vert->data(), GL_STATIC_DRAW);

	}
}

void gerarCurva() {		//metodo da curva normal bslipne
	aux->clear();
	for (int i = 0; i < vert->size()/6; i++) {	//preencher o vetor temporario para o calculo
		aux->push_back(vert->at(6 * i + 0));
		aux->push_back(vert->at(6 * i + 1));
		aux->push_back(vert->at(6 * i + 2));
	}
	curva->clear();	//limpa o vetor que armazenar os vertices da curva

	aux->push_back(vert->at(0));	//aqui colocamos mais três posições a mais para o calculo, já que o calculo acessa as tres posições, para quando for a ultima posição, não acessar uma posição invalida
	aux->push_back(vert->at(1));
	aux->push_back(vert->at(2));

	aux->push_back(vert->at(6));
	aux->push_back(vert->at(7));
	aux->push_back(vert->at(8));

	aux->push_back(vert->at(12));
	aux->push_back(vert->at(13));
	aux->push_back(vert->at(14));
	
	for (int i = 0; i < (aux->size()/3 - 3); i++)  {	//calculo da curva
		for (GLfloat t = 0; t <= 1.0; t += 0.05f) {		//x
			GLfloat x = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*aux->at(3 * i) +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*aux->at(3 * (i + 1)) +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*aux->at(3 * (i + 2)) +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*aux->at(3 * (i + 3))) / 6.0);
														//y
			GLfloat y = (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*aux->at((3 * i)+1) +
				(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4)*aux->at((3 * (i + 1))+1) +
				(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*aux->at((3 * (i + 2))+1) +
				(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0)*aux->at((3 * (i + 3))+1)) / 6.0);

			curva->push_back(x);					//adiciona o ponto no vetor da curva
			curva->push_back(y);
			curva->push_back(vert->at(6 * i + 2));
			curva->push_back(vert->at(6 * i + 3));
			curva->push_back(vert->at(6 * i + 4));
			curva->push_back(vert->at(6 * i + 5));
		}
	}
	curva->push_back(curva->at(0));			//adicionar o primeiro ponto na ultima posição para fechar a curva
	curva->push_back(curva->at(1));
	curva->push_back(curva->at(2));
	curva->push_back(curva->at(3));
	curva->push_back(curva->at(4));
	curva->push_back(curva->at(5));

	glBindVertexArray(VAO2);			//atualiza o vetor da curva
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*curva->size(), curva->data(), GL_STATIC_DRAW);
}

int desenhofinal() {					//metodo para desenhar novamente, só que agora com a animação do carrinho andando na pista
	ObjReader * obj = new ObjReader();		//inicializa o leitor de obj
	Mesh * malha = obj->read("teste2.obj");	//malha do obj lido

	vector<GLfloat>* vert = new vector<GLfloat>();	//vetor de vertices, normais e textura
	vector<GLint>* indi = new vector<GLint>();		//vetor com indices para o desenho

	vector<GLint>* indi2 = new vector<GLint>();	//indice do carro

	indi2->push_back(0);						//setar os indices do carro
	indi2->push_back(1);
	indi2->push_back(3);
	indi2->push_back(1);
	indi2->push_back(2);
	indi2->push_back(3);

	for (int k = 0; k < malha->getGroup(0)->getFaces().size(); k++) {	//carregar os indices da face da pista
		indi->push_back(malha->getGroup(0)->getFace(k)->getV(0));
		indi->push_back(malha->getGroup(0)->getFace(k)->getV(1));
		indi->push_back(malha->getGroup(0)->getFace(k)->getV(2));
	}

	for (int k = 0; k < malha->getVector().size() / 2; k++) {		//carregar e transformar os pontos da pista que se adeque a essa projeição
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

	vert2->push_back(vert->at(0));				//pegar os primeiros vertices da pista para mapear o carro 
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

	vector<Material*> materiais;							//carregar o material da pista com parametros da iluminação
	obj->readermaterial(malha->getnomematerial(), materiais);

	glfwInit();												//iniciar o glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//passos semelhantes a primeira inicialização da glfw feita lá em cima

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
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);		//redimensionar

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	glfwSetMouseButtonCallback(window, mouse_button_callback);	//evento do mouse

	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//vertexshader
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
		"   ourPos = vec4(ModelMatrix * vec4(aPos, 1.f)).xyz;"		//calculos da posição da camera e visualização e passagem de parametros para o fragment
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
	//fragment
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
		"   vec3 ambientLight = kambiente;"			//calculo da luz ambiente
		"   vec3 posToLightDirVec = normalize(lightPos0 - ourPos);"	//calculo da difusa
		"   vec3 diffuseColor = kdifusao;"
		"   float diffuse = clamp(dot(posToLightDirVec, ourNormal), 0, 1);"
		"   vec3 diffuseFinal = diffuseColor * diffuse;"
		"   vec3 lightToPosDirVec = normalize(ourPos - lightPos0);"		//calculo da especular
		"   vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(ourNormal)));"
		"   vec3 posToViewDirVec = normalize(cameraPos - ourPos );"
		"   float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), shiny.x);"
		"   vec3 specularFinal = kespecular * specularConstant;"
		"   FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * (vec4(ambientLight, 1.f) + vec4(diffuseFinal, 1.0f) + vec4(specularFinal, 1.f));"	//saida do resultado final
		"}";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);			//mesma logica da do começo do codigo
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	
	unsigned int VBO, VAO, EBO;		//vao, vbo e ebo do pista
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert->size(), vert->data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);		//linkar os indices com vertice desenhado
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint)*indi->size(), indi->data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);	//a cada 8 floats, um ponto novo, sendo que os 3 primeiros sao os vertices
	glEnableVertexAttribArray(0);
	// os proximos 2 sao a coordenada da textura
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// as normais
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	unsigned int VBO7, VAO7, EBO7; //vao do carro
	glGenVertexArrays(1, &VAO7);
	glGenBuffers(1, &VBO7);
	glGenBuffers(1, &EBO7);

	glBindVertexArray(VAO7);

	glBindBuffer(GL_ARRAY_BUFFER, VBO7);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert2->size(), vert2->data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO7);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint)*indi2->size(), indi2->data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


	glBindVertexArray(0);

	unsigned int texture1, texture2, texture3;	//texturas
	int width, height, nrChannels;

	unsigned char *data = stbi_load(materiais.at(0)->getArquivo().c_str(), &width, &height, &nrChannels, 0); //carrega a imagem da pista
	glGenTextures(1, &texture1); //aloca memoria para imagems
	glBindTexture(GL_TEXTURE_2D, texture1);	//defini o int como referencia da textura

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);		//parametros da texura e seu comportamento depois de mapeada
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);	//se tiver o caminha da imagem corretamente, linka esses valores como textura
		glGenerateMipmap(GL_TEXTURE_2D); //linka com fragment
	}
	else{
		std::cout << "Failed to load texture" << std::endl;	//senão falha no carregar a imagem
	}
	stbi_image_free(data);//libera memoria

	data = stbi_load(materiais.at(1)->getArquivo().c_str(), &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//mesma coisa da textura 1, agora com a 2
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if (data){
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//textura 3
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); //link das 2 primeiras texturas com o fragment 
	glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);	//bind das texturas
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glm::vec3 position(0.f);	//posição e transformações e rotações da camera e visão da janela
	glm::vec3 rotation(0.f);
	glm::vec3 scale(1.f);

	glm::mat4 ModelMatrix(1.f);
	ModelMatrix = glm::translate(ModelMatrix, position);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));	//calculos da camera
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
	ModelMatrix = glm::scale(ModelMatrix, scale);

	glm::vec3 camPosition(0.f, 0.f, 1.f);	//posições iniciais da camera
	glm::vec3 worldUp(0.f, 1.f, 0.f);
	glm::vec3 camFront(0.f, 0.f, -1.f);

	glm::mat4 ViewMatrix(1.f);				//a matriz da view da tela com os calculos
	ViewMatrix = glm::lookAt(camPosition, camPosition + camFront, worldUp);

	float fov = 90.f;				//parametros da visao e projeção para a janela
	float nearPlane = 0.1f;
	float farPlane = 1000.f;
	glm::mat4 ProjectionMatrix(1.f);	//matriz de projeção

	ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);

	glm::vec3 lightPos0(0.0f, 0.f, 1.f);			//constantes para a conta da iluminação
	glm::vec3 kambiente = materiais.at(0)->getKA();
	glm::vec3 kdifusao = materiais.at(0)->getKD();
	glm::vec3 kespecular = materiais.at(0)->getKS();
	glm::vec3 shiny = materiais.at(0)->getShiny();

	glUseProgram(shaderProgram);		//linka o shader para a passagem desses valores para o fragment

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
	int verif = 0;	//inicia o contador da animação do carro

	while (!glfwWindowShouldClose(window))	//loop do desenho
	{
		processInput(window);	//inputs do teclado
		processInput(window, position, rotation, scale);		//inputs do teclado para a camera

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);					//limpa a tela
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);							//sinaliza o shader a ser usado
		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);	//seta as duas texturas
		glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

		movimentar(vert, verif);			//metodo para movimentar o carro a partir dos pontos da pista
		verif = verif + 1;					//passa para o proximo ponto
		verif = verif % (vert->size()/32);	//e assegura que não passará do limite

		glBindVertexArray(VAO7);			//atualiza o VAO do carro
		glBindBuffer(GL_ARRAY_BUFFER, VBO7);	//VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vert2->size(), vert2->data(), GL_STATIC_DRAW);

		ModelMatrix = glm::mat4(1.f);
		ModelMatrix = glm::translate(ModelMatrix, position);	//atualiza conforme a movimentação da camera
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
		ModelMatrix = glm::scale(ModelMatrix, scale);
		//link da matriz com o shader
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));

		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight); //pega o tamanho atual da janela

		ProjectionMatrix = glm::mat4(1.f);		//tamanho da janela é importante para o calculo da projeção
		ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, nearPlane, farPlane);
		//link da projeção para o shader
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

		glActiveTexture(GL_TEXTURE0);	//ativa as 2 primeiras texturas para pista
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO);			//desenha a pista
		glDrawElements(GL_TRIANGLES, malha->getVector().size() * 4, GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE0);		//ativa a textura do carro
		glBindTexture(GL_TEXTURE_2D, texture3); 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture3);

		glBindVertexArray(VAO7);		//desenha o carro
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();			//eventos de interrupção
		glBindVertexArray(0);		//binds do vertex, texturae shader
		glUseProgram(0);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	//deleta os VAOS
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO7);
	glDeleteBuffers(1, &VBO7);
	glDeleteBuffers(1, &EBO7);
	glfwDestroyWindow(window);
	glDeleteProgram(shaderProgram);

	glfwTerminate();//encerra a glfw
	return 0;
}

void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)//metodo do teclado para camera
{				//modifica os valores conforme a tecla apertada para a movimentação da camera
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

void movimentar(vector<float> * vert, int contador) {		//atualizar os pontos do carro conforme a pista gerada pelo obj
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