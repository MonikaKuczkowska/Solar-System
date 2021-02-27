#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <ctime>

#include "Texture.h"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"

#include "Box.cpp"

#define SIZE 1

std::vector <glm::vec3> spaceMenPositions;

GLuint program;
GLuint programSun;
GLuint programTex;
GLuint programTexSun;
GLuint programSkybox;
GLuint programColor;

GLuint CubemapTexture;
GLuint SpaceManTexture;
GLuint SpaceManTextureN;

GLuint textureShip;
GLuint textureStar;
GLuint textureEarth;
GLuint textureMoon;
GLuint textureSun;
GLuint textureMercury;
GLuint textureVenus;
GLuint textureMars;
GLuint textureJupiter;
GLuint textureSaturn;
GLuint textureUranus;
GLuint textureNeptune;

GLuint textureShipN;
GLuint textureEarthN;
GLuint textureMoonN;
GLuint textureSunN;
GLuint textureMercuryN;
GLuint textureVenusN;
GLuint textureMarsN;
GLuint textureJupiterN;
GLuint textureSaturnN;
GLuint textureUranusN;
GLuint textureNeptuneN;

Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;
obj::Model spaceManModel;

float frustumScale;
float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-25, 0, 0);
glm::vec3 cameraDir;
glm::vec3 cameraSide;
glm::vec3 lightPos = glm::vec3(0, 0, 0);
glm::vec3 lightPos2 = glm::vec3(60, 20, 40);

glm::mat4 cameraMatrix, perspectiveMatrix;

float lastX = -1;
float lastY = -1;
float xoffset, yoffset;
bool firstMouse = true;
glm::quat rotation = glm::quat(1, 0, 0, 0);
glm::quat rotX = glm::normalize(glm::angleAxis(307 * 0.03f, glm::vec3(0, 1, 0)));
glm::quat rotY = glm::normalize(glm::angleAxis(209 * 0.03f, glm::vec3(1, 0, 0)));

float cubemapVertices[] = {       
	-SIZE,  SIZE, -SIZE,
	-SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	-SIZE,  SIZE, -SIZE,
	SIZE,  SIZE, -SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	SIZE, -SIZE,  SIZE
};

GLuint SkyboxVertexBuffer;
GLuint SkyboxVertexAttributes;

void keyboard(unsigned char key, int x, int y)
{

	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch (key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += cameraSide * moveSpeed; break;
	case 'a': cameraPos -= cameraSide * moveSpeed; break;
	}
}

void mouse(int x, int y)
{ 
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	xoffset = x - lastX;
	yoffset = y - lastY;
	lastX = x;
	lastY = y;

	float sens = 0.03f;
	xoffset *= sens;
	yoffset *= sens;


	/*
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	if (x < 10 || y < 10 || x >(width - 10) || y >(height - 10)) {
		lastX = width / 2;
		lastY = height / 2;
		glutWarpPointer(lastX, lastY);
	} 
	*/
}

glm::mat4 createCameraMatrix()
{
	glm::quat quatX = glm::angleAxis(xoffset, glm::vec3(0, 1, 0));
	glm::quat quatY = glm::angleAxis(yoffset, glm::vec3(1, 0, 0));

	xoffset = 0;
	yoffset = 0;

	rotX = glm::normalize(quatX * rotX);
	rotY = glm::normalize(quatY * rotY);

	rotation = glm::normalize(rotY * rotX);
	cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

	glm::mat4 cameraTranslation;
	cameraTranslation[3] = glm::vec4(-cameraPos, 1.0f);

	return glm::mat4_cast(rotation) * cameraTranslation;
}

void drawObject(GLuint program, obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);

	Core::DrawModel(model);
}

void drawObjectTexture(GLuint program, obj::Model * model, glm::mat4 modelMatrix, GLuint texture, GLuint normalmapId)
{
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);

	Core::SetActiveTexture(texture, "colorTexture", program, 0);
	Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);

	Core::DrawModel(model);
}

