#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Utils.h"

using namespace std;
#define numVAOs 1
#define numVBOs 2
float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
// allocate variables used in display() function, so that they won’t need to be allocated during rendering
//GLuint mvLoc, projLoc;
GLuint vLoc, tfLoc, projLoc;
int width, height, displayLoopi;
float aspect;
float timeFactor;
glm::mat4 pMat, vMat, mMat, mvMat;

glm::mat4 tMat, rMat;	////	added two matrices - translation and rotation for using current time to compute different translations in x, y, and z

void setupVertices(void) {						//2) 
	float vertexPositions[108] = {				//	36 vertices, 12 triangles, 2 on each cube's face, makes 2x2x2 cube placed at origin
	 -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
	 -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
	};
	glGenVertexArrays(1, vao);					//	creating VAO
	glBindVertexArray(vao[0]);					//	make the specified VAO "active" so that the generated buffers will be accosiated with that VAO
	glGenBuffers(numVBOs, vbo);					//	creating VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);		//	make 0th buffer "active" 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);	//	copy the array containing the cube's vertices into the active buffer (0th VBO)
}
void init(GLFWwindow* window) {					//1)init() performs tasks that only need to be done once:
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");	//	reading in shader code and building the rendeering program
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians == 60 degrees

	// position the camera further down the positive Z axis (to see all of the cubes)
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 420.0f;
	setupVertices();																		//	loading cube vertices into the VBO	
}
void display(GLFWwindow* window, double currentTime) {	//3)display() may be called	repeatedly and the rate at which it is called is referred to as the frame rate
	glClear(GL_DEPTH_BUFFER_BIT);	//	usually necessary to clear the depth buffer before rendering a frame so that hidden surface removal occurs properly
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);	////	as we added color changing we need to clear the color buffer too

	glUseProgram(renderingProgram);	//	enabling shaders by installing the GLSL code on the GPU.
									//	 this doesn’t run the shader program, but it does enable subsequent OpenGL calls to determine the shader’s vertex attribute and uniform locations
	///////////////computations that build (and transform) mMat have been moved to the vertex shader
	///////////////there is no longer any need to build an MV matrix in the C++ application.


	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));


	timeFactor = ((float)currentTime);
	tfLoc = glGetUniformLocation(renderingProgram, "tf");
	glUniform1f(tfLoc, (float)timeFactor);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100000);

}
int main(void) { /// main() is unchanged from before
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(1080, 1366, "Chapter 4 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);
	init(window);
	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}