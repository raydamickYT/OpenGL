#include <iostream>
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h> 

int resX = 1280, resY = 720;
//forward declaration
void processInput(GLFWwindow* window);
int Init(GLFWwindow* &window);
void createTriangle(GLuint &vao, int &size);
bool createShaders();

bool loadFileToBuffer(const std::string& filename, char*& buffer, size_t& bufferSize) {
	std::ifstream file(filename, std::ifstream::binary);
	if (!file) {
		std::cerr << "Kan het bestand niet openen: " << filename << std::endl;
		return false;
	}

	// Bepaal de grootte van het bestand
	file.seekg(0, file.end);
	size_t fileSize = file.tellg();
	file.seekg(0, file.beg);

	// Toewijzen van geheugen voor de buffer
	buffer = new char[fileSize + 1];  // +1 voor null-terminator

	// Lees het bestand in de buffer
	file.read(buffer, fileSize);
	buffer[fileSize] = '\0';  // Null-terminator toevoegen

	if (!file) {
		std::cerr << "Fout tijdens het lezen van het bestand." << std::endl;
		delete[] buffer;  // Vergeet niet het geheugen vrij te geven bij fouten
		buffer = nullptr;
		return false;
	}

	bufferSize = fileSize + 1;
	file.close();
	return true;
}

int main() {

	GLFWwindow* window;

	//call init
	int res = Init(window);
	if (res != 0) return res;

	GLuint triangleVAO;
	int triangleSize;
	createTriangle(triangleVAO, triangleSize);
	createShaders();
	
	//Open ViewPort
	glViewport(0, 0, resX, resY);

	while (!glfwWindowShouldClose(window)) 
	{


		//process hier je input
		processInput(window);

		//Rendering
		glClearColor(0.5, 0.3, 0.6, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, triangleSize);

		//swap && poll
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}
int Init(GLFWwindow* &window) {
	//glfwINIT
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(resX, resY, "GLFW OpenGL3 test demo", NULL, NULL);
	//Create window
	if (!window)
	{
		std::cout << "FAILED to init window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
		return -1;
	}

	glfwMakeContextCurrent(window);

	//load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to init GLAD" << std::endl;
		return -1;
	}

	return 0;
}

void createTriangle(GLuint& vao, int& size) {
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	size = sizeof(vertices);
}
bool createShaders() {
	size_t BUFFER_SIZE = 1024; // Aanpassen aan de verwachte grootte van het bestand
	char* vertexBuffer = nullptr;
	char* fragmentBuffer = nullptr;

	if (!loadFileToBuffer("shaders/simpleVertex.shader", vertexBuffer, BUFFER_SIZE)) {
		std::cerr << "Fout bij het laden van het vertex shader bestand." << std::endl;
		return false; // Laden mislukt
	}
	std::cout << "Vertex shader bestand succesvol geladen:\n" << vertexBuffer << std::endl;

	if (!loadFileToBuffer("shaders/simpleFragment.shader", fragmentBuffer, BUFFER_SIZE)) {
		std::cerr << "Fout bij het laden van het fragment shader bestand." << std::endl;
		delete[] vertexBuffer;
		return false; // Laden mislukt
	}
	std::cout << "Fragment shader bestand succesvol geladen:\n" << fragmentBuffer << std::endl;

	GLuint programID;
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexBuffer, nullptr);
	glCompileShader(vertexShaderID);
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShaderID, 512, nullptr, infoLog);
		std::cerr << "ERROR COMPILING VERTEX SHADER:\n" << infoLog << std::endl;
		delete[] vertexBuffer; delete[] fragmentBuffer;
		return false;
	}

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentBuffer, nullptr);
	glCompileShader(fragmentShaderID);
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShaderID, 512, nullptr, infoLog);
		std::cerr << "ERROR COMPILING FRAGMENT SHADER:\n" << infoLog << std::endl;
		delete[] vertexBuffer; delete[] fragmentBuffer;
		return false;
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, 512, nullptr, infoLog);
		std::cerr << "ERROR LINKING PROGRAM:\n" << infoLog << std::endl;
		return false;
	}


	// Shader cleanup na succesvolle compilatie, indien nodig
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	delete[] vertexBuffer;
	delete[] fragmentBuffer;


	return true; // Shaders succesvol geladen en gecompileerd
}