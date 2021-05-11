#pragma once
#include <string>
#include <vector>
// s_center에 관한 정보, 장외로 나갔는지 판단하는 check_on_area함수
struct square
{
	vec3 point;
	GLint x_length;
	GLint z_length;
};
vec3 old_s_center = vec3(0);
std::vector<struct square> divided_map1_land = {
	{ vec3(0,0,0),128,72 },{ vec3(90,0,0),36,72 } };
std::vector<struct square> divided_map1_obstacle = {
	{ vec3(0,0,0),14,16 },{ vec3(0,0,0),25,13 },{ vec3(0,0,0),30,11 },
{ vec3(0,0,0),35,8 },{ vec3(0,0,0),38,5 },{ vec3(0,0,0),40,3 }, { vec3(0,0,38),7,72-38 }, { vec3(0,0,41),14,72-41 },
{ vec3(0,0,45),18,72-45 }, { vec3(0,0,50),18,72-50 }, { vec3(0,0,55),20,72-55 },
{ vec3(0,0,58),24,72-58 },{ vec3(0,0,61),29,MAP_Z-61 }, { vec3(0,0,63),33,MAP_Z-63 },
{ vec3(0,0,65),33,MAP_Z-65 }, { vec3(0,0,69),39,MAP_Z-69 }};
std::vector<struct square> divided_map2_land = {
	{ vec3(0,0,0),36,72 },{ vec3(90,0,0),36,72 } };
//장외 판정을 모든 사각형으로 나누어서한다.
std::vector<struct square> divided_map2_bridge = {
	{ vec3(36,0,9),36,9 },{ vec3(63,0,18),9,18 },
{ vec3(45,0,27),27,9 },{ vec3(45,0,36),9,18 },{ vec3(54,0,45),18,9 },
{ vec3(63,0,54),27,9 }};
std::vector<struct square> divided_map3 = {
	{ vec3(0,0,0),128,72 } };
//장외 판정을 모든 사각형으로 나누어서한다.
//판을 사각형으로 나누어 입력하였다. land는 map2의 육지 bridge는 map2의 다리

