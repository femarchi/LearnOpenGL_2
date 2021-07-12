#include <iostream>
#include <glad/glad.h> // must be included before glfw
#include <GLFW/glfw3.h>
#include <cmath>

int OPENGL_CLIENT_API_VERSION = 3; // minimal version of openGL the client must use.

void setupGlfw()
{
	// initialize glfw
	glfwInit();
	// set minimal client openGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_CLIENT_API_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_CLIENT_API_VERSION);
	// specify openGL profile as core (not compatibility)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

/* Callback that initializes the viewport with correct size on every window resize */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	int xPos = 0;
	int yPos = 0;
	glViewport(xPos, yPos, width, height);
}

void processInput(GLFWwindow* window)
{
	// set escape key to close glfw window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

int main()
{
	std::cout << "Hello Window" << std::endl;
	setupGlfw();

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// make window context the main context on the current thread
	glfwMakeContextCurrent(window);
	// get OS specific address of OpenGL function pointers and load into GLAD
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// register callback that initializes viewport matching window size 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	int t = 0;

	// setup render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glfwSwapBuffers(window); // show buffered pixels
		glfwPollEvents(); // check keyboard, mouse and other events
		
		if (t > 60) 
		{
			r = fmod(r + 0.01f, 1.0f);
			g = fmod(g + 0.01f, 1.0f);
			b = fmod(b + 0.01f, 1.0f);
			t = 0;
		}
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		t += 1;
	}
	glfwTerminate(); // clear resources 

	return 0;
}