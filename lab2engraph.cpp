#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//код скрипта вершинного шейдера:
static const char* pVS = "                                                    \n\
#version 330                                                                  \n\
                                                                              \n\
layout (location = 0) in vec3 Position;                                       \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    gl_Position = vec4(0.5 * Position.x, 0.5 * Position.y, Position.z, 1.0);  \n\
}";

//код скрипта фрагментного шейдера
static const char* pFS = "                                                    \n\
#version 330                                                                  \n\
                                                                              \n\
out vec4 FragColor;                                                           \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);                                     \n\
}";



void RenderSceneCB() {

	glClear(GL_COLOR_BUFFER_BIT);


	glutSwapBuffers();

	glutPostRedisplay(); // останавливает повторный вызов ленивой функции
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

	////////lab 2//////////

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
	
	
	// нужно сделать ленивую функцию
	glutIdleFunc(RenderSceneCB);

	// И снова у нас проблемы. Мы ещё не рассмотрели функции шейдеров, поэтому у нас их нет. Вернёмся к уроку 4.
	/*gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");
	assert(gScaleLocation != 0xFFFFFFFF);*/


	struct Matrix4f {
		glm::mat4x4 m{};
	};

	GLuint gWorldLocation;

	//тут у нас происходит непонятка в sinf(Scale). Scale это uniform-переменная. Нужно вернуться к уроку 5 и рассмотреть их.
	//Matrix4f World;
	//World.m[0][0] = 1.0f; World.m[0][1] = 0.0f; World.m[0][2] = 0.0f; World.m[0][3] = sinf(Scale);
	//World.m[1][0] = 0.0f; World.m[1][1] = 1.0f; World.m[1][2] = 0.0f; World.m[1][3] = 0.0f;
	//World.m[2][0] = 0.0f; World.m[2][1] = 0.0f; World.m[2][2] = 1.0f; World.m[2][3] = 0.0f;
	//World.m[3][0] = 0.0f; World.m[3][1] = 0.0f; World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;
	
	/////////////////////////
	

	glutMainLoop();
}