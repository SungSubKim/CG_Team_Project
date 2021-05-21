#pragma once
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
vec3& boss_center = vec3(MAP_X * 3 / 4, 0, MAP_Z / 2);
vec3& invisible_center = vec3(68, 0, 10);
static float duration = 0.0f;
//장외 판정을 모든 사각형으로 나누어서한다.
//판을 사각형으로 나누어 입력하였다. land는 map2의 육지 bridge는 map2의 다리

int check_map1(bool& fall, int stage, int enemy_num, bool& play, bool& triangle_added) {
	//onLand_old,now는 각각 이전 프레임에 육지위에 있었는지 아닌지를 판단,bridge 또한 같은 방법으로 작동
	model& model_character = getModel("Character");
	model& e1_character = getModel("Enemy1");
	vec3& s_center = model_character.center;
	vec3& e1_center = e1_character.center;
	bool onLand_old = false, onLand_now = false;
	bool onLand_old_e1 = false, onLand_now_e1 = false;
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

		if ((s.point.x <= old_e1_center.x && s.point.z <= old_e1_center.z && old_e1_center.z <= s.point.z + s.z_length))
			onLand_old_e1 = true;
		if ((s.point.x <= e1_center.x && s.point.z <= e1_center.z && e1_center.z <= s.point.z + s.z_length))
			onLand_now_e1 = true;
		/*map1right = (old_s_center.x <= s.point.x + s.x_length && s_center.x <= s.point.x + s.x_length);
		if (!map1right) stage++;*/
	}
	//printf("%d %d\n", (old_s_center.x < 128 && 128 <= s_center.x),stage);
	bool onObstacle_old = false, onObstacle_now = false;
	bool onObstacle_old_e1 = false, onObstacle_now_e1 = false;
	for (auto& obs : divided_map1_obstacle) {
		if ((obs.point.x <= old_s_center.x && old_s_center.x <= obs.point.x + obs.x_length
			&& obs.point.z <= old_s_center.z && old_s_center.z <= obs.point.z + obs.z_length))
			onObstacle_old = true;
		if ((obs.point.x <= s_center.x && s_center.x <= obs.point.x + obs.x_length
			&& obs.point.z <= s_center.z && s_center.z <= obs.point.z + obs.z_length))
			onObstacle_now = true;
		if ((obs.point.x <= old_e1_center.x && old_e1_center.x <= obs.point.x + obs.x_length && obs.point.z <= old_e1_center.z && old_e1_center.z <= obs.point.z + obs.z_length))
			onLand_old_e1 = true;
		if ((obs.point.x <= e1_center.x && e1_center.x <= obs.point.x + obs.x_length && obs.point.z <= e1_center.z && e1_center.z <= obs.point.z + obs.z_length))
			onLand_now_e1 = true;
	}

	if (pow((s_center.x - 69) / 25.0f, 2) + pow((s_center.z - 35) / 13.0f, 2) < 1)
		onObstacle_now = true;
	if (pow((old_s_center.x - 69) / 25.0f, 2) + pow((old_s_center.z - 35) / 13.0f, 2) < 1)
		onObstacle_old = true;
	if (pow((e1_center.x - 69) / 25.0f, 2) + pow((e1_center.z - 35) / 13.0f, 2) < 1)
		onObstacle_now_e1 = true;
	if (pow((old_e1_center.x - 69) / 25.0f, 2) + pow((old_e1_center.z - 35) / 13.0f, 2) < 1)
		onObstacle_old_e1 = true;
	if (!onObstacle_old && onObstacle_now) {
		CopyMemory(s_center, old_s_center, sizeof(vec3));

	}
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
	if ((!onObstacle_old_e1 && onObstacle_now_e1) || (onLand_old_e1 && !onLand_now_e1)) {
		CopyMemory(e1_center, old_e1_center, sizeof(vec3));
	}
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
		if (getModel("triangle").center.y < -DEFAULT_HIGHT) {
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

int check_map2(bool& fall, int stage) {
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
		/*if (s.point.x <= old_s_center.x && s.point.x <= s_center.x) {
			map2right = true;
		}
		if (old_s_center.x <= s.point.x + s.x_length && s_center.x <= s.point.x + s.x_length) {
			map2left = true;
		}
		if (map2left) stage--;
		if (map2right) stage++;*/
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
		//CopyMemory(s_center, old_s_center, sizeof(vec3));
		if (s_center.x <= 0)
			stage--;
		else
			fall = true;
	}
	return stage;
}

