#include <iostream>
#include <glad/glad.h> // must be included before glfw
#include <GLFW/glfw3.h>
#include <cmath>
#include <stb_image.h>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <camera.h>

const unsigned int OPENGL_CLIENT_API_VERSION = 3; // minimal version of openGL the client must use.
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char* VERTEX_SHADER_PATH = "C:/Projects/VS2019/LearnOpenGL_2/src/shaders/shader.vs";
const char* FRAGMENT_SHADER_PATH = "C:/Projects/VS2019/LearnOpenGL_2/src/shaders/shader.fs";
const char* CONTAINER_IMG_PATH = "C:/Projects/VS2019/LearnOpenGL_2/assets/container.jpg";
const char* FACE_IMG_PATH = "C:/Projects/VS2019/LearnOpenGL_2/assets/awesomeface.png";

bool firstMouse = true;
float lastX = 800.0f / 2.0;  // last X mouse position (scr_height / 2 = middleX)
float lastY = 800.0f / 2.0;  // last Y mouse position (scr_width / 2 = middleY)

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera;

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

/* Callback that is called every frame with the current mouse position on the window */
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// avoid camera jump when mouse is captured
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// distance between the last and the current mouse position 
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
	// set escape key to close glfw window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

unsigned int createPlaneVertexArrayObject()
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

void rotateContainer(unsigned int shaderId)
{
	// order of operations is inverse, first rotate then translate
	glm::mat4 trans = glm::mat4(1.0f);
	//trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
	trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
	trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
	unsigned int transformLoc = glGetUniformLocation(shaderId, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
}


unsigned int createBoxVertexArrayObject() 
{
	// x y z | u v
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO); // create vertex arrays and assign memory location to VAO
	glGenBuffers(1, &VBO); // create buffer and assign memory location to VBO
	glBindVertexArray(VAO); // bind array object
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind buffer to OpenGL Array buffer location
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertices to bound buffer
	
	// position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	return VAO;
}

void setShaderModelMatrix(unsigned int shaderId)
{
	glm::mat4 model = glm::mat4(1.0f);
	// rotate model by t*50 degrees.
	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	unsigned int modelLoc = glGetUniformLocation(shaderId, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}


void setShaderViewMatrix(unsigned int shaderId)
{
	glm::mat4 view;
	view = glm::lookAt(
		camera.Position,
		camera.Position + camera.Front, // camera target (origin)
		camera.Up
	);
	unsigned int viewLoc = glGetUniformLocation(shaderId, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void setShaderProjectionMatrix(unsigned int shaderId)
{
	// create a projection matrix to transform vertices into a 3d perspective
	glm::mat4 projection;
	float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT; // ratio of the window
	float nearPlaneCoord = 0.1f; // view area starting z coord
	float farPlaneCoord = 100.0f; // view area ending z coord
	projection = glm::perspective(glm::radians(camera.Zoom), aspectRatio, nearPlaneCoord, farPlaneCoord);
	unsigned int projectionLoc = glGetUniformLocation(shaderId, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void drawMultipleCubes(unsigned int shaderId)
{
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		unsigned int modelLoc = glGetUniformLocation(shaderId, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

int main()
{
	std::cout << "Hello Camera" << std::endl;
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

	// Enable OpenGL to test depth so vertices behind faces don't get rendered
	glEnable(GL_DEPTH_TEST); 

	Shader ourShader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	camera = Camera();

	unsigned int VAO, texture1, texture2;
	VAO = createBoxVertexArrayObject();
	texture1 = generateTexture(CONTAINER_IMG_PATH, GL_RGB);
	texture2 = generateTexture(FACE_IMG_PATH, GL_RGBA);
	ourShader.use(); // must use shader before setting uniforms
	ourShader.setInt("texture1", 0); // set texture1 as texture unit 0
	ourShader.setInt("texture2", 1); // set texture2 as texture unit 1

	// hide mouse cursor and capture it
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// setup render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		
		// set clear color buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// clear buffer bit with color and depth test buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0); // set texture unit 0 as active before bind 
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1); // set texture unit 1 as active before bind 
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();
		setShaderModelMatrix(ourShader.ID);
		setShaderViewMatrix(ourShader.ID);
		setShaderProjectionMatrix(ourShader.ID);
		glBindVertexArray(VAO);
		drawMultipleCubes(ourShader.ID);

		glfwSwapBuffers(window); // show buffered pixels
		glfwPollEvents(); // check keyboard, mouse and other events
	}
	glfwTerminate(); // clear resources 

	return 0;
}