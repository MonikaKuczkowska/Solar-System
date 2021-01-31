#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Texture.h"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"

#include "Box.cpp"

GLuint program;
GLuint programSun;
GLuint programTex;
GLuint programTexSun;


GLuint texture;
GLuint textureMoon;
GLuint textureSun;
GLuint textureMercury;
GLuint textureVenus;
GLuint textureMars;
GLuint textureJupiter;
GLuint textureSaturn;
GLuint textureUranus;
GLuint textureNeptune;


Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;
Core::RenderContext shipContext;
Core::RenderContext sphereContext;


float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-5, 0, 0);
glm::vec3 cameraDir;
glm::vec3 lightPos = glm::vec3(0, 0, 0);

glm::mat4 cameraMatrix, perspectiveMatrix;

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch(key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'e': cameraPos += glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	case 'q': cameraPos -= glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	}
}

glm::mat4 createCameraMatrix()
{
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0,1,0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void drawObject(GLuint program, Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color)
{
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);

	Core::DrawContext(context);
}

void drawObjectTexture(GLuint program, Core::RenderContext context, glm::mat4 modelMatrix, GLuint texture)
{
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);

	Core::SetActiveTexture(texture, "colorTexture", program, 0);

	Core::DrawContext(context);
}

void renderPlanets()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glUseProgram(programTex);

	glUniform3f(glGetUniformLocation(programSun, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(programSun, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 sun = glm::translate(glm::vec3(0, 0, 0));
	glm::vec3 axis = glm::vec3(0, 1, 0);

	//Merkury
	glm::mat4 mercuryModelMatrix = sun * glm::rotate(glm::radians(time * 47.9f), axis) * glm::translate(glm::vec3(6, 0, 0)) * glm::scale(glm::vec3(0.2f))
		* glm::rotate(glm::radians(time * 1.0f), axis);
	drawObjectTexture(programTex, sphereContext, mercuryModelMatrix, textureMercury);
	//Venus
	glm::mat4 venusModelMatrix = sun * glm::rotate(glm::radians(time * 35.0f), axis) * glm::translate(glm::vec3(10, 0, 0)) * glm::scale(glm::vec3(0.6f))
		* glm::rotate(glm::radians(time * 0.6f), axis);
	drawObjectTexture(programTex, sphereContext, venusModelMatrix, textureVenus);
	//Ziemia
	glm::mat4 earthModelMatrix = sun * glm::rotate(glm::radians(time * 29.8f), axis) * glm::translate(glm::vec3(15, 0, 0)) * glm::scale(glm::vec3(0.6f))
		* glm::rotate(glm::radians(time * 167.4f), axis);
	drawObjectTexture(programTex, sphereContext, earthModelMatrix, texture);
	//Ksiê¿yc
	glm::mat4 moonModelMatrix = earthModelMatrix * glm::rotate(glm::radians(time * 1.0f), axis) * glm::translate(glm::vec3(2, 0, 0)) * glm::scale(glm::vec3(0.2f));
	drawObjectTexture(programTex, sphereContext, moonModelMatrix, textureMoon);
	//Mars
	glm::mat4 marsModelMatrix = sun * glm::rotate(glm::radians(time * 24.1f), axis) * glm::translate(glm::vec3(20, 0, 0)) * glm::scale(glm::vec3(0.3f))
		* glm::rotate(glm::radians(time * 86.6f), axis);
	drawObjectTexture(programTex, sphereContext, marsModelMatrix, textureMars);
	//Jowisz
	glm::mat4 jupiterhModelMatrix = sun * glm::rotate(glm::radians(time * 13.1f), axis) * glm::translate(glm::vec3(30, 0, 0)) * glm::scale(glm::vec3(3.5f))
		* glm::rotate(glm::radians(time * 455.8f), axis);
	drawObjectTexture(programTex, sphereContext, jupiterhModelMatrix, textureJupiter);
	//Saturn
	glm::mat4 saturnModelMatrix = sun * glm::rotate(glm::radians(time * 9.7f), axis) * glm::translate(glm::vec3(40, 0, 0)) * glm::scale(glm::vec3(3.0f))
		* glm::rotate(glm::radians(time * 368.4f), axis);
	drawObjectTexture(programTex, sphereContext, saturnModelMatrix, textureSaturn);
	//Uran
	glm::mat4 uranusModelMatrix = sun * glm::rotate(glm::radians(time * 6.8f), axis) * glm::translate(glm::vec3(50, 0, 0)) * glm::scale(glm::vec3(1.5f))
		* glm::rotate(glm::radians(time * 147.9f), axis);
	drawObjectTexture(programTex, sphereContext, uranusModelMatrix, textureUranus);
	//Neptun
	glm::mat4 neptuneModelMatrix = sun * glm::rotate(glm::radians(time * 5.4f), axis) * glm::translate(glm::vec3(70, 0, 0)) * glm::scale(glm::vec3(1.0f))
		* glm::rotate(glm::radians(time * 97.1f), axis);
	drawObjectTexture(programTex, sphereContext, neptuneModelMatrix, textureNeptune);
}

void renderSun()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glUseProgram(programTexSun);

	glUniform3f(glGetUniformLocation(programSun, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(programSun, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 sunModelMatrix = glm::translate(lightPos) * glm::scale(glm::vec3(4.0f)) * glm::rotate(glm::radians(time * 2.0f), glm::vec3(0, 1, 0));
	drawObjectTexture(programTexSun, sphereContext, sunModelMatrix, textureSun);
}

void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	float time = glutGet(GLUT_ELAPSED_TIME)/1000.f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(program);

	// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0,-0.25f,0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.25f));

	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	drawObject(program, shipContext, shipModelMatrix, glm::vec3(0.6f));

	renderPlanets();
	renderSun();

	glUseProgram(0);
	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_4_1.vert", "shaders/shader_4_1.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_4_2.vert", "shaders/shader_4_2.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_4_tex.vert", "shaders/shader_4_tex.frag");
	programTexSun = shaderLoader.CreateProgram("shaders/shader_sun_tex.vert", "shaders/shader_sun_tex.frag");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");

	texture = Core::LoadTexture("textures/earth2.png");
	textureMoon = Core::LoadTexture("textures/moon.png");
	textureSun = Core::LoadTexture("textures/sun.png");

	textureMercury = Core::LoadTexture("textures/mercury.png");
	textureVenus = Core::LoadTexture("textures/venus.png");
	textureMars = Core::LoadTexture("textures/mars.png");
	textureJupiter = Core::LoadTexture("textures/jupiter.png");
	textureSaturn = Core::LoadTexture("textures/saturn.png");
	textureUranus = Core::LoadTexture("textures/uranus.png");
	textureNeptune = Core::LoadTexture("textures/neptune.png");


	shipContext.initFromOBJ(shipModel);
	sphereContext.initFromOBJ(sphereModel);
}

void shutdown()
{
	shaderLoader.DeleteProgram(program);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Pierwszy Program");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
