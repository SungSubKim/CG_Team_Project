#pragma once
//a
#include <string>
#include <vector>
#define MGN 0.5f
// s_center에 관한 정보, 장외로 나갔는지 판단하는 check_on_area함수
struct square
{
	vec3 point;
	GLfloat x_length;
	GLfloat z_length;
};
vec3 old_s_center = vec3(0);
vec3 old_e1_center = vec3(0);
vec3 old_e2_center = vec3(0);
vec3 old_e3_center = vec3(0);
bool e1collide = false;
bool e2collide = false;
bool e3collide = false;
bool Bosscollide = false;

std::vector<struct square> divided_map1_land = {
	{ vec3(-MGN,0,-MGN),128 + 2 * MGN,72 + 2 * MGN } };
std::vector<struct square> divided_map1_obstacle = {
	{ vec3(-MGN,0,-MGN),14 + MGN,16 + MGN },{ vec3(0,0,0),25,13 },{ vec3(0,0,0),30,11 },{ vec3(0,0,0),35,8 },{ vec3(0,0,0),38,5 },{ vec3(-MGN,0,-MGN),40 + MGN,3 + MGN },
{ vec3(-MGN,0,38),7 + MGN,72 - 38 + MGN }, { vec3(0,0,41),14,72 - 41 },{ vec3(0,0,45),18,72 - 45 }, { vec3(0,0,50),18,72 - 50 }, { vec3(0,0,55),20,72 - 55 },
{ vec3(0,0,58),24,72 - 58 },{ vec3(0,0,61),29,MAP_Z - 61 }, { vec3(0,0,63),33,MAP_Z - 63 },
{ vec3(0,0,65),33,MAP_Z - 65 }, { vec3(0 - MGN,0,69),39 + MGN,MAP_Z - 69 + MGN } };
std::vector<struct square> divided_map2_land = {
	{ vec3(0,0,0),36,72 },{ vec3(90,0,0),38,72 } };
//장외 판정을 모든 사각형으로 나누어서한다.
std::vector<struct square> divided_map2_bridge = {
	{ vec3(36,0,9),36,9 },{ vec3(63,0,18),9,18 },
{ vec3(45,0,27),27,9 },{ vec3(45,0,36),9,18 },{ vec3(54,0,45),18,9 },
{ vec3(63,0,54),27,9 } };
std::vector<struct square> divided_map3 = {
	{ vec3(0,0,0),128,72 } };
vec3& bell_center = vec3(70, 0, 70);
vec3& boss_center = vec3(MAP_X * 4 / 5, 0, MAP_Z / 2);
vec3& invisible_center = vec3(68, 0, 10);
static float bell_duration = 0.0f;
//장외 판정을 모든 사각형으로 나누어서한다.
//판을 사각형으로 나누어 입력하였다. land는 map2의 육지 bridge는 map2의 다리

