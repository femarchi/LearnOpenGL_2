#include <iostream>
#include <glad/glad.h> // must be included before glfw
#include <GLFW/glfw3.h>
#include <cmath>
#include <stb_image.h>
#include <shader.h>

const unsigned int OPENGL_CLIENT_API_VERSION = 3; // minimal version of openGL the client must use.
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char* VERTEX_SHADER_PATH = "C:/Projects/VS2019/LearnOpenGL_2/src/shaders/shader.vs";
const char* FRAGMENT_SHADER_PATH = "C:/Projects/VS2019/LearnOpenGL_2/src/shaders/shader.fs";
const char* CONTAINER_IMG_PATH = "C:/Projects/VS2019/LearnOpenGL_2/assets/container.jpg";
const char* FACE_IMG_PATH = "C:/Projects/VS2019/LearnOpenGL_2/assets/awesomeface.png";

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

unsigned int createVertexArrayObject()
{

	float vertices[] = {
	//    x      y		z      r      g      b
		 0.5f,  0.5f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f,  1.0f, // 0 top right
		 0.5f, -0.5f,  0.0f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f, // 1 bottom right
		-0.5f, -0.5f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f,  0.0f, // 2 bottom left
		-0.5f,  0.5f,  0.0f,   1.0f,  1.0f,  0.0f,   0.0f,  1.0f  // 3 top left
	};
	unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
	};
	
	unsigned int VBO, VAO, EBO; // vertex buffer, element buffer and array objects
	glGenVertexArrays(1, &VAO); // create vertex arrays and assign memory location to VAO
	glGenBuffers(1, &VBO); // create buffer and assign memory location to VBO
	glGenBuffers(1, &EBO); // create buffer and assign memory location to VBO
	glBindVertexArray(VAO); // bind array object
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind buffer to OpenGL Array buffer location
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertices to bound buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // bind buffer to OpenGL Element Array buffer location
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(
		0, // location of the vertex attribute being configured (see vertexShaderSource)
		3, // this attribute is composed of 3 values
		GL_FLOAT, // type of data of vertex coordinates
		GL_FALSE, // no need to normalize the data
		8 * sizeof(float), // memory space of a vertex (stride)
		(void*)0 // offset in bytes of where this attribute begins in buffer
	);
	// enable vertex attributes at location 0 (disabled by default)
	glEnableVertexAttribArray(0);

	// set color attributes pointer (location = 1) and enable
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	// set texture attributes pointer (location = 2) and enable
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind, since glVertexAttribPointer registered VBO as the vertex attribute pointer
	glBindVertexArray(0); // unbind, same as above for VAO
	return VAO;
}

void animateTriangle()
{
	// GL_ARRAY_BUFFER must still be bound
	float t = (float)glfwGetTime();
	float xPos = sin(t) / 2.0f;
	float vertices[] = {
		0.0f, 0.5f, 0.0f,
		0.0f, -0.5f, 0.0f,
		xPos, 0.0f, 0.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

unsigned int generateTexture(const char* path, int colorFormat)
{
	unsigned int texture; // array that holds generated texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // bind texture to OpenGL 2d texture pointer
	 // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// use stb_image loader to load image data
	int width, height, nrChannels;
	// flip texture on the y-axis because openGL expects 0.0 y-coordinate to be on the bottom
	stbi_set_flip_vertically_on_load(true); 
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data)
	{
		// generate texture image and mipmaps from data
		glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load image" << std::endl;
		return 0;
	}
	//free data after generated
	stbi_image_free(data);

	return texture;
}

int main()
{
	std::cout << "Hello Textures" << std::endl;
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

	Shader ourShader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

	unsigned int VAO, texture1, texture2;
	VAO = createVertexArrayObject();
	texture1 = generateTexture(CONTAINER_IMG_PATH, GL_RGB);
	texture2 = generateTexture(FACE_IMG_PATH, GL_RGBA);
	ourShader.use(); // must use shader before setting uniforms
	ourShader.setInt("texture1", 0); // set texture1 as texture unit 0
	ourShader.setInt("texture2", 1); // set texture2 as texture unit 1

	// setup render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		
		// set clear color buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// clear buffer bit with color
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0); // set texture unit 0 as active before bind 
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1); // set texture unit 1 as active before bind 
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window); // show buffered pixels
		glfwPollEvents(); // check keyboard, mouse and other events
	}
	glfwTerminate(); // clear resources 

	return 0;
}