void check_map1() {
	//onLand_old,now는 각각 이전 프레임에 육지위에 있었는지 아닌지를 판단,bridge 또한 같은 방법으로 작동
	model& model_character = getModel("Character");
	vec3& s_center = model_character.center;
	bool onLand_old = false, onLand_now = false;
	for (auto& s : divided_map1_land) {
		if ((s.point.x <= old_s_center.x && old_s_center.x <= s.point.x + s.x_length
			&& s.point.z <= old_s_center.z && old_s_center.z <= s.point.z + s.z_length))
		{
			onLand_old = true;
		}
		if ((s.point.x <= s_center.x && s_center.x <= s.point.x + s.x_length
			&& s.point.z <= s_center.z && s_center.z <= s.point.z + s.z_length))
		{
			onLand_now = true;
		}
	}
	bool onObstacle_old = false, onObstacle_now = false;
	for (auto& s : divided_map1_obstacle) {
		if ((s.point.x <= old_s_center.x && old_s_center.x <= s.point.x + s.x_length
			&& s.point.z <= old_s_center.z && old_s_center.z <= s.point.z + s.z_length))
		{
			onObstacle_old = true;
		}
		if ((s.point.x <= s_center.x && s_center.x <= s.point.x + s.x_length
			&& s.point.z <= s_center.z && s_center.z <= s.point.z + s.z_length))
		{
			onObstacle_now = true;
		}
	}
	if (pow((s_center.x - 69) / 25.0f, 2) + pow((s_center.z - 35) / 13.0f, 2) < 1)
		onObstacle_now = true;
	if (pow((old_s_center.x - 69) / 25.0f, 2) + pow((old_s_center.z - 35) / 13.0f, 2) < 1)
		onObstacle_old = true;
	if((!onObstacle_old && onObstacle_now)|| (onLand_old&&!onLand_now))
		CopyMemory(s_center, old_s_center, sizeof(vec3));

	return;
}
void check_map2() {
	//onLand_old,now는 각각 이전 프레임에 육지위에 있었는지 아닌지를 판단,bridge 또한 같은 방법으로 작동
	model& model_character = getModel("Character");
	vec3& s_center = model_character.center;
	bool onLand_old = false,onLand_now = false;
	for (auto& s : divided_map2_land) {
		if ((s.point.x <= old_s_center.x  && old_s_center.x <= s.point.x + s.x_length
			&& s.point.z <= old_s_center.z && old_s_center.z <= s.point.z + s.z_length))
		{
			onLand_old = true;
		}
		if ((s.point.x <= s_center.x && s_center.x <= s.point.x + s.x_length
			&& s.point.z <= s_center.z && s_center.z <= s.point.z + s.z_length))
		{
			onLand_now = true;
		}
	}
	bool onBridge_old = false, onBridge_now= false;
	for (auto& s : divided_map2_bridge) {
		if ((s.point.x <= old_s_center.x && old_s_center.x <= s.point.x + s.x_length
			&& s.point.z <= old_s_center.z && old_s_center.z <= s.point.z + s.z_length))
		{
			onBridge_old = true;
		}
		if ((s.point.x <= s_center.x && s_center.x <= s.point.x + s.x_length
			&& s.point.z <= s_center.z && s_center.z <= s.point.z + s.z_length))
		{
			onBridge_now = true;
		}
	}
	if (!onBridge_now && !onLand_now) {
		if (onBridge_old) {
			// 다리위에서 떨어짐, 과거에는 다리위에 있었으나 영역밖으로 이동하려할때
			s_center.x = 0;
			s_center.z = 0;
		}
		else {
			// 육지위에서 떨어짐, 과거에는 다리위에 있었으나 영역밖으로 이동하려할때
			CopyMemory(s_center, old_s_center, sizeof(vec3));
		}
	}
	return;
}
void check_map3() {
	//onLand_old,now는 각각 이전 프레임에 육지위에 있었는지 아닌지를 판단,bridge 또한 같은 방법으로 작동
	model& model_character = getModel("Character");
	vec3& s_center = model_character.center;
	bool onLand_old = false, onLand_now = false;
	for (auto& s : divided_map3) {
		if ((s.point.x <= old_s_center.x && old_s_center.x <= s.point.x + s.x_length
			&& s.point.z <= old_s_center.z && old_s_center.z <= s.point.z + s.z_length))
		{
			onLand_old = true;
		}
		if ((s.point.x <= s_center.x && s_center.x <= s.point.x + s.x_length
			&& s.point.z <= s_center.z && s_center.z <= s.point.z + s.z_length))
		{
			onLand_now = true;
		}
	}
	if (onLand_old && !onLand_now) {
		CopyMemory(s_center, old_s_center, sizeof(vec3));
	}
	else if (!onLand_old && !onLand_now) {
		s_center = vec3(0);
	}
	return;
}
//실제로 나가면 s_center값의 초기화가 이루어지거나, 아니면 육지를 못벗어나게한다.
inline float xz_distance(vec3 a, vec3 b) {
	return  pow((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z),0.5f);
}
int check_to_enemy() {
	vec3& s_center = getModel("Character").center;
	int res = 0;
	model& e1 = getModel("Enemy1"), & e2 = getModel("Enemy2"), & e3 = getModel("Enemy3");
	/*printf("%f %f %f\n", xz_distance(e_center1, s_center), xz_distance(e_center2, s_center),
		xz_distance(e_center3, s_center));*/
	if (xz_distance(e1.center, s_center) < 5)
		getModel("Enemy1").visible = false;
	if (xz_distance(e2.center, s_center) < 5)
		getModel("Enemy2").visible = false;
	if (xz_distance(e3.center, s_center) < 5) {
		//printf("hi\n");
		getModel("Enemy3").visible = false;
	}
	if (e1.visible) res++; if (e2.visible) res++; if (e3.visible) res++;
	return res;
}
bool getTriangle() {
	int stage=2;
	bool res = false;
	vec3& s_center = getModel("Character").center;

	vec3& t_center = getModel("triangle").center;
	if (xz_distance(vec3(120, 0, 35), s_center) < 7) {
		stage = 3;
		s_center = vec3(0);
		getModel("triangle").visible = false;
		getModel("Enemy3").visible = false;
		getModel("Enemy2").visible = false;
		getModel("Enemy1").visible = false;
		res = true;
	}
	return res;
}
void setStage(int stage) {
	static int old_stage = 0;
	//printf("%d\n", stage);
	if (old_stage + 1 == stage) {
		switch (stage) {
			case 1:
				getModel("Map1").visible = true;
				getModel("Map2_1").visible = false;
				getModel("Map2_3").visible = false;
				getModel("Map3").visible = false;
				getModel("Enemy3").visible = true;
				getModel("Enemy2").visible = true;
				getModel("Enemy1").visible = true;
				break;
			case 2:
				//printf("hi");
				getModel("Character").center = vec3(3, 0, 3);
				getModel("Map1").visible = false;
				getModel("Map2_1").visible = true;
				getModel("Map2_3").visible = true;
				getModel("triangle").visible = true;
				getModel("Enemy3").visible = true;
				getModel("Enemy2").visible = true;
				getModel("Enemy1").visible = true;
				break;
			default:
				getModel("Character").center = vec3(3, 0, 3);
				getModel("Map2_1").visible = false;
				getModel("Map2_3").visible = false;
				getModel("Map3").visible = true;
				break;
		}
	}
	old_stage = stage;
}