int check_map1(bool& fall, int stage, int enemy_num, bool& play, bool& triangle_added) {
	//onLand_old,now는 각각 이전 프레임에 육지위에 있었는지 아닌지를 판단,bridge 또한 같은 방법으로 작동
	model& model_character = getModel("Character");
	vec3& s_center = model_character.center;
	bool onLand_old = false, onLand_now = false;
	bool map1right = false;

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

		/*map1right = (old_s_center.x <= s.point.x + s.x_length && s_center.x <= s.point.x + s.x_length);
		if (!map1right) stage++;*/
	}
	//printf("%d %d\n", (old_s_center.x < 128 && 128 <= s_center.x),stage);
	bool onObstacle_old = false, onObstacle_now = false;
	std::vector<model*> enemy;
	enemy.push_back(&getModel("Enemy1"));
	enemy.push_back(&getModel("Enemy2"));
	enemy.push_back(&getModel("Enemy3"));
	bool onObstacle_old_e[3] = { false, }, onObstacle_now_e[3] = { false, };

	static vec3 old_e_center[3] = { enemy[0]->center,enemy[1]->center,enemy[2]->center };

	for (auto& obs : divided_map1_obstacle) {
		if ((obs.point.x <= old_s_center.x && old_s_center.x <= obs.point.x + obs.x_length
			&& obs.point.z <= old_s_center.z && old_s_center.z <= obs.point.z + obs.z_length))
			onObstacle_old = true;
		if ((obs.point.x <= s_center.x && s_center.x <= obs.point.x + obs.x_length
			&& obs.point.z <= s_center.z && s_center.z <= obs.point.z + obs.z_length))
			onObstacle_now = true;
		for (int i = 0; i < 3; i++) {
			if ((obs.point.x <= old_e_center[i].x && old_e_center[i].x <= obs.point.x + obs.x_length && obs.point.z <= old_e_center[i].z && old_e_center[i].z <= obs.point.z + obs.z_length))
				onObstacle_old_e[i] = true;
			if ((obs.point.x <= enemy[i]->center.x && enemy[i]->center.x <= obs.point.x + obs.x_length && obs.point.z <= enemy[i]->center.z && enemy[i]->center.z <= obs.point.z + obs.z_length))
				onObstacle_now_e[i] = true;
		}
	}

	for (auto& e : enemy) {
		if (pow((s_center.x - 69) / 25.0f, 2) + pow((s_center.z - 35) / 13.0f, 2) < 1)
			onObstacle_now = true;
		if (pow((old_s_center.x - 69) / 25.0f, 2) + pow((old_s_center.z - 35) / 13.0f, 2) < 1)
			onObstacle_old = true;
		for (int i = 0; i < 3; i++) {
			if (pow((enemy[i]->center.x - 69) / 25.0f, 2) + pow((enemy[i]->center.z - 35) / 13.0f, 2) < 1)
				onObstacle_now_e[i] = true;
			if (pow((old_e_center[i].x - 69) / 25.0f, 2) + pow((old_e_center[i].z - 35) / 13.0f, 2) < 1)
				onObstacle_now_e[i] = true;
		}
	}

	if (!onObstacle_old && onObstacle_now)
		CopyMemory(s_center, old_s_center, sizeof(vec3));
	else if (onLand_old && !onLand_now) {
		if (128 <= s_center.x) {
			if (enemy_num == 0)
				stage++;
			else
				CopyMemory(s_center, old_s_center, sizeof(vec3));
		}
		else
			fall = true;
	}
	for (int i = 0; i < 3; i++) {
		if (!onObstacle_old_e[i] && onObstacle_now_e[i]) {
			//printf("hi\n");
			CopyMemory(enemy[i]->center, old_e_center[i], sizeof(vec3));
		}
	}
	for (int i = 0; i < 3; i++)
		CopyMemory(old_e_center[i], enemy[i]->center, sizeof(vec3));

	static bool fall_triangle = false, old_fall_triangle = false;
	static float falling_start = 0;
	float t = (float)glfwGetTime();
	fall_triangle = (enemy_num == 0);
	if (fall_triangle && !triangle_added) {
		if (!old_fall_triangle) {
			falling_start = t;
			getModel("triangle").center = vec3(MAP_X * 4 / 5, 30, MAP_Z / 2);
			getModel("triangle").visible = true;
			play = true;
		}
		float dt_fall = t - falling_start;
		if (getModel("triangle").center.y < -DEFAULT_HIGHT - 5) {
			fall_triangle = false;
			triangle_added = true;
		}
		else {
			getModel("triangle").center.y = 30 - 20 * dt_fall * dt_fall;
		}
	}
	old_fall_triangle = fall_triangle;

	return stage;
}

int check_map2(bool& fall, int stage, int difficulty) {
	//onLand_old,now는 각각 이전 프레임에 육지위에 있었는지 아닌지를 판단,bridge 또한 같은 방법으로 작동
	model& model_character = getModel("Character");
	vec3& s_center = model_character.center;
	bool onLand_old = false, onLand_now = false;
	/*bool map2left = false, map2right = false;*/
	for (auto& s : divided_map2_land) {
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
	bool onBridge_old = false, onBridge_now = false;
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
	//if ((onBridge_old && !onBridge_now) ||(onLand_old && !onLand_now)) {
	if ((onBridge_old || onLand_old) && (!onBridge_now && !onLand_now)) {
		printf("%f\n", s_center.x);
		if (s_center.x <= 0) //left
			stage--;
		else if (s_center.x >= 128)
			stage++;
		else if (difficulty == 0 && onBridge_old)
			CopyMemory(s_center, old_s_center, sizeof(vec3));
		else
			fall = true;
	}
	return stage;
}

int check_map3(bool& fall, int stage) {
	model& model_character = getModel("Character");
	model& boss_character = getModel("Boss");
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
		if (s_center.x <= 0)
			stage--;
		else
			fall = true;
	}
	return stage;
}

