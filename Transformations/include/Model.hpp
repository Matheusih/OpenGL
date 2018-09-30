// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>
#include <glerror.hpp>


#pragma once
class Model
{
public:
	enum Direction{UP, DOWN, LEFT, RIGHT, IN, OUT};
	enum Axis{X, Y, Z};
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;

	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;

	glm::mat4 modelMatrix = glm::mat4(1.0);
	std::vector<glm::mat4> transformations;

	glm::vec3 initialPos = glm::vec3(0, 0, 0);
	glm::vec3 finalPos = glm::vec3(0, 0, 0);

	bool isExample = 0;
	double anim_init_time;
	float step = 0.005f;
	int animation = 0;
	Direction dir;
	Axis rot_axis;
	int scale;
	float angle = glm::radians(15.0f);
	double currentAngle = 0;
	double range = 0;

	bool translate = 0;
	bool rotate = 0;
	bool scaling = 0;
	bool shearing = false;
	int shearing_axis;
	int rotate_about = 0;

	double trans_init_time;
	double rotate_init_time;
	double scaling_init_time;

	vec2 start_pos;
	bool bezier = 0;
	float t_bezier = 0.0f;
	glm::vec3 axis = glm::vec3(0, 0, 0);

	bool bspline;
	float t_spline = 0.0f;

	bool catmull;
	float t_catmull = 0.0f;

	Model(char * path, glm::vec3 initialPos);
	~Model();
};

