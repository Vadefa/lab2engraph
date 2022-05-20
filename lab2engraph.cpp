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
GLuint mvLoc, projLoc;
int width, height, displayLoopi;
float aspect;
float tf;
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
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
	cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f; /// shift down Y to reveal perspective
	setupVertices();																		//	loading cube vertices into the VBO	
}
void display(GLFWwindow* window, double currentTime) {	//3)display() may be called	repeatedly and the rate at which it is called is referred to as the frame rate
	glClear(GL_DEPTH_BUFFER_BIT);	//	usually necessary to clear the depth buffer before rendering a frame so that hidden surface removal occurs properly

	glClear(GL_COLOR_BUFFER_BIT);	////	as we added color changing we need to clear the color buffer too

	glUseProgram(renderingProgram);	//	enabling shaders by installing the GLSL code on the GPU.
									//	 this doesn’t run the shader program, but it does enable subsequent OpenGL calls to determine the shader’s vertex attribute and uniform locations
	/// get the uniform variables for the MV and projection matrices
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");	//	 get locations of uniforms in the shader program
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	/// build perspective matrix
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees
	/// build view matrix, model matrix, and model-view matrix
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	for (displayLoopi = 0; displayLoopi < 24; displayLoopi++)
	{
		tf = currentTime + displayLoopi; // tf == "time factor", declared as type float

	////// use current time to compute different translations in x, y, and z
		tMat = glm::translate(glm::mat4(1.0f), glm::vec3(sin(.35f * tf) * 8.0f, cos(.52f * tf) * 8.0f,
			sin(.70f * tf) * 8.0f));

		rMat = glm::rotate(glm::mat4(1.0f), 1.75f * (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
		rMat = glm::rotate(rMat, 1.75f * (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f));
		rMat = glm::rotate(rMat, 1.75f * (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
		////// the 1.75 adjusts the rotation speed
		mMat = tMat * rMat;	////	rotation initially -> next the translation performed

		////mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));

		mvMat = vMat * mMat;

		/// copy perspective and MV matrices to corresponding uniform variables
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));	//	send matrix data to the uniform variables
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
		/// associate VBO with the corresponding vertex attribute in the vertex shader
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	//	enables the buffer containing the cube vertices
		glEnableVertexAttribArray(0);							//	and attaches it to 0th vertex attribute to prepare for sending the vertices to the shader.
		/// adjust OpenGL settings and draw model
		glEnable(GL_DEPTH_TEST);	//	enable depth testing
		glDepthFunc(GL_LEQUAL);		//	and specify the particular depth test we wish OpenGL to use
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}
int main(void) { /// main() is unchanged from before
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(1080, 720, "Chapter 4 - program 1", NULL, NULL);
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