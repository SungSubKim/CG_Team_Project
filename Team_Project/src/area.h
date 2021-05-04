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
//장외 판정을 모든 사각형으로 나누어서한다.
std::vector<struct square> divided_map2 = {
	{ vec3(0,0,0),36,72 },{ vec3(36,0,9),36,9 },{ vec3(63,0,18),9,18 },
{ vec3(45,0,27),27,9 },{ vec3(45,0,36),9,18 },{ vec3(54,0,45),18,9 },
{ vec3(63,0,54),27,9 },{ vec3(90,0,0),36,72 } };
//판을 사각형으로 나누어 입력하였다.
void check_on_area() {
	bool good = false;
	for (auto& s : divided_map2) {
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
//실제로 나가면 s_center값의 초기화가 이루어진다.