#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void RenderSceneCB() {

	glClear(GL_COLOR_BUFFER_BIT);


	glutSwapBuffers();

	glutPostRedisplay(); // останавливает повторный вызов ленивой функции
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