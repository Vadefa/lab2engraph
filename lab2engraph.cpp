#define ToRadian(x) ((x) * 3.14f/ 180.0f)
#define ToDegree(x) ((x) * 180.0f / 3.14f)
#define WINDOW_HEIGHT 1024
#define WINDOW_WIDTH 1024


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
    Color = vec4(clamp(Position, 0.0, 1.0), 1.0);                                   \n\
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


class Pipeline
{
public:
	Pipeline()
	{
		m_scale = glm::vec3(1, 1, 1);
		m_worldPos = glm::vec3(0, 0, 0);
		m_rotateInfo = glm::vec3(0, 0, 0);

		m_persProj.FOV = 0.0f;
		m_persProj.Width = 0.0f;
		m_persProj.Height = 0.0f;
		m_persProj.zNear = 0.0f;
		m_persProj.zFar = 0.0f;

		m_transformation[0][0] = 0.0f;	m_transformation[0][1] = 0.0f;	m_transformation[0][2] = 0.0f;	m_transformation[0][3] = 0.0f;
		m_transformation[1][0] = 0.0f;	m_transformation[1][1] = 0.0f;	m_transformation[1][2] = 0.0f;	m_transformation[1][3] = 0.0f;
		m_transformation[2][0] = 0.0f;	m_transformation[2][1] = 0.0f;	m_transformation[2][2] = 0.0f;	m_transformation[2][3] = 0.0f;
		m_transformation[3][0] = 0.0f;	m_transformation[3][1] = 0.0f;	m_transformation[3][2] = 0.0f;	m_transformation[3][3] = 1.0f;
	}

	void WorldPos(float x, float y, float z)
	{
		m_worldPos[0] = x;
		m_worldPos[1] = y;
		m_worldPos[2] = z;
	}
	void Rotate(float RotateX, float RotateY, float RotateZ)
	{
		//m_rotateInfo[0] = RotateX;
		//m_rotateInfo[1] = RotateY;
		//m_rotateInfo[2] = RotateZ;

		m_rotateInfo[0] = ToRadian(RotateX);		// синус
		m_rotateInfo[1] = ToRadian(RotateY);		// минус синус
		m_rotateInfo[2] = ToRadian(RotateZ);		// косинус
	}
	void Scale(float ScaleX, float ScaleY, float ScaleZ)
	{
		m_scale[0] = ToRadian(ScaleX);
		m_scale[1] = ToRadian(ScaleY);				// Y должен умножаться на косинус, так что в кач-ве ScaleY нужно передать косинус
		m_scale[2] = ToRadian(ScaleZ);
	}
	void SetPerspectiveProj(float FOV, float Width, float Height, float zNear, float zFar) {
		m_persProj.FOV = FOV;
		m_persProj.Width = Width;
		m_persProj.Height = Height;
		m_persProj.zNear = zNear;
		m_persProj.zFar = zFar;
	}


