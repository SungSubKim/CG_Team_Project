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
	mat4 model_matrix = {
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
	};
	// �⺻���� ������ķ� ����, render���� model_matrix�μ� ȣ��Ǿ� uniform ������ vertex shader�� �Ѱ�����.
	bool visible = true;
	//visible�� true�� ���ְ� �ʱ�ȭ �����̳� Ű���� �Է°������� false�� ����Ǹ� render���� �ʴ´�.
	mesh2* pMesh;
	//mesh pointer
};
model none = { " ","none" };
//getModelByName���� �ҷ����� ���Ҷ� ���� model��ü�� �����ϰ� �ȴ�.

std::vector<model> models;
//model���� ������ ��� model struct�� vector�̴�.

model& getModelByName(std::string name) {
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

int	deaccel_keys = 0;
// �̰Ϳ� ���� �ӷ� ���𿩺� ����
vec3 s_center = vec3(0, 0, 0); // sphere�� ��������

