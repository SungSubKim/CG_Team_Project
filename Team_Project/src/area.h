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

std::vector<struct square> divided_map2_land = {
	{ vec3(0,0,0),36,72 },{ vec3(90,0,0),36,72 } };
//장외 판정을 모든 사각형으로 나누어서한다.
std::vector<struct square> divided_map2_bridge = {
	{ vec3(36,0,9),36,9 },{ vec3(63,0,18),9,18 },
{ vec3(45,0,27),27,9 },{ vec3(45,0,36),9,18 },{ vec3(54,0,45),18,9 },
{ vec3(63,0,54),27,9 }};
//판을 사각형으로 나누어 입력하였다. land는 map2의 육지 bridge는 map2의 다리
void check_on_area() {
	//onLand_old,now는 각각 이전 프레임에 육지위에 있었는지 아닌지를 판단,bridge 또한 같은 방법으로 작동
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
	//printf("%f %f %f %f %d %d %d %d\n", s_center.x, s_center.z, old_s_center.x, old_s_center.z,
	//	onLand_old, onLand_now, onBridge_old, onBridge_now);
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
//실제로 나가면 s_center값의 초기화가 이루어진다.