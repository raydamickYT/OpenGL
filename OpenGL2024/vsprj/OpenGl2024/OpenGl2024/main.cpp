#include <iostream>
#include <fstream>
#include <string>

#include "Triangle.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 
#include <math/linmath.h>



int resX = 1280, resY = 720;
//forward declaration
void processInput(GLFWwindow* window);
int Init(GLFWwindow* &window);

int main()
{
	Triangle myTriangle;

	myTriangle.setupShaders();
}


int Init(GLFWwindow* &window) {
	//glfwINIT
	if (!glfwInit())
		exit(EXIT_FAILURE);
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
	gladLoadGL();
	glfwSwapInterval(1);


	//load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to init GLAD" << std::endl;
		return -1;
	}

	return 0;
}