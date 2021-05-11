#pragma once
#include <string>
#include <vector>
// s_center�� ���� ����, ��ܷ� �������� �Ǵ��ϴ� check_on_area�Լ�
struct square
{
	vec3 point;
	GLint x_length;
	GLint z_length;
};
vec3 old_s_center = vec3(0);

std::vector<struct square> divided_map2_land = {
	{ vec3(0,0,0),36,72 },{ vec3(90,0,0),36,72 } };
//��� ������ ��� �簢������ ������Ѵ�.
std::vector<struct square> divided_map2_bridge = {
	{ vec3(36,0,9),36,9 },{ vec3(63,0,18),9,18 },
{ vec3(45,0,27),27,9 },{ vec3(45,0,36),9,18 },{ vec3(54,0,45),18,9 },
{ vec3(63,0,54),27,9 }};
std::vector<struct square> divided_map3 = {
	{ vec3(0,0,0),128,72 } };
//��� ������ ��� �簢������ ������Ѵ�.
//���� �簢������ ������ �Է��Ͽ���. land�� map2�� ���� bridge�� map2�� �ٸ�

void check_map2() {
	//onLand_old,now�� ���� ���� �����ӿ� �������� �־����� �ƴ����� �Ǵ�,bridge ���� ���� ������� �۵�
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
			// �ٸ������� ������, ���ſ��� �ٸ����� �־����� ���������� �̵��Ϸ��Ҷ�
			s_center.x = 0;
			s_center.z = 0;
		}
		else {
			// ���������� ������, ���ſ��� �ٸ����� �־����� ���������� �̵��Ϸ��Ҷ�
			CopyMemory(s_center, old_s_center, sizeof(vec3));
		}
	}
	return;
}
void check_map3() {
	//onLand_old,now�� ���� ���� �����ӿ� �������� �־����� �ƴ����� �Ǵ�,bridge ���� ���� ������� �۵�
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
//������ ������ s_center���� �ʱ�ȭ�� �̷�����ų�, �ƴϸ� ������ ��������Ѵ�.
inline float xz_distance(vec3 a, vec3 b) {
	return  pow((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z),0.5f);
}
void check_to_enemy() {
	vec3& s_center = getModel("Character").center;

	vec3& e_center1 = getModel("Enemy1").center;
	vec3& e_center2 = getModel("Enemy2").center;
	vec3& e_center3 = getModel("Enemy3").center;
	/*printf("%f %f %f\n", xz_distance(e_center1, s_center), xz_distance(e_center2, s_center),
		xz_distance(e_center3, s_center));*/
	if (xz_distance(e_center1, s_center) < 5)
		getModel("Enemy1").visible = false;
	if (xz_distance(e_center2, s_center) < 5)
		getModel("Enemy2").visible = false;
	if (xz_distance(e_center3, s_center) < 5) {
		//printf("hi\n");
		getModel("Enemy3").visible = false;
	}
}
int getTriangle() {
	int stage=2;
	vec3& s_center = getModel("Character").center;

	vec3& t_center = getModel("triangle").center;
	/*printf("%f %f %f\n", xz_distance(e_center1, s_center), xz_distance(e_center2, s_center),
		xz_distance(e_center3, s_center));*/
	//printf("%f	%f	%f\n", s_center.x,s_center.z, xz_distance(t_center, s_center));
	if (xz_distance(vec3(120, 0, 35), s_center) < 7) {
		stage = 3;
		s_center = vec3(0);
		getModel("triangle").visible = false;
		getModel("Enemy3").visible = false;
		getModel("Enemy2").visible = false;
		getModel("Enemy1").visible = false;
	}
	return stage;
}
void setStage(int stage) {
	//printf("%d\n", stage);
	switch (stage) {
	case 1:
		getModel("Map1").visible = true;
		getModel("Map2_1").visible = false;
		getModel("Map2_3").visible = false;
		getModel("Map3").visible = false;
		break;
	case 2:
		//printf("hi");
		getModel("Map1").visible = false;
		getModel("Map2_1").visible = true;
		getModel("Map2_3").visible = true;
		getModel("Map3").visible = false;
		break;
	default:
		getModel("Map1").visible = false;
		getModel("Map2_1").visible = false;
		getModel("Map2_3").visible = false;
		getModel("Map3").visible = true;
		break;
	}
}