inline float xz_distance(vec3 a, vec3 b) {
	return  pow((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z), 0.5f);
}

inline int x_distance(vec3 a, vec3 b) {
	int direct;
	if (a.x - b.x > 0) direct = 2;
	else direct = 1;

	return direct;
}

inline int z_distance(vec3 a, vec3 b) {
	int direct;
	if (a.z - b.z > 0) direct = 4;
	else direct = 3;
	return direct;
}

inline int direction(vec3 a, vec3 b) {
	int dist = 0;
	if (abs(a.x - b.x) < 5.0f && abs(a.z - b.z) < 20.0f) {
		dist = z_distance(a, b);
	}
	else if (abs(a.x - b.x) < 20.0f && abs(a.z - b.z) < 5.0f) {
		dist = x_distance(a, b);
	}
	return dist;
}

void trace_enemy_direction(model& source, model& destination, float t, float old_t) {
	vec3& s_center = source.center;
	vec3& d_center = destination.center;
	vec3& direction_to_source = s_center - d_center;
	vec3& diff_e = normalize(direction_to_source) * (t - old_t) * 15.0f;
	diff_e.y = 0;
	if (xz_distance(d_center, s_center) < 5.0f) {
		destination.theta = destination.theta;
		d_center = d_center;
		return;

	}
	if (d_center.z - s_center.z < 0) {
		if (d_center.x - s_center.x > 0) {
			destination.theta = PI / 2 - atan(abs((d_center.x - s_center.x) / (d_center.z - s_center.z)));

		}
		else {
			destination.theta = PI / 2 + atan(abs((d_center.x - s_center.x) / (d_center.z - s_center.z)));

		}
	}
	else {
		if (d_center.x - s_center.x > 0) {
			destination.theta = atan(abs((d_center.x - s_center.x) / (d_center.z - s_center.z))) - PI / 2;


		}
		else {
			destination.theta = -atan(abs((d_center.x - s_center.x) / (d_center.z - s_center.z))) - PI / 2;

		}
	}
	d_center = d_center + diff_e;
}

void trace_enemy_direction_boss(model& source, model& destination, float t, float old_t, bool bell, bool opacity) {
	static bool aggro = false;
	vec3& s_center = vec3(0, 0, 0);

	//투명하고 벨이 안울리면 자기자리로 돌아감.
	if (opacity && bell == false) {
		s_center = boss_center;
	}
	// 어그로가 끌리면 쫓아감
	else if (aggro) {
		s_center = source.center;
	}
	//어그로가 안끌리고 벨이 안울리면 자리로 돌아감
	else if (!aggro&&!bell) {
		s_center = boss_center;
	}

	//둘 사이 거리가 30이하면 쫓아감
	if (xz_distance(source.center, destination.center) < 30.0f) {
		aggro = true;
	}
	// 아니면 안쫓아감
	else {
		aggro = false;
	}
	//벨이 울리면 어그로를 벨이 받음
	if (bell == true) {
		aggro = false;
		s_center = bell_center;
	}


	vec3& d_center = destination.center;
	vec3& direction_to_source = s_center - d_center;
	vec3& diff_e = normalize(direction_to_source) * (t - old_t) * 15.0f;
	diff_e.y = 0;

	if (xz_distance(d_center, s_center) < 5.0f) {
		destination.theta = destination.theta;
		d_center = d_center;
		return;
	}

	if (d_center.z - s_center.z < 0) {
		if (d_center.x - s_center.x > 0) {
			destination.theta = PI / 2 - atan(abs((d_center.x - s_center.x) / (d_center.z - s_center.z)));
		}
		else {
			destination.theta = PI / 2 + atan(abs((d_center.x - s_center.x) / (d_center.z - s_center.z)));
		}
	}
	else {
		if (d_center.x - s_center.x > 0) {
			destination.theta = atan(abs((d_center.x - s_center.x) / (d_center.z - s_center.z))) - PI / 2;
		}
		else {
			destination.theta = -atan(abs((d_center.x - s_center.x) / (d_center.z - s_center.z))) - PI / 2;
		}
	}
	d_center = d_center + diff_e;
}

