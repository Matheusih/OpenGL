// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <GL/glew.h>
#include <glfw3.h>

extern GLFWwindow* g_pWindow;
extern unsigned int g_nWidth, g_nHeight;

// Include GLM
#include <glm/glm.hpp>

using namespace glm;


#include "Model.hpp"
#pragma once

void handle_input(int *selected_model, std::vector<Model> &my_models, double lastTime) {

	//inserts new models
	if (glfwGetKey(g_pWindow, GLFW_KEY_1) == GLFW_PRESS) {
		if (glfwGetKey(g_pWindow, GLFW_KEY_1) == GLFW_RELEASE) {
			Model *new_model = new Model("mesh/cube.obj", glm::vec3(1, 0, 0));
			my_models.push_back(*new_model);
		}
	}
	else if (glfwGetKey(g_pWindow, GLFW_KEY_2) == GLFW_PRESS) {
		if (glfwGetKey(g_pWindow, GLFW_KEY_2) == GLFW_RELEASE) {
			Model *new_model = new Model("mesh/goose.obj", glm::vec3(2, 0, 0));
			my_models.push_back(*new_model);
		}
	}
	else if (glfwGetKey(g_pWindow, GLFW_KEY_3) == GLFW_PRESS) {
		if (glfwGetKey(g_pWindow, GLFW_KEY_3) == GLFW_RELEASE) {
			Model *new_model = new Model("mesh/suzanne.obj", glm::vec3(3, 0, 0));
			my_models.push_back(*new_model);
		}
	}

	if (my_models.size() <= 0) return;

	double curtime = glfwGetTime();

	if (curtime - lastTime >= 1.5) return;
	//selects next model
	if (glfwGetKey(g_pWindow, GLFW_KEY_N) == GLFW_PRESS) {
		if (glfwGetKey(g_pWindow, GLFW_KEY_N) == GLFW_RELEASE)
			(*selected_model)++;

		if (*selected_model >= my_models.size()) *selected_model = 0;
	}

	//rotation
	if (glfwGetKey(g_pWindow, GLFW_KEY_F1) == GLFW_PRESS) {
		my_models[*selected_model].rotate = true;
		my_models[*selected_model].rotate_init_time = glfwGetTime();
		my_models[*selected_model].rot_axis = Model::Axis::X;
	}
	else if (glfwGetKey(g_pWindow, GLFW_KEY_F2) == GLFW_PRESS) {
		my_models[*selected_model].rotate = true;
		my_models[*selected_model].rotate_init_time = glfwGetTime();
		my_models[*selected_model].rot_axis = Model::Axis::Y;
	}
	else if (glfwGetKey(g_pWindow, GLFW_KEY_F3) == GLFW_PRESS) {
		my_models[*selected_model].rotate = true;
		my_models[*selected_model].rotate_init_time = glfwGetTime();
		my_models[*selected_model].rot_axis = Model::Axis::Z;
	}

	//shearing
	if (glfwGetKey(g_pWindow, GLFW_KEY_4) == GLFW_PRESS) {
		my_models[*selected_model].shearing = true;
		my_models[*selected_model].shearing_axis = 0;
	}
	else if (glfwGetKey(g_pWindow, GLFW_KEY_5) == GLFW_PRESS) {
		my_models[*selected_model].shearing = true;
		my_models[*selected_model].shearing_axis = 1;
	}
	if (glfwGetKey(g_pWindow, GLFW_KEY_6) == GLFW_PRESS) {
		my_models[*selected_model].shearing = true;
		my_models[*selected_model].shearing_axis = 2;
	}
	else if (glfwGetKey(g_pWindow, GLFW_KEY_7) == GLFW_PRESS) {
		my_models[*selected_model].shearing = true;
		my_models[*selected_model].shearing_axis = 3;
	}

	//scale
	if (glfwGetKey(g_pWindow, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
		my_models[*selected_model].scaling = true;
		my_models[*selected_model].scaling_init_time = glfwGetTime();
		my_models[*selected_model].scale = 1; //grows
	}
	else if (glfwGetKey(g_pWindow, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
		my_models[*selected_model].scaling = true;
		my_models[*selected_model].scaling_init_time = glfwGetTime();
		my_models[*selected_model].scale = -1; //shrinks
	}

	//Rotate around a nearby point
	if (glfwGetKey(g_pWindow, GLFW_KEY_F7) == GLFW_PRESS) {
		my_models[*selected_model].animation = 5;
		my_models[*selected_model].rotate_about = 1;
		//my_models[*selected_model].scale = 1; //grows
	}

	/***  TRANSFORMATIONS  ***/
	if (glfwGetKey(g_pWindow, GLFW_KEY_A) == GLFW_PRESS) {
		my_models[*selected_model].dir = Model::Direction::LEFT;
		my_models[*selected_model].translate = true;
		my_models[*selected_model].trans_init_time = glfwGetTime();
		translate_model(my_models[*selected_model]);
	}
	else if ((glfwGetKey(g_pWindow, GLFW_KEY_D) == GLFW_PRESS)) {
		my_models[*selected_model].dir = Model::Direction::RIGHT;
		my_models[*selected_model].translate = true;
		my_models[*selected_model].trans_init_time = glfwGetTime();
		translate_model(my_models[*selected_model]);
	}
	if ((glfwGetKey(g_pWindow, GLFW_KEY_W) == GLFW_PRESS)) {
		my_models[*selected_model].dir = Model::Direction::UP;
		my_models[*selected_model].translate = true;
		my_models[*selected_model].trans_init_time = glfwGetTime();
		translate_model(my_models[*selected_model]);
	}
	else if ((glfwGetKey(g_pWindow, GLFW_KEY_S) == GLFW_PRESS)) {
		my_models[*selected_model].dir = Model::Direction::DOWN;
		my_models[*selected_model].translate = true;
		my_models[*selected_model].trans_init_time = glfwGetTime();
		translate_model(my_models[*selected_model]);
	}
	if ((glfwGetKey(g_pWindow, GLFW_KEY_Q) == GLFW_PRESS)) {
		my_models[*selected_model].dir = Model::Direction::IN;
		my_models[*selected_model].translate = true;
		my_models[*selected_model].trans_init_time = glfwGetTime();
		translate_model(my_models[*selected_model]);
	}
	else if ((glfwGetKey(g_pWindow, GLFW_KEY_E) == GLFW_PRESS)) {
		my_models[*selected_model].dir = Model::Direction::OUT;
		my_models[*selected_model].translate = true;
		my_models[*selected_model].trans_init_time = glfwGetTime();
		translate_model(my_models[*selected_model]);
	}



	if (glfwGetKey(g_pWindow, GLFW_KEY_DELETE) == GLFW_PRESS) {
		if (glfwGetKey(g_pWindow, GLFW_KEY_DELETE) == GLFW_RELEASE) {
			if (my_models.size() > 0 && *selected_model < (my_models.size()))
				my_models.erase(my_models.begin() + *selected_model);
			if (*selected_model > my_models.size())
				*selected_model = 0;
		}

	}
	//f9 for bezier curve
	if (glfwGetKey(g_pWindow, GLFW_KEY_F9) == GLFW_PRESS) {
		if (glfwGetKey(g_pWindow, GLFW_KEY_F9) == GLFW_RELEASE) {
			my_models[*selected_model].start_pos = vec2(my_models[*selected_model].modelMatrix[3][0], my_models[*selected_model].modelMatrix[3][1]); //saves start position
			my_models[*selected_model].bezier = true;
			my_models[*selected_model].t_bezier = 0;
		}
	}
	//f10 for bspline curve
	else if (glfwGetKey(g_pWindow, GLFW_KEY_F10) == GLFW_PRESS) {
		if (glfwGetKey(g_pWindow, GLFW_KEY_F10) == GLFW_RELEASE) {
			my_models[*selected_model].start_pos = vec2(my_models[*selected_model].modelMatrix[3][0], my_models[*selected_model].modelMatrix[3][1]); //saves start position
			my_models[*selected_model].bspline = true;
			my_models[*selected_model].t_spline = 0;
		}
	}
	//f11 for catmull rom spline
	else if (glfwGetKey(g_pWindow, GLFW_KEY_F11) == GLFW_PRESS) {
		if (glfwGetKey(g_pWindow, GLFW_KEY_F11) == GLFW_RELEASE) {
			my_models[*selected_model].start_pos = vec2(my_models[*selected_model].modelMatrix[3][0], my_models[*selected_model].modelMatrix[3][1]); //saves start position
			my_models[*selected_model].catmull = true;
			my_models[*selected_model].t_catmull = 0;
		}
	}
	if (glfwGetKey(g_pWindow, GLFW_KEY_F12) == GLFW_PRESS) {
		if (glfwGetKey(g_pWindow, GLFW_KEY_F12) == GLFW_RELEASE) {
			my_models[*selected_model].isExample = (my_models[*selected_model].isExample == true ? false : true);
		}
	}
}