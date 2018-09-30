// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* g_pWindow;
unsigned int g_nWidth = 1024, g_nHeight = 768;

// Include AntTweakBar
#include <AntTweakBar.h>
TwBar *g_pToolBar;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>
#include<glm/gtx/spline.hpp>

using namespace glm;

#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>
#include <glerror.hpp>

#include "Model.hpp"
#include "Transformations.h"
#include "KeyboardHandles.h"



void draw(
	std::vector<Model> &my_models,
	int nUseMouse, int nbFrames, double lastTime,
	GLuint MatrixID, GLuint ViewMatrixID, GLuint ModelMatrixID, GLuint LightID, GLuint Texture, GLuint TextureID, GLuint programID
);


void WindowSizeCallBack(GLFWwindow *pWindow, int nWidth, int nHeight) {

	g_nWidth = nWidth;
	g_nHeight = nHeight;
	glViewport(0, 0, g_nWidth, g_nHeight);
	TwWindowSize(g_nWidth, g_nHeight);
}

int main(void)
{
	int nUseMouse = 0;

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	g_pWindow = glfwCreateWindow(g_nWidth, g_nHeight, "CG UFPel", NULL, NULL);
	if (g_pWindow == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(g_pWindow);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	check_gl_error();//OpenGL error from GLEW

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(g_nWidth, g_nHeight);

	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetMouseButtonCallback(g_pWindow, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetCursorPosCallback(g_pWindow, (GLFWcursorposfun)TwEventMousePosGLFW);          // - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetScrollCallback(g_pWindow, (GLFWscrollfun)TwEventMouseWheelGLFW);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetKeyCallback(g_pWindow, (GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
	glfwSetCharCallback(g_pWindow, (GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar
	glfwSetWindowSizeCallback(g_pWindow, WindowSizeCallBack);

	//create the toolbar
	g_pToolBar = TwNewBar("CG UFPel ToolBar");
	// Add 'speed' to 'bar': it is a modifable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [s] and [S].
	double speed = 0.0;
	TwAddVarRW(g_pToolBar, "speed", TW_TYPE_DOUBLE, &speed, " label='Rot speed' min=0 max=2 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");
	// Add 'bgColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR3F (3 floats color)
	vec3 oColor(0.0f);
	TwAddVarRW(g_pToolBar, "bgColor", TW_TYPE_COLOR3F, &oColor[0], " label='Background color' ");

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(g_pWindow, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(g_pWindow, g_nWidth / 2, g_nHeight / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("shaders/StandardShading.vertexshader", "shaders/StandardShading.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	GLuint Texture = loadDDS("mesh/uvmap.DDS");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");



	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	std::vector<Model> my_models;
	
	//creates examples
	Model * su = new Model("mesh/cube.obj", glm::vec3(6, 0, 0));
	Model * su2 = new Model("mesh/suzanne.obj", glm::vec3(0, 0, 0));
	Model * su3 = new Model("mesh/suzanne.obj", glm::vec3(-4, 0, 0));

	su->anim_init_time = glfwGetTime(); su->translate = false; su->rotate = false; su->isExample = true; su->rotate_about = true;
	su->finalPos = glm::vec3(6, 0, 0);
	
	su2->isExample = true; su2->rotate = true; su2->translate = true;
	su2->anim_init_time = glfwGetTime();

	su3->isExample = true; su3->scaling = true; su3->scale = 1;

	my_models.push_back(*su);
	my_models.push_back(*su2);
	my_models.push_back(*su3);
	int selected_model = 0;
	do {

		//printf("My model size: %uz", my_models.size());

		handle_input(&selected_model, my_models, lastTime);
		printf("Current model: %d\n", selected_model);
		


		check_gl_error();

		//use the control key to free the mouse
		if (glfwGetKey(g_pWindow, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
			nUseMouse = 1;
		else
			nUseMouse = 0;

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
			// printf and reset
			//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		draw(my_models,nUseMouse, nbFrames, lastTime, MatrixID, ViewMatrixID, ModelMatrixID,
			LightID, Texture, TextureID, programID);



		// Draw tweak bars
		TwDraw();

		// Swap buffers
		glfwSwapBuffers(g_pWindow);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(g_pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(g_pWindow) == 0);


	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Terminate AntTweakBar and GLFW
	TwTerminate();
	glfwTerminate();

	return 0;
}


void draw(
	std::vector<Model> &my_models,
	int nUseMouse, int nbFrames, double lastTime,
	GLuint MatrixID, GLuint ViewMatrixID, GLuint ModelMatrixID, GLuint LightID, GLuint Texture, GLuint TextureID, GLuint programID
) {
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	for (int i = 0; i < my_models.size(); ++i) {

		double currentTime = glfwGetTime();

		if (currentTime - my_models[i].anim_init_time >= 0.005) {
			//my_models[i].anim_init_time = currentTime;

			if (my_models[i].translate == true)  //THIS IS A TRANSLATION
			{
				if (currentTime - my_models[i].trans_init_time >= 3.0 && my_models[i].isExample == true) {
					my_models[i].trans_init_time = currentTime;
					if (my_models[i].dir == Model::Direction::LEFT)
						my_models[i].dir = Model::Direction::RIGHT;
					else if (my_models[i].dir == Model::Direction::RIGHT)
						my_models[i].dir = Model::Direction::LEFT;
					else if(my_models[i].dir == Model::Direction::UP)
						my_models[i].dir = Model::Direction::DOWN;
					else
						my_models[i].dir = Model::Direction::UP;
					translate_model(my_models[i]);
				}
				else{
					translate_model(my_models[i]);
					if(!my_models[i].isExample)
						my_models[i].translate = false;
				}
			}
			if (my_models[i].rotate == true) {  //THIS IS A ROTATION
				if(!my_models[i].isExample) my_models[i].rotate = false;
				if (my_models[i].rot_axis == Model::Axis::X) { //rotation around X axis
					my_models[i].modelMatrix = glm::rotate(my_models[i].modelMatrix, (float)my_models[i].angle, glm::vec3(1, 0, 0));
				}
				if (my_models[i].rot_axis == Model::Axis::Y) { //rotation around Y axis
					my_models[i].modelMatrix = glm::rotate(my_models[i].modelMatrix, (float)my_models[i].angle, glm::vec3(0, 1, 0));
				}
				if (my_models[i].rot_axis == Model::Axis::Z) { //rotation around Z axis
					my_models[i].modelMatrix = glm::rotate(my_models[i].modelMatrix, (float)my_models[i].angle, glm::vec3(0, 0, 1));
				}
			}
			if (my_models[i].scaling == true) { // THIS IS A SCALE
				if(!my_models[i].isExample)
					my_models[i].scaling = false;
				if (currentTime - my_models[i].trans_init_time >= 3.0 && my_models[i].isExample == true) {
					my_models[i].trans_init_time = currentTime;
					my_models[i].scale == 1 ? my_models[i].scale = 0 : my_models[i].scale = 1;
				}
				if (my_models[i].scale == 1)
					my_models[i].modelMatrix = glm::scale(my_models[i].modelMatrix, glm::vec3(1.001f, 1.001f, 1.001f));
				else
					my_models[i].modelMatrix = glm::scale(my_models[i].modelMatrix, glm::vec3(0.999f, 0.999f, 0.999f));

				
			}
			if (my_models[i].shearing == true) { // THIS IS A SHEAR
				if (!my_models[i].isExample)
					my_models[i].shearing = false;
				if (my_models[i].shearing_axis == 0) {
					my_models[i].modelMatrix = glm::shearX3D(my_models[i].modelMatrix,0.01f, 0.01f);
				}
				else if (my_models[i].shearing_axis == 1) {
					my_models[i].modelMatrix = glm::shearX3D(my_models[i].modelMatrix, -0.01f, -0.01f);
				}
				else if (my_models[i].shearing_axis == 2) {
					my_models[i].modelMatrix = glm::shearY3D(my_models[i].modelMatrix, 0.01f, 0.01f);
				}
				else{
					my_models[i].modelMatrix = glm::shearY3D(my_models[i].modelMatrix, -0.01f, -0.01f);
				}
			}

			if (my_models[i].rotate_about == 1) {	//ROTATE AROUND POINT
				if(!my_models[i].isExample)
					my_models[i].rotate_about = 0;
				rotate_around_point(my_models[i], vec3(0,0,0)); //rotates around given point, this case origin
			}

			if (my_models[i].bezier == true && my_models[i].t_bezier < 1) //WALKS IN BEZIER CURVE to the right
			{
				//makes a bezier curve starting at models position and targets models position +3 in x-axis
				//with middle point being +3 in y-axis

				vec2 points[3];
				points[0] = my_models[i].start_pos;
				points[1] = vec2(my_models[i].start_pos[0], 5);
				points[2] = vec2(my_models[i].start_pos[0] + 3, 0);  
				vec2 ans = getBezierPoint(points, 3, my_models[i].t_bezier);
				my_models[i].t_bezier += 0.01f;
				my_models[i].modelMatrix[3][0] = ans[0];
				my_models[i].modelMatrix[3][1] = ans[1];
			}
			else if (my_models[i].bspline == true && my_models[i].t_spline < 5) {  //WALKS IN BSPLINE CURVE to the right
				
				vec3 points[4], ans;
				
				points[0] = vec3(my_models[i].start_pos[0], my_models[i].start_pos[1],0);  //start point
				points[1] = vec3(my_models[i].start_pos[0], 5,0);  //midpoint1
				points[2] = vec3(my_models[i].start_pos[0] + 3, 7,0); //midpoint2
				points[3] = vec3(my_models[i].start_pos[0] + 5, 0,0);  //target point
				std::vector<glm::vec3> cp;
				cp.push_back(points[0]);
				cp.push_back(points[1]);
				cp.push_back(points[2]);
				cp.push_back(points[3]);

				//glm::vec3 catmull_rom_spline(const std::vector<glm::vec3>& cp, float t)
				//ans = catmull_rom_spline(cp, my_models[i].t_spline);
				ans = cubic_spline(cp, my_models[i].t_spline);
				
				my_models[i].modelMatrix[3][0] = ans[0];
				my_models[i].modelMatrix[3][1] = ans[1];

				my_models[i].t_spline += 0.01f;

			}
			else if (my_models[i].catmull == true && my_models[i].t_catmull < 5) {  //WALKS IN BSPLINE CURVE to the right

				vec3 points[4], ans;

				points[0] = vec3(my_models[i].start_pos[0], my_models[i].start_pos[1], 0);  //start point
				points[1] = vec3(my_models[i].start_pos[0], 5, 0);  //midpoint1
				points[2] = vec3(my_models[i].start_pos[0] + 3, 7, 0); //midpoint2
				points[3] = vec3(my_models[i].start_pos[0] + 5, 0, 0);  //target point
				std::vector<glm::vec3> cp;
				cp.push_back(points[0]);
				cp.push_back(points[1]);
				cp.push_back(points[2]);
				cp.push_back(points[3]);

				//glm::vec3 catmull_rom_spline(const std::vector<glm::vec3>& cp, float t)
				ans = catmull_rom_spline(cp, my_models[i].t_spline);
				//ans = cubic_spline(cp, my_models[i].t_spline);

				my_models[i].modelMatrix[3][0] = ans[0];
				my_models[i].modelMatrix[3][1] = ans[1];

				my_models[i].t_spline += 0.01f;

			}
		}



		

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs(nUseMouse, g_nWidth, g_nHeight);
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		//glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 ModelMatrix = my_models[i].modelMatrix;
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, my_models[i].vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, my_models[i].uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, my_models[i].normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_models[i].elementbuffer);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,        // mode
			(GLsizei)my_models[i].indices.size(),      // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0             // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

	}
}