void renderPlanets()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glUseProgram(programTex);

	glUniform3f(glGetUniformLocation(programTex, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(programTex, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
	glUniform3f(glGetUniformLocation(programTex, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 sun = glm::translate(glm::vec3(0, 0, 0));
	glm::vec3 axis = glm::vec3(0, 1, 0);

	//Merkury
	glm::mat4 mercuryModelMatrix = sun * glm::rotate(glm::radians(time * 23.9f), axis) * glm::translate(glm::vec3(6, 0, 0)) * glm::scale(glm::vec3(0.2f))
		* glm::rotate(glm::radians(time * 1.0f), axis);
	drawObjectTexture(programTex, &sphereModel, mercuryModelMatrix, textureMercury, textureMercuryN);
	//Venus
	glm::mat4 venusModelMatrix = sun * glm::rotate(glm::radians(time * 17.0f), axis) * glm::translate(glm::vec3(10, 0, 0)) * glm::scale(glm::vec3(0.6f))
		* glm::rotate(glm::radians(time * 0.6f), axis);
	drawObjectTexture(programTex, &sphereModel, venusModelMatrix, textureVenus, textureVenusN);
	//Ziemia
	glm::mat4 earthModelMatrix = sun * glm::rotate(glm::radians(time * 15.8f), axis) * glm::translate(glm::vec3(15, 0, 0)) * glm::scale(glm::vec3(0.6f))
		* glm::rotate(glm::radians(time * 167.4f), axis);
	drawObjectTexture(programTex, &sphereModel, earthModelMatrix, textureEarth, textureEarthN);
	//Ksiê¿yc
	glm::mat4 moonModelMatrix = earthModelMatrix * glm::rotate(glm::radians(time * 1.0f), axis) * glm::translate(glm::vec3(2, 0, 0)) * glm::scale(glm::vec3(0.2f));
	drawObjectTexture(programTex, &sphereModel, moonModelMatrix, textureMoon, textureMoonN);
	//Mars
	glm::mat4 marsModelMatrix = sun * glm::rotate(glm::radians(time * 12.1f), axis) * glm::translate(glm::vec3(20, 0, 0)) * glm::scale(glm::vec3(0.3f))
		* glm::rotate(glm::radians(time * 86.6f), axis);
	drawObjectTexture(programTex, &sphereModel, marsModelMatrix, textureMars, textureMarsN);
	//Jowisz
	glm::mat4 jupiterhModelMatrix = sun * glm::rotate(glm::radians(time * 6.1f), axis) * glm::translate(glm::vec3(30, 0, 0)) * glm::scale(glm::vec3(3.5f))
		* glm::rotate(glm::radians(time * 455.8f / 4), axis);
	drawObjectTexture(programTex, &sphereModel, jupiterhModelMatrix, textureJupiter, textureJupiterN);
	//Saturn
	glm::mat4 saturnModelMatrix = sun * glm::rotate(glm::radians(time * 4.7f), axis) * glm::translate(glm::vec3(40, 0, 0)) * glm::scale(glm::vec3(3.0f))
		* glm::rotate(glm::radians(time * 368.4f), axis);
	drawObjectTexture(programTex, &sphereModel, saturnModelMatrix, textureSaturn, textureSaturnN);
	//Uran
	glm::mat4 uranusModelMatrix = sun * glm::rotate(glm::radians(time * 3.8f), axis) * glm::translate(glm::vec3(50, 0, 0)) * glm::scale(glm::vec3(1.5f))
		* glm::rotate(glm::radians(time * 147.9f), axis);
	drawObjectTexture(programTex, &sphereModel, uranusModelMatrix, textureUranus, textureUranusN);
	//Neptun
	glm::mat4 neptuneModelMatrix = sun * glm::rotate(glm::radians(time * 2.4f), axis) * glm::translate(glm::vec3(70, 0, 0)) * glm::scale(glm::vec3(1.0f))
		* glm::rotate(glm::radians(time * 97.1f), axis);
	drawObjectTexture(programTex, &sphereModel, neptuneModelMatrix, textureNeptune, textureNeptuneN);
}

void renderSun()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glUseProgram(programTexSun);

	glUniform3f(glGetUniformLocation(programTexSun, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(programTexSun, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 sunModelMatrix = glm::translate(lightPos) * glm::scale(glm::vec3(4.0f)) * glm::rotate(glm::radians(time * 2.0f), glm::vec3(0, 1, 0));
	drawObjectTexture(programTexSun, &sphereModel, sunModelMatrix, textureSun, textureSunN);
}

void renderStar()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glUseProgram(programTexSun);

	glUniform3f(glGetUniformLocation(programTexSun, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(programTexSun, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 starModelMatrix = glm::translate(lightPos2) * glm::scale(glm::vec3(3.0f)) * glm::rotate(glm::radians(time * 2.0f), glm::vec3(0, 1, 0));
	drawObjectTexture(programTexSun, &sphereModel, starModelMatrix, textureStar, textureSunN);
}

void renderShip()
{
	glUseProgram(programTex);

	glUniform3f(glGetUniformLocation(programTex, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(programTex, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
	glUniform3f(glGetUniformLocation(programTex, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	/* glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0, -0.25f, 0)) * 
		glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0, 1, 0)) * 
		glm::scale(glm::vec3(0.25f)); */
	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(0, -0.25f, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(rotation)) * shipInitialTransformation;

	drawObjectTexture(programTex, &shipModel, shipModelMatrix, textureShip, textureShipN);
}

void renderSpaceMen(glm::vec3 position)
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;
	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 spaceManModelMatrix = glm::translate(position) * glm::scale(glm::vec3(0.001f)) * glm::rotate(glm::radians(time * 2.0f), glm::vec3(0, 1, 0));

	drawObject(program, &spaceManModel, spaceManModelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
}

void renderSkybox()
{
	glUseProgram(programSkybox);
	glm::mat4 view = glm::mat4(glm::mat3(cameraMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "view"), 1, GL_FALSE, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "perspective"), 1, GL_FALSE, (float*)&perspectiveMatrix);

	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(SkyboxVertexAttributes);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}

glm::vec3 randomVec() {
	int x, y, z;
	x = rand() % 81 + (-40);
	y = rand() % 81 + (-40);
	z = rand() % 81 + (-40);	
	return glm::vec3(x, y, z);
}

void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix(0.1f, 100.f, frustumScale);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	renderShip();
	renderPlanets();
	renderSun();
	renderStar();
	for (int i = 0; i < spaceMenPositions.size(); i++)
	{
		renderSpaceMen(spaceMenPositions[i]);
	}
	renderSkybox();
	for (int i = 0; i < spaceMenPositions.size(); i++)
	{
		if (glm::distance(cameraPos, spaceMenPositions[i]) < 1.0) {
			spaceMenPositions.erase(spaceMenPositions.begin() + i);
		}
	}
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
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	spaceManModel = obj::loadModelFromFile("models/among us.obj");

	SpaceManTexture = Core::LoadTexture("textures/Plastic_4K_Diffuse.png");
	SpaceManTextureN = Core::LoadTexture("textures/Plastic_4K_Normal.png");

	textureShip = Core::LoadTexture("textures/spaceship.png");
	textureStar = Core::LoadTexture("textures/star.png");
	textureEarth = Core::LoadTexture("textures/earth2.png");
	textureMoon = Core::LoadTexture("textures/moon.png");
	textureSun = Core::LoadTexture("textures/sun.png");
	textureMercury = Core::LoadTexture("textures/mercury.png");
	textureVenus = Core::LoadTexture("textures/venus.png");
	textureMars = Core::LoadTexture("textures/mars.png");
	textureJupiter = Core::LoadTexture("textures/jupiter.png");
	textureSaturn = Core::LoadTexture("textures/saturn.png");
	textureUranus = Core::LoadTexture("textures/uranus.png");
	textureNeptune = Core::LoadTexture("textures/neptune.png");

	textureShipN = Core::LoadTexture("textures/spaceship_normals.png");
	textureEarthN = Core::LoadTexture("textures/earth2_normals.png");
	textureMoonN = Core::LoadTexture("textures/moon_normals.png");
	textureSunN = Core::LoadTexture("textures/sun_normals.png");
	textureMercuryN = Core::LoadTexture("textures/mercury_normals.png");
	textureVenusN = Core::LoadTexture("textures/venus_normals.png");
	textureMarsN = Core::LoadTexture("textures/mars_normals.png");
	textureJupiterN = Core::LoadTexture("textures/jupiter_normals.png");
	textureSaturnN = Core::LoadTexture("textures/saturn_normals.png");
	textureUranusN = Core::LoadTexture("textures/uranus_normals.png");
	textureNeptuneN = Core::LoadTexture("textures/neptune_normals.png");

	std::vector<std::string> faces
	{
		"textures/right.png",
		"textures/left.png",
		"textures/top.png",
		"textures/bottom.png",
		"textures/back.png",
		"textures/front.png"
	};
	CubemapTexture = Core::LoadCubemap(faces);

	glGenBuffers(1, &SkyboxVertexBuffer);
	glGenVertexArrays(1, &SkyboxVertexAttributes);
	glBindVertexArray(SkyboxVertexAttributes);
	glBindBuffer(GL_ARRAY_BUFFER, SkyboxVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void shutdown()
{
	shaderLoader.DeleteProgram(program);
}

void idle()
{
	glutPostRedisplay();
}

void onReshape(int width, int height)
{
	frustumScale = (float)width / height;

	glViewport(0, 0, width, height);
}

int main(int argc, char ** argv)
{
	srand((int)time(0));
	for (int i = 0; i < 10; i++)
	{
		spaceMenPositions.push_back(randomVec());
	}
	glutInit(&argc, argv);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glEnable(GL_MULTISAMPLE);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Solar System");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);
	glutReshapeFunc(onReshape);

	glutMainLoop();

	shutdown();

	return 0;
}