void check_side() {
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
	vec3& diff_e = normalize(direction_to_source) * (t - old_t) * 10.0f;
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
//boss의 행동 정리
//1. 벨이 false이고 주인공이 72 안넘어가면 가만히 있어야함.
//2. 벨이 true이고 
//2. 벨이 이전에 방문해서 true가 되고 지속시간 내면 벨에 있어야함.
//3. 벨의 지속시간이 끝나면 원래 있던 위치로 되돌아감.
//4.
void trace_enemy_direction_boss(model& source, model& destination, float t, float old_t, bool bell, bool opacity) {
	static bool aggro = false;
	vec3& s_center = vec3(0, 0, 0);
	
	if (aggro) {
		s_center = source.center;
	}

	//어그로가 안끌리면
	if (!aggro) {
		s_center = boss_center;
	}
	if (xz_distance(source.center, destination.center) < 30.0f) {
		aggro = true;
	}else {
		aggro = false;
	}

	if (bell == true) {
		aggro = false;
		s_center = bell_center;
	}
	if (opacity && bell == false) {
		s_center = boss_center;
	}
	vec3& d_center = destination.center;
	vec3& direction_to_source = s_center - d_center;
	vec3& diff_e = normalize(direction_to_source) * (t - old_t) * 15.0f;
	diff_e.y = 0;
	printf("%lf\n", s_center.x);

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
	static bool previously_visited = false;

	if (xz_distance(s_center, bell_center) < 10.0f && space) {
		previously_visited = true;
		return true;
	}
	if (previously_visited) {
		duration += t - old_t;

		if (duration >= 5.0f) {
			duration = 0.0f;
			previously_visited = false;
		}
		return true;
	}

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
		printf("geelo\n");
		collide = true;
		e1collide = true;
		e1.live = false;
		e1.visible = false;
	}
	if (e2.visible && !e2collide && xz_distance(e2.center, s_center) < 5.0f) {
		printf("geelo\n");
		collide = true;
		e2collide = true;
		e2.live = false;
		e2.visible = false;
	}
	if (e3.visible && !e3collide && xz_distance(e3.center, s_center) < 5.0f) {
		printf("geelo\n");
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
bool getTriangle(bool b_triangle, bool& b_ability) {
	int stage = 2;
	if (b_triangle)
		return true;
	bool res = false;
	vec3& s_center = getModel("Character").center;

	vec3& t_center = getModel("triangle").center;
	//printf("%f \n", xz_distance(vec3(120, 0, 35), s_center));
	if (xz_distance(t_center, s_center) < 7) {
		if (b_ability) {
			res = true;
			b_ability = false;
		}
	}
	else
		b_ability = true;
	return res;
}
void setStage(int stage, bool b_triangle) {
	static int old_stage = 0;
	model& model_ch = getModel("Character");
	if (old_stage != stage) {
		//printf("%d %d\n", b_triangle, getModel("triangle").stage);
		if (b_triangle)
			getModel("triangle").stage = stage;
		for (auto& m : models) {
			if (m.name == "triangle" && m.stage == stage) {
				m.visible = true;
				continue;
			}
			m.visible = false;
		}
		model_ch.visible = true;
		switch (stage) {
		case 1:
			if (old_stage == 0) {
				getModel("Enemy1").live = true;
				getModel("Enemy2").live = true;
				getModel("Enemy3").live = true;
				getModel("Character").center = vec3(2.3f, 0, 20);
			}
			else if (old_stage == 2)
				model_ch.center.x = 128 - 0.1f;
			getModel("Map1").visible = true;
			getModel("Map3").visible = false;
			getModel("Enemy3").visible = getModel("Enemy3").live;
			getModel("Enemy2").visible = getModel("Enemy2").live;
			getModel("Enemy1").visible = getModel("Enemy1").live;
			break;
		case 2:
			if (old_stage == 1) {
				model_ch.center.x = 0.1f;
			}
			else
				model_ch.center.x = 128 - 0.1f;
			getModel("Map2_1").visible = true;
			getModel("Map2_3").visible = true;
			break;
		case 3:
			model_ch.center.x = 0.1f;
			getModel("Map3").visible = true;
			getModel("Boss").visible = true;
			break;
		default:
			break;
		}
	}
	old_stage = stage;
}