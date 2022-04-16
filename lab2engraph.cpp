#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//код скрипта вершинного шейдера:
static const char* pVS = "															\n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec4 Color;                                                                     \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
    Color = vec4(clamp(Position, 0.0, 0.8), 1.0);                                   \n\
}";

//код скрипта фрагментного шейдера
static const char* pFS = "															\n\
#version 330                                                                        \n\
                                                                                    \n\
in vec4 Color;                                                                      \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = Color;                                                              \n\
}";

GLuint VBO;
GLuint gWorldLocation;
GLuint IBO;				//указатель на буферный объект для индексов вершин


void RenderSceneCB() {

	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;
	Scale += 0.011f;

	glm::mat4x4 World{};
	World[0][0] = sinf(Scale);	World[0][1] = 0.0f;			World[0][2] = 0.0f;			World[0][3] = 0.0f;
	World[1][0] = 0.0f;			World[1][1] = cosf(Scale);	World[1][2] = 0.0f;			World[1][3] = 0.0f;
	World[2][0] = 0.0f;			World[2][1] = 0.0f;			World[2][2] = sinf(Scale);	World[2][3] = 0.0f;
	World[3][0] = 0.0f;			World[3][1] = 0.0f;			World[3][2] = 0.0f;			World[3][3] = 1.0f;

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);		//привязали и индексный буфер тоже

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);	//теперь мы выводим 
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
	glutPostRedisplay(); // останавливает повторный вызов ленивой функции
}


static void createVertexBuffer() {
	/*glm::vec3 vertices[3];
	vertices[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
	vertices[1] = glm::vec3(1.0f, -1.0f, 0.0f);
	vertices[2] = glm::vec3(0.0f, 1.0f, 0.0f);*/

	glm::vec3 vertices[4];						//мы делаем пирамиду.теперь нам нужно 4 вектора для 4 вершин и 4 треугольников
	vertices[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
	vertices[1] = glm::vec3(0.0f, -1.0f, 1.0f);
	vertices[2] = glm::vec3(1.0f, -1.0f, 0.0f);
	vertices[3] = glm::vec3(0.0f, 1.0f, 0.0f);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	unsigned int Indices[] = { 0, 3, 1,		// массив индексов, указывающих на расположение вершин в вершинном буфере
							1, 3, 2,
							2, 3, 0,
							0, 2, 1 };

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

}

static void createShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
	//GLuint ShaderProgram = glCreateProgram();

	GLuint ShaderObj = glCreateShader(ShaderType);

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj); 

	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}

static void compileShaders() {

	//нужно поработать с шейдерами

	GLuint ShaderProgram = glCreateProgram();

	createShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
	createShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

	glLinkProgram(ShaderProgram);

	GLint Success = 0;					// проверяем на успешную линковку
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
	}

	glValidateProgram(ShaderProgram);

	glUseProgram(ShaderProgram);

	//gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");
	//assert(gScaleLocation != 0xFFFFFFFF);
	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	assert(gWorldLocation != 0xFFFFFFFF);
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Lab 2");

	glutDisplayFunc(RenderSceneCB);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	createVertexBuffer();

	////////lab 2//////////
	//сначала зададим шейдеры и uniform-переменную Scale
	compileShaders();

	//теперь нужно задать ленивую функцию
	glutIdleFunc(RenderSceneCB);

	glutMainLoop();
}