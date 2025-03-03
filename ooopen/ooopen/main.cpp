#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "mycamera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// you can change the size of the window here
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera, see camera class if you need to, you can import the camera appeared in the assignment if you find it difficult to use
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(-7.0f, 0.0f, -3.0f);

int main()
{
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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Evil Cult", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// don't touch my shaders if you don't know what you want to do, create your own, if you don't know how, ask me
	// -------------------------
	Shader sceneShader("../sources/shaders/vshader_SCENE.txt", "../sources/shaders/fshader_SCENE.txt");
	Shader lampShader("../sources/shaders/lamp_vshader.txt", "../sources/shaders/lamp_fshader.txt");

	// load scene models
	// -----------
	Model bridgeModel("../sources/models/371model/podium.obj");
	Model towerModel("../sources/models/comp371project/sceneObjects.obj");

	float vertices[] = {
	   -0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
	   -0.5f,  0.5f, -0.5f,
	   -0.5f, -0.5f, -0.5f,

	   -0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
	   -0.5f,  0.5f,  0.5f,
	   -0.5f, -0.5f,  0.5f,

	   -0.5f,  0.5f,  0.5f,
	   -0.5f,  0.5f, -0.5f,
	   -0.5f, -0.5f, -0.5f,
	   -0.5f, -0.5f, -0.5f,
	   -0.5f, -0.5f,  0.5f,
	   -0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

	   -0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
	   -0.5f, -0.5f,  0.5f,
	   -0.5f, -0.5f, -0.5f,

	   -0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
	   -0.5f,  0.5f,  0.5f,
	   -0.5f,  0.5f, -0.5f,
	};

	unsigned int VBO, lightVAO;
	glGenVertexArrays(1, &lightVAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(lightVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	//don't touch the stuff below
	glm::vec3 bridgePositions[] = {
		glm::vec3(0.0f, -1.75f, 0.0f),
		glm::vec3(0.0f, 0.0f,-48.0f),
		glm::vec3(-48.0f, 0.0f, -48.0f),
		glm::vec3(-48.0f, 0.0f, 0.0f)
	};

	float bridgeRotationAngles[] = {
		0,90,180,270
	};


	glm::vec3 towerPositions[] = {
		glm::vec3(8.0f, 19.0f, -45.0f),
		glm::vec3(-55.0f, 0.0f,-60.0f),
		glm::vec3(-70.0f, 0.0f, -33.0f),
		glm::vec3(-40.0f, 0.0f, -65.0f)
	};

	float towerRotationAngles[] = {
		0,70,120,90
	};




	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//these are realted to light, you don't need to do change here, by the way the shader_m I added is really easier to use
		sceneShader.use();

		sceneShader.setVec3("pLight.position", lightPos);
		sceneShader.setVec3("viewPos", camera.Position);

		// light properties
		sceneShader.setVec3("pLight.ambient", 0.5f, 0.5f, 0.5f);
		sceneShader.setVec3("pLight.diffuse", 0.8f, 0.8f, 0.8f);
		sceneShader.setVec3("pLight.specular", 1.0f, 1.0f, 1.0f);
		sceneShader.setFloat("pLight.constant", 1.0f);
		sceneShader.setFloat("pLight.linear", 0.09f);
		sceneShader.setFloat("pLight.quadratic", 0.032f);

		sceneShader.setVec3("dLight.direction", 10.0f, 20.0f, -5.0f);
		sceneShader.setVec3("dLight.ambient", 1.0f, 1.0f, 1.0f);
		sceneShader.setVec3("dLight.diffuse", 0.6f, 0.6f, 0.6f);
		sceneShader.setVec3("dLight.specular", 0.5f, 0.5f, 0.5f);


		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		sceneShader.setMat4("projection", projection);
		sceneShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		// render the loaded model
		for (unsigned int i = 0; i < 4; i++) {
			 // translate it down so it's at the center of the scene
			model = glm::translate(model, bridgePositions[i]);
			model = glm::rotate(model, glm::radians(bridgeRotationAngles[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			sceneShader.setMat4("model", model);
			bridgeModel.Draw(sceneShader);
		}

		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		for (unsigned int i = 0; i < 4; i++) {
			
			model = glm::translate(model, towerPositions[i]);
			model = glm::rotate(model, glm::radians(towerRotationAngles[i]), glm::vec3(0.0f, 1.0f, 0.0f));

			sceneShader.setMat4("model", model);
			towerModel.Draw(sceneShader);
		}
		
		//I added a cube where there's a point light, comment it if you want
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.5f)); // a smaller cube
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}