	const glm::mat4x4* GetTrans();
	void InitScaleTransform(glm::mat4x4& m);
	void InitRotateTransform(glm::mat4x4& m);
	void InitTranslationTransform(glm::mat4x4& m);
	void InitPerspectiveProj(glm::mat4x4& m) const;

private:
	glm::vec3 m_scale;
	glm::vec3 m_worldPos;
	glm::vec3 m_rotateInfo;
	glm::mat4x4 m_transformation;
	struct {
		float FOV;
		float Width;
		float Height;
		float zNear;
		float zFar;
	}m_persProj;
};
void Pipeline::InitTranslationTransform(glm::mat4x4& m) {

	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = m_worldPos[0];
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = m_worldPos[1];
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = m_worldPos[2];
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}
void Pipeline::InitRotateTransform(glm::mat4x4& m) {

	m[0][0] = m_rotateInfo[2];						m[0][1] = m_rotateInfo[1];						m[0][2] = 0.0f;		m[0][3] = 0.0f;
	m[1][0] = m_rotateInfo[0];						m[1][1] = m_rotateInfo[0];						m[1][2] = 0.0f;		m[1][3] = 0.0f;
	m[2][0] = 0.0f;									m[2][1] = 0.0f;									m[2][2] = 1.0f;		m[2][3] = 0.0f;
	m[3][0] = 0.0f;									m[3][1] = 0.0f;									m[3][2] = 0.0f;		m[3][3] = 1.0f;

}
void Pipeline::InitScaleTransform(glm::mat4x4& m) {
	m[0][0] = m_scale[0];	m[0][1] = 0.0f;			m[0][2] = 0.0f;			m[0][3] = 0.0f;
	m[1][0] = 0.0f;			m[1][1] = m_scale[1];	m[1][2] = 0.0f;			m[1][3] = 0.0f;
	m[2][0] = 0.0f;			m[2][1] = 0.0f;			m[2][2] = m_scale[2];	m[2][3] = 0.0f;
	m[3][0] = 0.0f;			m[3][1] = 0.0f;			m[3][2] = 0.0f;			m[3][3] = 1.0f;
}
void Pipeline::InitPerspectiveProj(glm::mat4x4& m) const
{
	const float ar = m_persProj.Width / m_persProj.Height;		//в структуре есть переменные ширины и высоты камеры
	const float zNear = m_persProj.zNear;						//также близости
	const float zFar = m_persProj.zFar;							//и дальности
	const float zRange = zNear - zFar;
	const float tanHalfFOV = tanf(ToRadian(m_persProj.FOV / 2.0));	//а также поля обзора

	m[0][0] = 1.0f / (tanHalfFOV * ar);
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f / tanHalfFOV;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = (-zNear - zFar) / zRange;
	m[2][3] = 2.0f * zFar * zNear / zRange;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 1.0f;
	m[3][3] = 0.0f;
}

const glm::mat4x4* Pipeline::GetTrans()
{
	glm::mat4x4 PersProjTrans, ScaleTrans, RotateTrans, TranslationTrans = glm::mat4x4{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

	InitPerspectiveProj(PersProjTrans);
	InitScaleTransform(ScaleTrans);
	InitRotateTransform(RotateTrans);
	InitTranslationTransform(TranslationTrans);
	m_transformation = PersProjTrans * TranslationTrans * RotateTrans * ScaleTrans;
	return &m_transformation;
}


void RenderSceneCB() {

	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0001f;
	Scale += 0.01f;

	Pipeline p;
	p.SetPerspectiveProj(30.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 10.0f, 10000.0f);
	p.Scale(sinf(Scale * 0.1f), cosf(Scale * 0.1f), sinf(Scale * 0.1f));
	p.WorldPos(sinf(Scale), 0.0f, 0.0f);
	p.Rotate(sinf(Scale) * 90.0f, -sinf(Scale) * 90.0f, cosf(Scale) * 90.0f);
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());


	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
	glutPostRedisplay(); // останавливает повторный вызов ленивой функции
}


static void createVertexBuffer() {
	glm::vec3 vecArrTrngl[4];
	vecArrTrngl[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
	vecArrTrngl[1] = glm::vec3(0.0f, -1.0f, 1.0f);
	vecArrTrngl[2] = glm::vec3(1.0f, -1.0f, 0.0f);
	vecArrTrngl[3] = glm::vec3(0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vecArrTrngl), vecArrTrngl, GL_STATIC_DRAW);

	unsigned int Indices[] = { 0, 3, 1,
								1, 3, 2,
								2, 3, 0,
								0, 2, 1 };

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

}

static void createShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
	
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

	GLuint ShaderProgram = glCreateProgram();

	createShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
	createShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

	glLinkProgram(ShaderProgram);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
	}

	glValidateProgram(ShaderProgram);

	glUseProgram(ShaderProgram);

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

	glClearColor(0.0f, 0.2f, 0.0f, 0.0f);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	createVertexBuffer();

	compileShaders();

	glutIdleFunc(RenderSceneCB);

	glutMainLoop();
}