// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>

using namespace glm;

#include "Model.hpp"
#include<glm/gtx/spline.hpp>

#pragma once
void rotate_around_point(Model &my_model, glm::vec3 p) {
	//glm::vec3 pos = glm::vec3(my_model.modelMatrix[3][0], my_model.modelMatrix[3][1], my_model.modelMatrix[3][2]); //creates matrix from model pos
	//glm::vec3 pos = glm::vec3(my_model.modelMatrix[0][3], my_model.modelMatrix[1][3], my_model.modelMatrix[2][3]); //creates matrix from model pos
	glm::vec3 pos = my_model.finalPos;
	glm::mat4 to_origin = glm::translate(my_model.modelMatrix, (-pos) + p); //send to origin

	to_origin = glm::rotate(to_origin, (float)my_model.angle, glm::vec3(0, 1, 0)); //rotates

	to_origin = glm::translate(to_origin, (-p) + pos);
	my_model.modelMatrix = to_origin;
}

void translate_model(Model &my_model) {
	glm::vec3 d(0.0f, 0.0f, 0.0f);
	if (my_model.dir == Model::Direction::LEFT) {
		d.x -= my_model.step;
	}
	else if (my_model.dir == Model::Direction::RIGHT) {
		d.x += my_model.step;
	}
	else if (my_model.dir == Model::Direction::DOWN) {
		d.y -= my_model.step;
	}
	else if (my_model.dir == Model::Direction::UP) {
		d.y += my_model.step;
	}
	else if (my_model.dir == Model::Direction::IN) {
		d.z -= my_model.step;
	}
	else {
		d.z += my_model.step;
	}
	my_model.modelMatrix = glm::translate(my_model.modelMatrix, d);
	my_model.finalPos.x = my_model.modelMatrix[3].x; my_model.finalPos.y = my_model.modelMatrix[3].y; my_model.finalPos.z = my_model.modelMatrix[3].z;
}

vec2 getBezierPoint(vec2* points, int numPoints, float t) {
	vec2* tmp = new vec2[numPoints];
	memcpy(tmp, points, numPoints * sizeof(vec2));
	int i = numPoints - 1;
	while (i > 0) {
		for (int k = 0; k < i; k++)
			tmp[k] = tmp[k] + t * (tmp[k + 1] - tmp[k]);
		i--;
	}
	vec2 answer = tmp[0];
	delete[] tmp;
	return answer;
}

glm::vec3 catmull_rom_spline(const std::vector<glm::vec3>& cp, float t)
{
	// indices of the relevant control points
	int i0 = (int)glm::clamp(t - 1, 0.0f, (float)cp.size() - 1);
	int i1 = (int)glm::clamp(t, 0.0f, (float)cp.size() - 1);
	int i2 = (int)glm::clamp(t + 1, 0.0f, (float)cp.size() - 1);
	int i3 = (int)glm::clamp(t + 2, 0.0f, (float)cp.size() - 1);

	// parameter on the local curve interval
	float local_t = glm::fract(t);

	return glm::catmullRom(cp[i0], cp[i1], cp[i2], cp[i3], local_t);
}

glm::vec3 cubic_spline(const std::vector<glm::vec3>& cp, float t)
{
	// indices of the relevant control points
	int i0 = (int)glm::clamp(t - 1, 0.0f, (float)cp.size() - 1);
	int i1 = (int)glm::clamp(t, 0.0f, (float)cp.size() - 1);
	int i2 = (int)glm::clamp(t + 1, 0.0f, (float)cp.size() - 1);
	int i3 = (int)glm::clamp(t + 2, 0.0f, (float)cp.size() - 1);

	// parameter on the local curve interval
	float local_t = glm::fract(t);

	return glm::catmullRom(cp[i0], cp[i1], cp[i2], cp[i3], local_t);
}