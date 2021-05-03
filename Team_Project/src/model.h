#pragma once
#include <string>
#include <vector>
#define DEFAULT_HIGHT 10.0
#define MAP_X	95
#define MAP_Z	72
struct model {
	std::string path = "/";
	std::string name = "default";
	mat4 model_matrix = {
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
	};
	bool visible = true;
	mesh2* pMesh;
};
model none = { " ","none" };

std::vector<model> models;

model& getModelByName(std::string name) {
	model& res = none;
	for (auto& model : models)
		if (model.name == name)
			return model;
	return res;
}
bool load_models() {
	bool res = true;
	for (auto& model : models) {
		model.pMesh = load_model(model.path);
		if (model.pMesh == nullptr) {
			printf("Panic: Unable to load mesh\n");
			res = false;
		}
	}
	return res;
}

int	deaccel_keys = 0;
vec3 s_center = vec3(0, 0, 0); // sphere의 시작지점

//checking area
struct square
{
	vec3 point;
	GLint x_length;
	GLint z_length;
};
std::vector<struct square> square = {
	{ vec3(0,0,0),36,72 },{ vec3(36,0,9),36,9 },{ vec3(63,0,18),9,18 },
{ vec3(45,0,27),27,9 },{ vec3(45,0,36),9,18 },{ vec3(54,0,45),18,9 },
{ vec3(63,0,54),27,9 },{ vec3(90,0,0),36,72 } };
void check_on_area() {
	bool good = false;
	for (auto& s : square) {
		if (!(s_center.x < s.point.x || s.point.x + s.x_length < s_center.x
			|| s_center.z < s.point.z || s.point.z + s.z_length < s_center.z))
		{
			good = true;
			break;
		}
	}
	if (!good) {
		s_center.x = 0;
		s_center.z = 0;
	}
	return;
}