bool bell_ring(float t, float old_t, bool space) {

	vec3& s_center = getModel("Character").center;
	vec3& b_center = getModel("Boss").center;
	static bool previously_visited = false;
	//벨이 일정거리 이하고 스페이스바를 누른상태면 이전 방문상태 true 및 true 반환
	if (xz_distance(s_center, bell_center) < 15.0f && space) {
		previously_visited = true;
		return true;
	}
	//이전에 방문했던 상태면 5초 이후에 false 반환
	if (previously_visited) {
		printf("bell duration: %lf\n", bell_duration);
		if (xz_distance(b_center, bell_center) < 8.0f) {
			bell_duration += t - old_t;
		}
		
		if (bell_duration >= 5.0f) {
			bell_duration = 0.0f;
			previously_visited = false;
		}
		return true;
	}
	//이전에 방문하지 않았으면 false
	return false;
}

bool invisible() {
	vec3& s_center = getModel("Character").center;
	if (xz_distance(s_center, invisible_center) < 12.0f) {
		return true;
	}

	return false;
}

int check_to_enemy(int direc, bool space) {
	vec3& s_center = getModel("Character").center;
	int res = 0;
	model& e1 = getModel("Enemy1"), & e2 = getModel("Enemy2"), & e3 = getModel("Enemy3");
	if (xz_distance(e1.center, s_center) < 20 && space && direction(e1.center, s_center) == direc) {
		getModel("Enemy1").live = false;
		getModel("Enemy1").visible = false;
	}
	if (xz_distance(e2.center, s_center) < 20 && space && direction(e2.center, s_center) == direc) {
		getModel("Enemy2").live = false;
		getModel("Enemy2").visible = false;
	}
	if (xz_distance(e3.center, s_center) < 20 && space && direction(e3.center, s_center) == direc) {
		getModel("Enemy3").live = false;
		getModel("Enemy3").visible = false;
	}
	if (e1.visible) res++; if (e2.visible) res++; if (e3.visible) res++;
	return res;
}

int check_collision(int life) {
	vec3& s_center = getModel("Character").center;
	model& e1 = getModel("Enemy1"), & e2 = getModel("Enemy2"), & e3 = getModel("Enemy3");
	bool collide = false;
	if (e1.visible && !e1collide && xz_distance(e1.center, s_center) < 5.0f) {
		collide = true;
		e1collide = true;
		e1.live = false;
		e1.visible = false;
	}
	if (e2.visible && !e2collide && xz_distance(e2.center, s_center) < 5.0f) {
		collide = true;
		e2collide = true;
		e2.live = false;
		e2.visible = false;
	}
	if (e3.visible && !e3collide && xz_distance(e3.center, s_center) < 5.0f) {
		collide = true;
		e3collide = true;
		e3.live = false;
		e3.visible = false;
	}

	if (collide) {
		life--;
		collide = false;
	}

	return life;
}


float duration = 0.0f;
bool check_boss_collision(float t, float old_t) {
	vec3& s_center = getModel("Character").center;
	model& Boss = getModel("Boss");
	bool collide = false;
	duration += t - old_t;
	if (Boss.visible && !Bosscollide && xz_distance(Boss.center, s_center) < 10.0f) {
		collide = true;
		Bosscollide = true;
	}

	if (duration >= 5.0f) {
		duration = 0.0f;
		Bosscollide = false;
	}

	if (collide) {
		return collide;
	}

	return collide;
}

bool getTriangle(bool b_triangle) {
	bool res = false;
	if (b_triangle)
		return true;
	vec3& s_center = getModel("Character").center;

	vec3& t_center = getModel("triangle").center;
	if (xz_distance(t_center, s_center) < 7) {
		res = true;
	}
	return res;
}