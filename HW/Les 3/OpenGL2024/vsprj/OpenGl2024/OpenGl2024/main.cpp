#include "Triangle.h"

int resX = 1280, resY = 720;

int Init(GLFWwindow*& window);

int main()
{
	GLFWwindow* window;
	if (Init(window) != 0) {
		return -1;
	}

	Triangle myTriangle;
	myTriangle.setupShaders();
	//myTriangle.render(window);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


int Init(GLFWwindow*& window) {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(resX, resY, "GLFW OpenGL3 Test Demo", NULL, NULL);
	if (!window)
	{
		std::cout << "FAILED to init window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSwapInterval(1);

	return 0;
}
