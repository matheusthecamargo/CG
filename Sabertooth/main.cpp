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
void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	ObjReader * obj = new ObjReader();
	Mesh * malha = obj->read("teste.obj");
	//glm::vec2 testete = malha->getIndT(22);
	//cout << malha->getGroup(0)->getFace(11)->getV(1);

	//vector<float> vertices;
	//normais.
	float vertices[192];
	unsigned int indices[36];
	//float * vertices = new float[tam];

	for (int k = 0; k < 24; k++) {
		vertices[k * 8 + 0] = malha->getIndV(k).x;
		vertices[k * 8 + 1] = malha->getIndV(k).y;
		vertices[k * 8 + 2] = malha->getIndV(k).z;
		vertices[k * 8 + 3] = malha->getIndT(k).x;
		vertices[k * 8 + 4] = malha->getIndT(k).y;
		vertices[k * 8 + 5] = malha->getIndN(k).x;
		vertices[k * 8 + 6] = malha->getIndN(k).y;
		vertices[k * 8 + 7] = malha->getIndN(k).z;
	}

	for (int k = 0; k < 12; k++) {
		indices[k * 3 + 0] = malha->getGroup(0)->getFace(k)->getV(0);
		indices[k * 3 + 1] = malha->getGroup(0)->getFace(k)->getV(1);
		indices[k * 3 + 2] = malha->getGroup(0)->getFace(k)->getV(2);
	}

	vector<Material*> materiais;
	obj->readermaterial(malha->getnomematerial(), materiais);
	cout << materiais.at(1)->getArquivo();

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
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	/*
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
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

	
	// ------------------------------------------------------------------
	/*float vertices[] = {
		// positions            //texturecoords  //normals
		0.5f,  0.5f, 0.0f,      1.0f, 1.0f,		0.0f, 0.0f, 1.0f,			// top right
		0.5f, -0.5f, 0.0f,      1.0f, 0.0f,		0.0f, 0.0f, 1.0f,			// bottom right
		-0.5f, -0.5f, 0.0f,     0.0f, 0.0f,		0.0f, 0.0f, 1.0f,			// bottom left
		-0.5f,  0.5f, 0.0f,     0.0f, 1.0f,		0.0f, 0.0f, 1.0f,			// top left
		0.5f,  0.5f, -1.f,      1.0f, 1.0f,		0.0f, 0.0f, 1.0f,			// top right back	//2
		0.5f, -0.5f, -1.f,      1.0f, 0.0f,		0.0f, 0.0f, 1.0f,			// bottom right back
		-0.5f, -0.5f, -1.f,     0.0f, 0.0f,		0.0f, 0.0f, 1.0f,			// bottom left back
		-0.5f,  0.5f, -1.f,     0.0f, 1.0f,		0.0f, 0.0f, 1.0f,			// top left back
		0.5f,  0.5f, -1.f,      1.0f, 1.0f,		0.0f, 0.0f, 1.0f,			// top right    //3
		0.5f, -0.5f, -1.f,      1.0f, 0.0f,		0.0f, 0.0f, 1.0f,			// bottom right 
		0.5f, -0.5f, 0.0f,      0.0f, 0.0f,		0.0f, 0.0f, 1.0f,			// top left
		0.5f,  0.5f, 0.0f,      0.0f, 1.0f,		0.0f, 0.0f, 1.0f,			// bottom left
		-0.5f,  0.5f, 0.0f,     1.0f, 1.0f,		0.0f, 0.0f, 1.0f,			//4
		-0.5f, -0.5f, 0.0f,     1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -1.f,     0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -1.f,     0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -1.f,      1.0f, 1.0f,		0.0f, 0.0f, 1.0f,			//5
		0.5f,  0.5f, 0.0f,      1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -1.f,     0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f,      1.0f, 1.0f,		0.0f, 0.0f, 1.0f,			//6
		0.5f, -0.5f, -1.f,      1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -1.f,     0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,     0.0f, 1.0f,		0.0f, 0.0f, 1.0f

	};*/
	/*unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3,  // second triangle
		4, 5, 7, // first triangle
		5, 6, 7,  // second triangle
		8, 9, 11, // first triangle
		9, 10, 11,  // second triangle
		12, 13, 15, // first triangle
		13, 14, 15,  // second triangle
		16, 17, 19, // first triangle
		17, 18, 19,  // second triangle
		20, 21, 23, // first triangle
		21, 22, 23  // second triangle
	};*/
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	int width, height, nrChannels;
	// texture 1
	// ---------
	//unsigned char *data = SOIL_load_image("container.jpg", &width, &height, &nrChannels, SOIL_LOAD_RGBA);
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
											// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	
	
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
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

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	//ourShader->use(); // don't forget to activate/use the shader before setting uniforms!
					 // either set it manually like so:
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

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);
		processInput(window, position, rotation, scale);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

		/*position.z -= 0.01f;
		rotation.y += 2.0;
		scale.x += 0.01f;*/

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
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
		glBindVertexArray(0);
		glUseProgram(0);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwDestroyWindow(window);
	glDeleteProgram(shaderProgram);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
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