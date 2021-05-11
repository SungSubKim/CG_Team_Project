#pragma once
#include <string>
#include <vector>
#define DEFAULT_HIGHT 10.0
// ���� 10��ŭ ������.
#define MAP_X	128
#define MAP_Z	72
// ���� ���� ���� ũ��
struct model {
	std::string path = "/";
	//���� ���� ���� ���
	std::string name = "default";
	// getModelByName�� �Ķ���� �񱳿� �̸�, triangle, Map2_1, ..����� ������.
	vec3 center;
	float scale=1, theta=0;
	vec3 pole=vec3(0,1,0);
	
	mat4 model_matrix = mat4::translate(center) 
		* mat4::rotate(pole,theta) * mat4::scale(scale);/*{
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
	};*/
	void update_matrix();
	// �⺻���� ������ķ� ����, render���� model_matrix�μ� ȣ��Ǿ� uniform ������ vertex shader�� �Ѱ�����.
	bool visible = true;
	//visible�� true�� ���ְ� �ʱ�ȭ �����̳� Ű���� �Է°������� false�� ����Ǹ� render���� �ʴ´�.
	mesh2* pMesh;
	//mesh pointer
};
inline void model::update_matrix() {
	model_matrix = mat4::translate(center)
		* mat4::rotate(pole, theta) * mat4::scale(scale);
}
//update �Լ����� ���δ�.
model none = { " ","none" };
//getModelByName���� �ҷ����� ���Ҷ� ���� model��ü�� �����ϰ� �ȴ�.

std::vector<model> models;
std::vector<model *> models_enemy, models_all_agent;
//model���� ������ ��� model struct�� vector�̴�.

model& getModel(std::string name) {
	// name���� �������� �´� model�� ȣ���Ѵ�.
	// �������������� none�̶�� model�� ��ȯ
	model& res = none;
	for (auto& model : models)
		if (model.name == name)
			return model;
	return res;
}
bool load_models() {
	// models vector�� ��ϵ� ��� �𵨵��� �ε��Ѵ�.
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

int	accel = 0;
// �̰Ϳ� ���� �ӷ� ���𿩺� ����
//vec3 s_center = vec3(0, 0, 0); // sphere�� ��������

