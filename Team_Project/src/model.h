#pragma once
#include <string>
#include <vector>
#define DEFAULT_HIGHT 10.0
// 맵을 10만큼 내린다.
#define MAP_X	128
#define MAP_Z	72
// 맵의 가로 세로 크기
struct model {
	std::string path = "/";
	//실제 모델의 로컬 경로
	std::string name = "default";
	// getModelByName의 파라미터 비교용 이름, triangle, Map2_1, ..등등이 쓰였다.
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
	// 기본값은 단위행렬로 셋팅, render전에 model_matrix로서 호출되어 uniform 변수로 vertex shader에 넘겨진다.
	bool visible = true;
	//visible은 true로 되있고 초기화 과정이나 키보드 입력과정에서 false로 변경되면 render되지 않는다.
	mesh2* pMesh;
	//mesh pointer
};
inline void model::update_matrix() {
	model_matrix = mat4::translate(center)
		* mat4::rotate(pole, theta) * mat4::scale(scale);
}
//update 함수에서 쓰인다.
model none = { " ","none" };
//getModelByName에서 불러오지 못할때 예외 model객체로 리턴하게 된다.

std::vector<model> models;
std::vector<model *> models_enemy, models_all_agent;
//model들의 정보가 담긴 model struct의 vector이다.

model& getModel(std::string name) {
	// name값을 기준으로 맞는 model을 호출한다.
	// 존재하지않으면 none이라는 model을 반환
	model& res = none;
	for (auto& model : models)
		if (model.name == name)
			return model;
	return res;
}
bool load_models() {
	// models vector에 등록된 모든 모델들을 로드한다.
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
// 이것에 따라 속력 감퇴여부 결정
//vec3 s_center = vec3(0, 0, 0); // sphere의 시작지점

