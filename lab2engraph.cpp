#define ToRadian(x) ((x) * 3.14f/ 180.0f)
#define ToDegree(x) ((x) * 180.0f / 3.14f)


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
uniform mat4 gWorld;														  \n\
                                                                              \n\
layout (location = 0) in vec3 Position;                                       \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
	gl_Position = gWorld * vec4(Position, 1.0);								  \n\
}";

//код скрипта фрагментного шейдера
static const char* pFS = "                                                    \n\
#version 330                                                                  \n\
                                                                              \n\
out vec4 FragColor;                                                           \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    FragColor = vec4(1, 0.6, 0.0, 1.0);                                     \n\
}";

GLuint VBO;
GLuint gWorldLocation;



inline glm::mat4x4 operator*(const glm::mat4x4& Right) //const
{
	glm::mat4x4 Ret;
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			Ret[i][j] = Ret[i][0] * Right[0][j] +
				Ret[i][1] * Right[1][j] +
				Ret[i][2] * Right[2][j] +
				Ret[i][3] * Right[3][j];
		}
	}
	return Ret;
}

class Pipeline
{
public:
	Pipeline()
	{
		m_scale = glm::vec3(1, 1, 1);
		m_worldPos = glm::vec3(0, 0, 0);
		m_rotateInfo = glm::vec3(0, 0, 0);

		m_transformation[0][0] = 0.0f;	m_transformation[0][1] = 0.0f;	m_transformation[0][2] = 0.0f;	m_transformation[0][3] = 0.0f;
		m_transformation[1][0] = 0.0f;	m_transformation[1][1] = 0.0f;	m_transformation[1][2] = 0.0f;	m_transformation[1][3] = 0.0f;
		m_transformation[2][0] = 0.0f;	m_transformation[2][1] = 0.0f;	m_transformation[2][2] = 0.0f;	m_transformation[2][3] = 0.0f;
		m_transformation[3][0] = 0.0f;	m_transformation[3][1] = 0.0f;	m_transformation[3][2] = 0.0f;	m_transformation[3][3] = 1.0f;
	}

	void Scale(float ScaleX, float ScaleY, float ScaleZ)
	{
		m_scale[0] = ToRadian(sqrt(1 - pow (ScaleX, 2)));	// сделали косинус
		m_scale[1] = ToRadian(ScaleY);
		m_scale[2] = ToRadian(ScaleZ);
	}

	void WorldPos(float x, float y, float z)
	{
		m_worldPos[0] = x;
		m_worldPos[1] = y;
		m_worldPos[2] = z;
	}

	void Rotate(float RotateX, float RotateY, float RotateZ)
	{
		m_rotateInfo[0] = RotateX;
		m_rotateInfo[1] = RotateY;
		m_rotateInfo[2] = RotateZ;
	}

	const glm::mat4x4* GetTrans();
	void InitScaleTransform(glm::mat4x4& m);
	void InitRotateTransform(glm::mat4x4& m);
	void InitTranslationTransform(glm::mat4x4& m);
private:
	glm::vec3 m_scale;
	glm::vec3 m_worldPos;
	glm::vec3 m_rotateInfo;
	glm::mat4x4 m_transformation;
};
void Pipeline::InitScaleTransform(glm::mat4x4& m) {
	m[0][0] = m_scale[0];	m[0][1] = 0.0f;			m[0][2] = 0.0f;			m[0][3] = 0.0f;
	m[1][0] = 0.0f;			m[1][1] = m_scale[1];	m[1][2] = 0.0f;			m[1][3] = 0.0f;
	m[2][0] = 0.0f;			m[2][1] = 0.0f;			m[2][2] = m_scale[2];	m[2][3] = 0.0f;
	m[3][0] = 0.0f;			m[3][1] = 0.0f;			m[3][2] = 0.0f;			m[3][3] = 1.0f;
}
void Pipeline::InitRotateTransform(glm::mat4x4& mm) {
	glm::mat4x4 m;
	m[0][0] = m_rotateInfo[0];						m[0][1] = -m_rotateInfo[1];						m[0][2] = 0.0f;		m[0][3] = 0.0f;
	m[1][0] = m_rotateInfo[1];						m[1][1] = m_rotateInfo[0];						m[1][2] = 0.0f;		m[1][3] = 0.0f;
	m[2][0] = 0.0f;									m[2][1] = 0.0f;									m[2][2] = 0.0f;		m[2][3] = 0.0f;
	m[3][0] = 0.0f;									m[3][1] = 0.0f;									m[3][2] = 0.0f;		m[3][3] = 1.0f;

	mm = mm * m;
}
void Pipeline::InitTranslationTransform(glm::mat4x4& m) {

	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = m_worldPos[0];
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = m_worldPos[1];
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = m_worldPos[2];
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}
const glm::mat4x4* Pipeline::GetTrans()
{
	glm::mat4x4 ScaleTrans, RotateTrans, TranslationTrans = glm::mat4x4 {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1};
	
	InitScaleTransform(ScaleTrans);
	InitRotateTransform(RotateTrans);
	InitTranslationTransform(TranslationTrans);
	m_transformation = TranslationTrans * RotateTrans * ScaleTrans;
	return &m_transformation;
}


void RenderSceneCB() {

	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;
	Scale += 0.01f;

	Pipeline p;
	p.Scale(sinf(Scale * 0.1f), sinf(Scale * 0.1f), sinf(Scale * 0.1f));
	p.WorldPos(sinf(Scale), 0.0f, 0.0f);
	p.Rotate(sinf(Scale) * 90.0f, sinf(Scale) * 90.0f, sinf(Scale) * 90.0f);
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());


	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
	glutPostRedisplay(); // останавливает повторный вызов ленивой функции
}


static void createVertexBuffer() {
	glm::vec3 vecArrTrngl[3];
	vecArrTrngl[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
	vecArrTrngl[1] = glm::vec3(1.0f, -1.0f, 0.0f);
	vecArrTrngl[2] = glm::vec3(0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vecArrTrngl), vecArrTrngl, GL_STATIC_DRAW);

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