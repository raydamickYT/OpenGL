#include "Triangle.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

void Triangle::setupTextures() {
	dirt = loadTexture("Textures/BlockText.jpg", 0);
	//sand = loadTexture("Textures/sand.jpg", 0);
	//grass = loadTexture("Textures/grass.png", 4);
	//rock = loadTexture("Textures/rock.jpg", 0);
	//snow = loadTexture("Textures/snow.jpg", 0);
}

void Triangle::processUniforms(GLuint program) {
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "ourTexture"), 0);
	//glUniform1i(glGetUniformLocation(program, "sand"), 1);
	//glUniform1i(glGetUniformLocation(program, "grass"), 2);
	//glUniform1i(glGetUniformLocation(program, "rock"), 3);
	//glUniform1i(glGetUniformLocation(program, "snow"), 4);
}

unsigned int Triangle::loadTexture(const std::string& url, int comp) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, numChannels;
	unsigned char* data = stbi_load(url.c_str(), &width, &height, &numChannels, comp);

	if (data) {
		std::cout << "Loaded texture data: " << static_cast<void*>(data) << std::endl;
		std::cout << "Texture width: " << width << ", height: " << height << ", channels: " << numChannels << std::endl;

		if (comp != 0) numChannels = comp;

		switch (numChannels) {
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		default:
			std::cerr << "Unsupported number of channels: " << numChannels << std::endl;
			stbi_image_free(data);
			return 0;
		}

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "Error uploading texture data to GPU: " << error << std::endl;
		}

		glGenerateMipmap(GL_TEXTURE_2D);
		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "Error generating mipmaps: " << error << std::endl;
		}

		std::cout << "Texture loaded successfully: " << url << std::endl;
	}
	else {
		std::cerr << "Failed to load texture: " << url << std::endl;
		return 0;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

void Triangle::setupShaders()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "OpenGL Cube", NULL, NULL);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return;
	}
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);

	GLuint vertex_buffer, index_buffer;
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &index_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	const GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glUseProgram(program);

	const GLint mvp_location = glGetUniformLocation(program, "MVP");
	const GLint vpos_location = glGetAttribLocation(program, "vPos");
	const GLint vcol_location = glGetAttribLocation(program, "vCol");
	const GLint vuv_location = glGetAttribLocation(program, "vUv");

	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	int stride = (3 + 3 + 2 + 3) * sizeof(float);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(vuv_location);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	setupTextures();
	processUniforms(program);

	while (!glfwWindowShouldClose(window))
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		const float ratio = width / (float)height;

		processInput(window);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4x4 m, p, mvp;
		mat4x4_identity(m);
		mat4x4_rotate_Y(m, m, (float)glfwGetTime());
		mat4x4_perspective(p, 1.0f, ratio, 0.1f, 10.0f);
		mat4x4_translate_in_place(p, 0.0f, 0.0f, -2.5f);
		mat4x4_mul(mvp, p, m);

		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp);
		glBindVertexArray(vertex_array);
		glBindTexture(GL_TEXTURE_2D, dirt); // Bind the texture before drawing
		glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(cubeIndices[0]), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &index_buffer);
	glDeleteVertexArrays(1, &vertex_array);
	glDeleteProgram(program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void Triangle::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}