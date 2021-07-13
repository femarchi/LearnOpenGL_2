#include <iostream>
#include <glad/glad.h> // must be included before glfw
#include <GLFW/glfw3.h>
#include <cmath>

const unsigned int OPENGL_CLIENT_API_VERSION = 3; // minimal version of openGL the client must use.
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

void checkShaderCompilation(unsigned int shader, const char* type)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::"<< type <<"::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}


void checkProgramLink(unsigned int program)
{
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}
}

unsigned int createVertexShader()
{
	// vertex shader in GLSL language
	const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

	// create a vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// source the code into the created shader and compile
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	checkShaderCompilation(vertexShader, "VERTEX");
	return vertexShader;
}

unsigned int createFragmentShader()
{
	const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\0";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	checkShaderCompilation(fragmentShader, "FRAGMENT");
	return fragmentShader;
}

unsigned int createShaderProgram() 
{
	unsigned int vertexShader = createVertexShader();
	unsigned int fragmentShader = createFragmentShader();
	// create a program to link the vertex and fragment shaders
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkProgramLink(shaderProgram);

	// after shaders are linked we can delete them
	glDeleteShader(shaderProgram);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

unsigned int createVertexArrayObject()
{

	float vertices[] = {
	//    x      y      z
		 0.5f,  0.5f,  0.0f, // 0
		 0.5f, -0.5f,  0.0f, // 1
		-0.5f, -0.5f,  0.0f, // 2
		-0.5f,  0.5f,  0.0f, // 3
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	
	unsigned int VBO, VAO, EBO; // vertex buffer, element buffer and array objects
	glGenVertexArrays(1, &VAO); // create vertex arrays and assign memory location to VAO
	glGenBuffers(1, &VBO); // create buffer and assign memory location to VBO
	glGenBuffers(1, &EBO); // create buffer and assign memory location to EBO
	glBindVertexArray(VAO); // bind array object
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind buffer to OpenGL Array buffer location
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertices to bound buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // bind buffer to OpenGL Element Array buffer location
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(
		0, // location of the vertex attribute being configured (see vertexShaderSource)
		3, // vertex is composed of 3 values
		GL_FLOAT, // type of data of vertex coordinates
		GL_FALSE, // no need to normalize the data
		3 * sizeof(float), // memory space of a vertex (stride)
		(void*)0 // offset of where data begins in buffer
	);
	// enable vertex attributes at location 0 (disabled by default)
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind, since glVertexAttribPointer registered VBO as the vertex attribute pointer
	glBindVertexArray(0); // unbind, same as above for VAO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // don't unbind the EBO since it's stored in the VAO
	return VAO;
}

int main()
{
	std::cout << "Hello Rectangle" << std::endl;
	setupGlfw();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// make window context the main context on the current thread
	glfwMakeContextCurrent(window);
	// get OS specific address of OpenGL function pointers and load into GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	};
	// register callback that initializes viewport matching window size 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	unsigned int VAO, shaderProgram;
	VAO = createVertexArrayObject();
	shaderProgram = createShaderProgram();

	// setup render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		
		// set clear color buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// clear buffer bit with color
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // use indices to connect bound vertices 

		glfwSwapBuffers(window); // show buffered pixels
		glfwPollEvents(); // check keyboard, mouse and other events
	}
	glfwTerminate(); // clear resources 

	return 0;
}