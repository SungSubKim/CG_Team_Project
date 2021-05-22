#include "cgmath.h"			// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION
#include "cgut.h"			// slee's OpenGL utility
#include "cgut2.h"			// slee's OpenGL utility
#include "trackball.h"
#include "assimp_loader.h"
#include "model.h"
#include "area.h"
#include "particle.h"
#include "irrKlang\irrKlang.h"
#pragma comment(lib, "irrKlang.lib")

//*******************************************************************
// forward declarations for freetype text
bool init_text();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color, GLfloat dpi_scale = 1.0f);

//*************************************
// global constants
static const char* window_name = "Team project - Woori's Adventure";
static const char* vert_shader_path = "../bin/shaders/model.vert";
static const char* frag_shader_path = "../bin/shaders/model.frag";
static const char* skybox_left_path = "../bin/images/Background.png";
static const char* skybox_down_path = "../bin/images/Background_floor.png";
static const char* skybox_back_path = "../bin/images/Background.png";
static const char* skybox_right_path = "../bin/images/Background.png";
static const char* skybox_up_path = "../bin/images/Background.png";
static const char* skybox_front_path = "../bin/images/Background.png";
static const char* snow_image_path = "../bin/images/snow-flake.png";
static const char* title_image_path = "../bin/images/CGTitle.png";
static const char* help_image_path1 = "../bin/images/Map1.png";
static const char* help_image_path2 = "../bin/images/Map2.png";
static const char* help_image_path3 = "../bin/images/Map3.png";
static const char* select_image_path00 = "../bin/images/character_select00.png";
static const char* select_image_path01 = "../bin/images/character_select01.png";
static const char* select_image_path02 = "../bin/images/character_select02.png"; 
static const char* select_image_path10 = "../bin/images/character_select10.png"; 
static const char* select_image_path11 = "../bin/images/character_select11.png"; 
static const char* select_image_path12 = "../bin/images/character_select12.png";
static const char* select_image_path20 = "../bin/images/character_select20.png";
static const char* select_image_path21 = "../bin/images/character_select21.png"; 
static const char* select_image_path22 = "../bin/images/character_select22.png";
static const char* final_image_path = "../bin/images/Final.png";
static const char* die_image_path = "../bin/images/die.png";
static const char* mp3_path = "../bin/sounds/CGMusic.mp3";
static const char* attack_mp3_path = "../bin/sounds/attack.mp3";
static const char* bell_mp3_path = "../bin/sounds/bell.mp3";
static const char* falling_mp3_path = "../bin/sounds/falling.mp3";

std::vector<vertex>	unit_circle_vertices;	// host-side vertices
//*************************************
// common structures
//struct camera
//{
//	vec3	eye = vec3(0 + MAP_X / 2, 100, 80 + MAP_Z / 2);
//	vec3	at = vec3(MAP_X / 2, 0, 0 + MAP_Z / 2);
//	vec3	up = vec3(0, 1, 0);
//	mat4	view_matrix = mat4::look_at(eye, at, up);
//
//	float	fovy = PI / 4.0f; // must be in radian
//	float	aspect_ratio;
//	float	dNear = 1.0f;
//	float	dFar = 1000.0f;
//	mat4	projection_matrix;
//} cam;

struct light_t
{
	vec4	position = vec4(10.0f, 10.0f, 10.0f, 0.0f);   // directional light
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float	shininess = 100000.0f;
};

mat4 model_matrix_background = mat4::translate(0, 0.0f, -250.0f) * mat4::scale(500.0f, 500.0f, 100.0f);

// window objects
GLFWwindow* window = nullptr;
ivec2		window_size = ivec2(1280, 720); // cg_default_window_size(); // initial window size

//*******************************************************************
// irrKlang objects
irrklang::ISoundEngine* engine;
irrklang::ISoundSource* mp3_src = nullptr;
irrklang::ISoundSource* attack_mp3_src = nullptr;
irrklang::ISoundSource* bell_mp3_src = nullptr;
irrklang::ISoundSource* falling_mp3_src = nullptr;

//*************************************
// OpenGL objects
GLuint	program = 0;	// ID holder for GPU program
GLuint	vertex_buffer = 0;	// ID holder for vertex buffer
GLuint	index_buffer = 0;		// ID holder for index buffer
GLuint	vertex_array = 0;	// ID holder for vertex array object*************************
GLuint	snow_vertex_array = 0;
GLuint	TEX_SKY = 0, SKY_LEFT = 0, SKY_DOWN = 0, SKY_BACK = 0, SKY_RIGHT = 0, SKY_UP = 0, SKY_FRONT = 0, SNOWTEX = 0, TITLETEX = 0, SELECTTEX00 = 0, SELECTTEX01 = 0, SELECTTEX02 = 0, SELECTTEX10 = 0, SELECTTEX11= 0, SELECTTEX12= 0, SELECTTEX20= 0, SELECTTEX21 = 0, SELECTTEX22 = 0, HELPTEX1 = 0, HELPTEX2 = 0, HELPTEX3 = 0, FINALTEX = 0, DIETEX = 0;
GLuint	mode;
//*************************************
// global variables
float	alpha = 1.0f;
int		frame = 0;		// index of rendering frames
int		direc = 0;
int		stage = 0;
int		next = 0;
int		life = 3;
int		enemy_num = 3;
int		before_game = 0; // 0(title) -> (1) help -> (2) game start
int		difficulty = 0;
int		selected_res[2];
bool	b_help = false, b_wireframe = false, b_space = false, character_stop = false, b_die = false, old_b_die = false, b_triangle = false, b_ability_to_get = false, bell = false, opacity = false, triangle_added = false, boss_collide = false;

std::vector<particle_t> particles;

//*************************************
// scene objects
//
//camera		cam;
trackball	tb;
bool l = false, r = false, u = false, d = false; // 어느쪽으로 keyboard가 눌렸는지 flag
float old_t = 0;					//update 함수에서 dt값 계산을 위해 쓰이는 old value
float theta0 = 0;
float counter = 0.0f;
mat4 model_matrix0;
light_t		light;
material_t	material;
int frame_counter = 0;
//*************************************
//float min(float a, float b) {
//	return a < b ? a : b;
//}
//float& theta = getModel("Character").theta;
void rotate_character(float t, float old_t, float ntheta);
void update()
{
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "before_game"), before_game);
	glUniform1i(glGetUniformLocation(program, "b_help"), b_help);
	glUniform1i(glGetUniformLocation(program, "stage"), stage);
	if (before_game < 3 )
		return;
	if (stage == 4) {
		static float final_time = (float)glfwGetTime();
		if((float)glfwGetTime()-final_time > 3.0f)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}



	// update projection matrix
	cam.aspect_ratio = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect_ratio, cam.dNear, cam.dFar);

	// build the model matrix for oscillating scale
	static float falling_start = 0, ntheta = 0;
	static bool isfall = false, old_isfall = false;
	float t = float(glfwGetTime());
	int rate = 20; if (accel) rate *= 2; if (b_triangle) rate -= 5;
	float ds = 0;
	//키보드에서 left control키를 누른 상태면 속력이 감소하게 해준다
	model& model_character = getModel("Character");
	vec3& s_center = model_character.center;
	//몬스터 움직임 구현
	model& model_duck1 = getModel("Enemy1");
	model& model_duck2 = getModel("Enemy2");
	model& model_duck3 = getModel("Enemy3");
	model& model_boss = getModel("Boss");


	vec3& e1_center = model_duck1.center;

	if (!isfall && !b_help) {
		if (difficulty == 1) {
			trace_enemy_direction(model_character, model_duck1, t, old_t);
		}
		else {
			e1_center = vec3(69 + 26 * cos(0.2f * t), 0, 35 + 14 * sin(0.2f * t));
			model_duck1.theta = PI / 2.0f - 0.2f * t;
		}
	}

	static mat4 view_matrix0;
	if (isfall) {
		if (!old_isfall) {
			falling_start = t;
			view_matrix0 = cam.view_matrix;
			engine->play2D(falling_mp3_src);
		}
		float dt_fall = t - falling_start;
		if (dt_fall > 1.5f) {
			s_center = vec3(2.3f, 0, 20);
			cam.view_matrix = view_matrix0;
			isfall = false;
			life--;
		}
		else {
			vec3 eye = vec3(s_center.x + 70 * cos(ntheta), 30, s_center.z - 70 * sin(ntheta));
			vec3 at = vec3(s_center.x, 0.5f * s_center.y, s_center.z);
			getModel("Character").theta = ntheta;
			cam.view_matrix = mat4::look_at(eye, at, vec3(0, 1, 0));
			s_center.y = -20 * dt_fall * dt_fall;
		}
	}
	else {
		if (!character_stop)
			ds = (t - old_t);
		if (l) {
			s_center.x -= ds * rate;
			ntheta = PI / 2 * 2;
			direc = 1;
		}
		else if (r) {
			s_center.x += ds * rate;
			ntheta = 0;
			direc = 2;
		}
		else if (u) {
			s_center.z -= ds * rate;
			ntheta = PI / 2;
			direc = 3;
		}
		else if (d) {
			s_center.z += ds * rate;
			ntheta = PI / 2 * 3;
			direc = 4;
		}
		rotate_character(t, old_t, ntheta);
	}
	old_isfall = isfall;
	bool falling_play = false;
	switch (stage) {
	case 1:
		enemy_num = check_to_enemy(direc, b_space);
		stage = check_map1(isfall, 1, enemy_num, falling_play, triangle_added);
		if (falling_play)
			engine->play2D(falling_mp3_src);
		life = check_collision(life);
		break;
	case 2:
		stage = check_map2(isfall, 2, difficulty);
		break;
	case 3:
		stage = check_map3(isfall, 3);
		boss_collide = check_boss_collision(t, old_t);
		if (boss_collide) {
			life--;
			engine->play2D(falling_mp3_src);
			boss_collide = false;
		}
		break;
	}
	b_triangle = getTriangle(b_triangle) && b_ability_to_get;
	counter += t - old_t;
	//stage clear조건
	if (stage == 3) {
		bell = bell_ring(t, old_t, b_space);
		if (bell == true && counter >= 10.0f) {
			counter = 0.0f;
			engine->play2D(bell_mp3_src);
		}
		opacity = invisible();
		trace_enemy_direction_boss(model_character, model_boss, t, old_t, bell, opacity);

	}
	//stage clear조
	setStage(stage, b_triangle);
	if (stage == 0)
		stage++;

	static float die_t = 0;
	if (life == 0) {
		b_die = true;
		b_help = true;
		stage = 5;//DIETEX in fragment shader
		if (!old_b_die)
			die_t = (float)glfwGetTime();
		if ((float)glfwGetTime() - die_t > 2) {
			before_game = 0;
			stage = 0;
			life = 3;
			b_die = false;
			b_help = false;

			getModel("triangle").stage = 1;
			getModel("triangle").theta = 0;
			getModel("triangle").center = vec3(100, 30, MAP_Z - 20);
			getModel("triangle").visible = false;
			b_triangle = false;
			//b_ability_to_get = true;
			triangle_added = false;
		}
	}
	old_b_die = b_die;
	CopyMemory(old_s_center, s_center, sizeof(vec3));
	CopyMemory(old_e1_center, e1_center, sizeof(vec3));
	// old_s_center의 값을 준비하여 backtracking을 준비
	if (b_triangle) {
		float theta = model_character.theta;
		getModel("triangle").center = s_center + vec3(6 * cos(theta), -DEFAULT_HIGHT, -6 * sin(theta));
		getModel("triangle").theta = theta + PI / 6.0f;
	}
	else if (getModel("triangle").stage == 3 && stage == 3 && xz_distance(getModel("triangle").center, vec3(MAP_X - 10, 0, MAP_Z / 2)) < 7) {
		b_help = true;
		stage++;
	}
	else if (triangle_added)
		getModel("triangle").center.y = -DEFAULT_HIGHT - 5;
	for (auto& m : models)
		m.update_matrix();

	//center와 theta의 정보를 캐릭터매트릭스에 반영한다.

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);

	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);

	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);


	//화면 skybox 부분
	// glBindTexture(SKY_XXX) : 이 방향대로 붙어있으니 텍스처를 바꾸고싶으면 여기서 바꾸면 됨
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, SKY_LEFT);

	glUniform1i(glGetUniformLocation(program, "SKY_LEFT"), 1);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, SKY_DOWN);
	glUniform1i(glGetUniformLocation(program, "SKY_DOWN"), 2);


	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, SKY_BACK);
	glUniform1i(glGetUniformLocation(program, "SKY_BACK"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, SKY_RIGHT);
	glUniform1i(glGetUniformLocation(program, "SKY_RIGHT"), 4);


	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, SKY_UP);
	glUniform1i(glGetUniformLocation(program, "SKY_UP"), 5);


	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, SKY_FRONT);
	glUniform1i(glGetUniformLocation(program, "SKY_FRONT"), 6);

	// select the texture slot to bind
	for (auto& p : particles) {
		p.update(s_center.x, s_center.z, b_space, direc);
	}
	b_space = false;
}
void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// notify GL that we use our own program
	glUseProgram(program);

	float dpi_scale = cg_get_dpi_scale();

	if (b_die) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, DIETEX);
		glUniform1i(glGetUniformLocation(program, "DIETEX"), 0);
		glBindVertexArray(vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);
		return;
	}
	if (before_game == 0 || b_help) {
		glActiveTexture(GL_TEXTURE0);
		if (b_help) {
			switch (stage) {
			case 1:
				glBindTexture(GL_TEXTURE_2D, HELPTEX1);
				glUniform1i(glGetUniformLocation(program, "HELPTEX1"), 0);
				break;
			case 2:
				glBindTexture(GL_TEXTURE_2D, HELPTEX2);
				glUniform1i(glGetUniformLocation(program, "HELPTEX2"), 0);
				break;
			case 3:
				glBindTexture(GL_TEXTURE_2D, HELPTEX3);
				glUniform1i(glGetUniformLocation(program, "HELPTEX3"), 0);
				break;
			default: //final
				glBindTexture(GL_TEXTURE_2D, FINALTEX);
				glUniform1i(glGetUniformLocation(program, "FINALTEX"), 0);
				break;
			}
		}

		else {
			glBindTexture(GL_TEXTURE_2D, TITLETEX);
			glUniform1i(glGetUniformLocation(program, "TITLETEX"), 0);
		}
		glBindVertexArray(vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);
		return;
	}
	if (before_game == 1) {
		glActiveTexture(GL_TEXTURE0);
		if (selected_res[0] == 0) {
			if (selected_res[1] == 0) {
				glBindTexture(GL_TEXTURE_2D, SELECTTEX00);
			}
			else if (selected_res[1] == 1) {
				glBindTexture(GL_TEXTURE_2D, SELECTTEX01);

			}
			else {
				glBindTexture(GL_TEXTURE_2D, SELECTTEX02);

			}
		}
		else if (selected_res[0] == 1) {
			if (selected_res[1] == 0) {
				glBindTexture(GL_TEXTURE_2D, SELECTTEX10);

			}
			else if (selected_res[1] == 1) {
				glBindTexture(GL_TEXTURE_2D, SELECTTEX11);

			}
			else {
				glBindTexture(GL_TEXTURE_2D, SELECTTEX12);

			}
		}
		else {
			if (selected_res[1] == 0) {
				glBindTexture(GL_TEXTURE_2D, SELECTTEX20);

			}
			else if (selected_res[1] == 1) {
				glBindTexture(GL_TEXTURE_2D, SELECTTEX21);

			}
			else {
				glBindTexture(GL_TEXTURE_2D, SELECTTEX22);

			}
		}
		glUniform1i(glGetUniformLocation(program, "SELECTTEX"), 0);
		glBindVertexArray(vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);
		return;
	}
	if (before_game == 2) {
		alpha = 0.2f + 0.8f * abs(sin(float(glfwGetTime())));
		int x = window_size.x, y = window_size.y;
		float scale_x = x / 1280.0f, scale_y = y / 720.0f;
		float scale = min(scale_x, scale_y);

		render_text("** How to play this game **", x / 4, y / 6, 0.8f * scale, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
		render_text("Press direction keys to move the character.", x / 6, y * 2 / 7, 0.8f * scale, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
		render_text("Press Space key to fire the snowflakes.", x / 6, y * 3 / 7, 0.8f * scale, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
		render_text("Press 't' key to pick up or put things down.", x / 6, y * 4 / 7, 0.8f * scale, vec4(1.0f, 1.0f, 1.0f, 1.0f), dpi_scale);
		render_text("Press Space key to start!", x / 5, y * 4 / 5, 1.0f * scale, vec4(1, 1, 0, alpha), dpi_scale);
		glfwSwapBuffers(window);
		return;
	}

	GLint uloc;
	uloc = glGetUniformLocation(program, "sky"); if (uloc > -1) glUniform1i(uloc, false);
	uloc = glGetUniformLocation(program, "snow"); if (uloc > -1) glUniform1i(uloc, false);
	mode = 0;
	float alpha = 0.3f;
	//false를 넣어준다.
	/*if (opacity)
		printf("true\n");
	else
		printf("false\n");*/
	int model_number = 0;
	for (auto& model : models) {
		model_number++;
		// 모든 models vector에 등록된 model을 돌며 렌더링한다.
		if (!model.visible)
			//model struct의 visible이 꺼져있으면 출력하지 않고 넘어간다.
			continue;
		//if (model.name == "Enemy1"||model.name=="Enemy2"||model.name=="Enemy3")
		//	continue;
		mesh2* pMesh = model.pMesh;
		//메쉬포인터 지정하기, 이하 예전 코드와 동일
		glBindVertexArray(pMesh->vertex_array);
		GLint uloc = glGetUniformLocation(program, "model_matrix");
		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model.model_matrix);
		//각 모델의 model_matrix 불러와서 uniform으로 넘기기

		for (size_t k = 0, kn = pMesh->geometry_list.size(); k < kn; k++) {
			geometry& g = pMesh->geometry_list[k];

			//printf("%lf\n", g.mat->textures.ambient.id);
			if (g.mat->textures.diffuse) {
				glBindTexture(GL_TEXTURE_2D, g.mat->textures.diffuse->id);
				glUniform1i(glGetUniformLocation(program, "TEX"), 0);
				glUniform1i(glGetUniformLocation(program, "use_texture"), true);
			}
			else {
				glUniform4fv(glGetUniformLocation(program, "ambient"), 1, (const float*)(&g.mat->ambient));
				glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
				glUniform4fv(glGetUniformLocation(program, "specular"), 1, (const float*)(&g.mat->specular));
				glUniform4fv(glGetUniformLocation(program, "emissive"), 1, (const float*)(&g.mat->emissive));
				//uloc = glGetUniformLocation(program, "color");			if (uloc > -1) glUniform4fv(uloc, 1, vec4(1.0f,1.0f,1.0f,1.0f));
				glUniform1i(glGetUniformLocation(program, "use_texture"), false);

				glUniform1i(glGetUniformLocation(program, "opacity"), opacity);
				glUniform1i(glGetUniformLocation(program, "model_number"), model_number);
			}

			// render vertices: trigger shader programs to process vertex data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->index_buffer);
			glDrawElements(GL_TRIANGLES, g.index_count, GL_UNSIGNED_INT, (GLvoid*)(g.index_start * sizeof(GLuint)));
		}
	}
	// 가져온 모델의 렌더링의 끝

	//printf("%lf\n", g.mat->textures.ambient.id);
	uloc = glGetUniformLocation(program, "sky");			if (uloc > -1) glUniform1i(uloc, true);
	glBindVertexArray(vertex_array);

	//다시 sky변수에 true를 넣어 fragment shader로 넘긴다.
	float scale2 = 400;
	model_matrix_background = mat4::translate(cam.at+vec3(0,0,scale2)) * mat4::rotate(vec3(1, 0, 0), PI) * mat4::scale(scale2);
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background);
	mode = 6;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(cam.at + vec3(0, 0, -scale2))  * mat4::scale(scale2);
	model_matrix_background = model_matrix_background * mat4::rotate(vec3(0, 0, 1), PI);
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background);
	mode = 3;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(cam.at + vec3(-scale2, 0, 0)) * mat4::rotate(vec3(0, 1, 0), PI / 2) * mat4::scale(scale2);
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background);
	mode = 1;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(cam.at + vec3(scale2, 0, 0)) * mat4::rotate(vec3(0, 1, 0), -PI / 2) * mat4::scale(scale2);
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background);
	mode = 4;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(cam.at + vec3(0,-scale2, 0)) * mat4::rotate(vec3(1, 0, 0), -PI / 2) * mat4::scale(scale2);
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background);
	mode = 2;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(cam.at + vec3(0, scale2, 0)) * mat4::rotate(vec3(1, 0, 0), PI / 2) * mat4::scale(scale2);
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background);
	mode = 5;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	mode = 0;
	uloc = glGetUniformLocation(program, "sky");			if (uloc > -1) glUniform1i(uloc, false);

	for (auto& p : particles)
	{
		mat4 translate_matrix = mat4::translate(vec3(p.pos.x, 15.0f, p.pos.y));
		mat4 scale_matrix = mat4::scale(p.scale);
		mat4 model_matrix = translate_matrix * scale_matrix;

		glActiveTexture(GL_TEXTURE0);		// select the texture slot to bind
		glBindTexture(GL_TEXTURE_2D, SNOWTEX);
		glUniform1i(glGetUniformLocation(program, "TEX_SNOW"), 0);	 // GL_TEXTURE0

		GLint uloc;
		uloc = glGetUniformLocation(program, "snow");			if (uloc > -1) glUniform1i(uloc, true);
		uloc = glGetUniformLocation(program, "color");			if (uloc > -1) glUniform4fv(uloc, 1, p.color);
		uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	}
	uloc = glGetUniformLocation(program, "snow"); if (uloc > -1) glUniform1i(uloc, false);

	char stagenum[20], lifenum[20];
	sprintf(stagenum, "Stage: %d", stage);
	sprintf(lifenum, "Life: %d", life);

	render_text(stagenum, 50, 50, 1.0f, vec4(0, 0, 0, 1.0f), dpi_scale);
	render_text(lifenum, window_size.x - 220, 50, 1.0f, vec4(0, 0, 0, 1.0f), dpi_scale);

	glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void print_help()
{
	printf("[help]\n");
	printf("- press ESC or 'q' to terminate the program\n");
	printf("- press F1 or 'h' to see help\n");
	printf("- press Home to reset camera\n");
	printf("- press 'd' to toggle between OBJ format and 3DS format\n");
	printf("\n");
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) glfwSetWindowShouldClose(window, GL_TRUE);
		if (before_game == 2) {
			if (key == GLFW_KEY_SPACE) {
				before_game++;
				if (selected_res[0] == 1) {
					delete models[6].pMesh;
					model hani = { "../bin/mesh/Character.obj","Character",vec3(2.3f, 0, 20),0.4f };
					hani.pMesh = load_model(hani.path);
					models[6] = hani;
				}
				else {
					delete models[6].pMesh;
					model hani = { "../bin/mesh/MainGirl.obj","Character",vec3(2.3f, 0, 20),0.4f };
					hani.pMesh = load_model(hani.path);
					models[6] = hani;
				}
			}
			return;
		}
		if (key == GLFW_KEY_H || key == GLFW_KEY_F1) {
			b_help = true;
		}
		else if (key == GLFW_KEY_HOME) cam = camera();
		else if (key == GLFW_KEY_C) {
			model& m = getModel("Character");
			printf("%f %f %f\n", m.center.x, m.center.z, m.center.y);
		}
		else if (key == GLFW_KEY_D)
		{
			static bool is_obj = true;
			is_obj = !is_obj;

			glFinish();
			delete_texture_cache();
		}
		else if (key == GLFW_KEY_SPACE && stage>0) {
			engine->play2D(attack_mp3_src, false);
			b_space = true;
		}
#ifndef GL_ES_VERSION_2_0
		else if (key == GLFW_KEY_R)
		{
			before_game = 0;
			stage = 0;
			life = 3;

			getModel("triangle").stage = 1;
			getModel("triangle").theta = 0;
			getModel("triangle").center = vec3(100, 30, MAP_Z - 20);
			getModel("triangle").visible = false;
			b_triangle = false;
			//b_ability_to_get = true;
			triangle_added = false;
		}
		else if (key == GLFW_KEY_LEFT) {
			l = true; r = false; u = false; d = false;
		}

		else if (key == GLFW_KEY_RIGHT) {
			l = false; r = true; u = false; d = false;
		}
		else if (key == GLFW_KEY_UP) {
			l = false; r = false; u = true; d = false;
		}
		else if (key == GLFW_KEY_DOWN) {
			l = false; r = false; u = false; d = true;
		}
		else if (key == GLFW_KEY_B) {
			model& m = getModel("Map2_2");
			if (&m == &none)
				printf("not found: Map2_2\n");
			m.visible = !m.visible;
			//map2_2, 다리부분의 표시유무를 조절한다.
		}
		else if (key == GLFW_KEY_T) {
			model& c = getModel("Character");
			model& m = getModel("triangle");
			//b_triangle = false;
			//b_ability_to_get = false;
			if (xz_distance(c.center,m.center) <7)
				b_ability_to_get = !b_ability_to_get;
		}
		else if (key == GLFW_KEY_LEFT_CONTROL)
			accel = 1;
		// 속력 감소시키기 update에서 속력이 25로 감소한다.
		else if (key == GLFW_KEY_LEFT_ALT)
			character_stop = true;
#endif
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) {
			l = false;
		}
		/*else if (key == GLFW_KEY_T) {
			b_ability_to_get = false;
		}*/
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1) {
			b_help = false;
		}
		else if (key == GLFW_KEY_RIGHT) {
			r = false;
		}
		else if (key == GLFW_KEY_UP) {
			u = false;
		}
		else if (key == GLFW_KEY_DOWN) {
			d = false;
		}
		else if (key == GLFW_KEY_LEFT_CONTROL)
			accel = 0;
		else if (key == GLFW_KEY_LEFT_ALT)
			character_stop = false;
		//속력감소의 원상복귀
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	static int cnt = 0;
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		ivec2 npos2 = converted_loc(pos.x, pos.y, window_size);
		printf("%f %f %d %d\n", pos.x, pos.y, npos2.x, npos2.y);
		if (before_game == 0) {
			if (894 < npos2.x && npos2.x < 1181 && 64 < npos2.y && npos2.y < 150)
				before_game++;
			cnt = 2;
			selected_res[0] = 0;
			selected_res[1] = 0;
			return;
		}
		if (before_game == 1) {
			cnt--;
			if (cnt > 0)
				return;
			if (269 < npos2.x && npos2.x < 596 && 124 < npos2.y && npos2.y < 585) {
				//before_game++;
				selected_res[0] = 1;
			}
			else if (693 < npos2.x && npos2.x < 1018 && 124 < npos2.y && npos2.y < 585) {
				//before_game++;
				selected_res[0] = 2;
			}
			else if (1055 < npos2.x && npos2.x < 1219 && 236 < npos2.y && npos2.y < 326) {
				difficulty = 0;
				selected_res[1] = 1;
			}
			else if (1055 < npos2.x && npos2.x < 1219 && 380 < npos2.y && npos2.y < 471) {
				difficulty = 1;
				selected_res[1] = 2;
			}
			else if (1029 < npos2.x&&npos2.x < 1211 && 28 < npos2.y&&npos2.y < 123&& selected_res[0]!=0&&selected_res[1]!=0) {

				before_game++;
			}
			return;
		}
		if (before_game == 2) {
			return;
		}
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS) {
			tb.begin(cam.view_matrix, npos);
		}
		else if (action == GLFW_RELEASE)	tb.end();
	}
}

void motion(GLFWwindow* window, double x, double y)
{
	if (!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
	cam.view_matrix = tb.update(npos);
}

GLuint create_texture(const char* image_path, bool mipmap = true, GLenum wrap = GL_CLAMP_TO_EDGE, GLenum filter = GL_LINEAR)
{
	// load image
	image* i = cg_load_image(image_path); if (!i) return 0; // return null texture; 0 is reserved as a null texture
	int		w = i->width, h = i->height, c = i->channels;

	// induce internal format and format from image
	GLint	internal_format = c == 1 ? GL_R8 : c == 2 ? GL_RG8 : c == 3 ? GL_RGB8 : GL_RGBA8;
	GLenum	format = c == 1 ? GL_RED : c == 2 ? GL_RG : c == 3 ? GL_RGB : GL_RGBA;

	// create a src texture (hani texture)
	GLuint texture;
	glGenTextures(1, &texture); if (texture == 0) { printf("%s(): failed in glGenTextures()\n", __func__); return 0; }
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, i->ptr);
	if (i) { delete i; i = nullptr; } // release image

	// build mipmap
	if (mipmap)
	{
		int mip_levels = 0; for (int k = w > h ? w : h; k; k >>= 1) mip_levels++;
		for (int l = 1; l < mip_levels; l++)
			glTexImage2D(GL_TEXTURE_2D, l, internal_format, (w >> l) == 0 ? 1 : (w >> l), (h >> l) == 0 ? 1 : (h >> l), 0, format, GL_UNSIGNED_BYTE, nullptr);
		if (glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
	}

	// set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, !mipmap ? filter : filter == GL_LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);

	return texture;
}
void rotate_character(float t, float old_t, float ntheta) {
	// ntheta를 통하여 목표로 하는 각도 설정 , theta와 nthtea의 범위는 (0<= x < 2PI)
	// theta의 값만큼 오른쪽 보는 방향 default에서 반시계 방향으로 회전
	//printf("%f %f %f\n", theta, 2*PI, theta0);
	float& theta = getModel("Character").theta;
	int vel = 3;
	if (l || r || u || d) {
		//회전 중일때 ntheta와 theta값을 토대로 theta값 재설정
		if (ntheta == 0 && (abs(theta - 2 * PI) < 0.1f || abs(theta) < 0.1f))
			theta = 0;
		else if (abs(theta - ntheta) < 0.1f)
			theta = ntheta;
		else if (abs(abs(ntheta - theta) - PI) < 0.01f) {
			//ntheta theta차이가 PI값일때
			if (abs(theta - PI) < 0.01f || abs(theta - PI / 2 * 3) < 0.01f ||
				abs(theta) < 0.01f)
				theta += vel * (t - old_t);
			else //(theta<0.01f || 2*PI-theta<0.01f)
				theta -= vel * (t - old_t);
		}
		//좌<->우, 상<->하 이동시 일관성부여
		else if (theta < ntheta) {
			//위에를 제외하고 ntheta가 더클때
			if (abs(abs(ntheta - theta) - PI) < 0.01f || ntheta - theta <= PI)
				theta += vel * (t - old_t);
			//증가하는게 최선
			else
				theta -= vel * (t - old_t);
			//감소하는게 최선
		}
		else if (theta > ntheta) {
			//if (abs(abs(ntheta - theta)-PI) < 0.01f)
			//	theta -= 10 * (t - old_t);// *abs(ntheta - theta0);
			if (theta - ntheta < PI)
				theta -= vel * (t - old_t); // * abs(ntheta - theta0);
			else
				theta += vel * (t - old_t);// *abs(ntheta - theta0);
		}
	}
	while (theta >= 2 * PI)
		theta -= 2 * PI;
	while (theta < 0)
		theta += 2 * PI;
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor(0 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f);	// set clear color
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);								// turn on depth tests
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	vertex corners[4];
	corners[0].pos = vec3(-1.0f, -1.0f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(+1.0f, -1.0f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(+1.0f, +1.0f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1.0f, +1.0f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);
	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return false; }

	//텍스쳐 로딩

	SKY_LEFT = create_texture(skybox_left_path, true); if (!SKY_LEFT) return false;
	SKY_DOWN = create_texture(skybox_down_path, true); if (!SKY_DOWN) return false;
	SKY_BACK = create_texture(skybox_back_path, true); if (!SKY_BACK) return false;
	SKY_RIGHT = create_texture(skybox_right_path, true); if (!SKY_RIGHT) return false;
	SKY_UP = create_texture(skybox_up_path, true); if (!SKY_UP) return false;
	SKY_FRONT = create_texture(skybox_front_path, true); if (!SKY_FRONT) return false;

	// load the mesh
	models.push_back({ "../bin/mesh/Map1.obj","Map1", vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.push_back({ "../bin/mesh/Triangle.obj","triangle", vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.back().visible = false;
	models.push_back({ "../bin/mesh/Map2_1.obj" ,"Map2_1",vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.push_back({ "../bin/mesh/Map2_2.obj" ,"Map2_2",vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.back().visible = false;
	models.push_back({ "../bin/mesh/Map2_3.obj" ,"Map2_3" , vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.push_back({ "../bin/mesh/Map3.obj","Map3", vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.push_back({ "../bin/mesh/Character.obj", "Character",vec3(0),0.4f });
	models.push_back({ "../bin/mesh/Enemy1.obj", "Enemy1", vec3(96,0,16), (0.5) });
	models.push_back({ "../bin/mesh/Enemy2.obj", "Enemy2", vec3(16,0,16), (0.5) });
	models.push_back({ "../bin/mesh/Enemy3.obj", "Enemy3", vec3(8,0,32),(0.5) });
	models.push_back({ "../bin/mesh/Boss.obj", "Boss",  vec3(MAP_X * 3 / 4, 0, MAP_Z / 2),(0.5) });
	models.back().visible = false;
	//model들의 정보를 저장한 models vector에 정보를 넣어준다. model.h의 자료구조를 참조
	// model matrix의 정보도 바로 생성해서 삽입

	if (!load_models())
		//models에 저장된 모델들을 불러온다. 모델의 mesh pointer는 models의 model구조체에 저장된다.
		return false;


	static vertex snow_vertices[] = { {vec3(-1,-1,0),vec3(0,0,1),vec2(0,0)}, {vec3(1,-1,0),vec3(0,0,1),vec2(1,0)}, {vec3(-1,1,0),vec3(0,0,1),vec2(0,1)}, {vec3(1,1,0),vec3(0,0,1),vec2(1,1)} }; // strip ordering [0, 1, 3, 2]

	// generation of vertex buffer: use snow_vertices as it is
	GLuint snow_vertex_buffer;
	glGenBuffers(1, &snow_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, snow_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, &snow_vertices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (snow_vertex_array) glDeleteVertexArrays(1, &snow_vertex_array);
	snow_vertex_array = cg_create_vertex_array(snow_vertex_buffer);
	if (!snow_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return false; }

	// create a particle texture from image with alpha
	SNOWTEX = cg_create_texture(snow_image_path, false); if (!SNOWTEX) return false; // disable mipmapping

	// initialize particles
	particles.resize(particle_t::MAX_PARTICLES);

	if (!init_text()) return false;
	TITLETEX = create_texture(title_image_path, true); if (!TITLETEX) return false;
	HELPTEX1 = create_texture(help_image_path1, true); if (!HELPTEX1) return false;
	HELPTEX2 = create_texture(help_image_path2, true); if (!HELPTEX2) return false;
	HELPTEX3 = create_texture(help_image_path3, true); if (!HELPTEX3) return false;
	FINALTEX = create_texture(final_image_path, true); if (!FINALTEX) return false;
	SELECTTEX00 = create_texture(select_image_path00, true); if (!SELECTTEX00) return false;
	SELECTTEX01 = create_texture(select_image_path01, true); if (!SELECTTEX01) return false;
	SELECTTEX02 = create_texture(select_image_path02, true); if (!SELECTTEX02) return false;
	SELECTTEX10 = create_texture(select_image_path10, true); if (!SELECTTEX10) return false;
	SELECTTEX11 = create_texture(select_image_path11, true); if (!SELECTTEX11) return false;
	SELECTTEX12 = create_texture(select_image_path12, true); if (!SELECTTEX12) return false;
	SELECTTEX20 = create_texture(select_image_path20, true); if (!SELECTTEX20) return false;
	SELECTTEX21 = create_texture(select_image_path21, true); if (!SELECTTEX21) return false;
	SELECTTEX22 = create_texture(select_image_path22, true); if (!SELECTTEX22) return false;
	DIETEX = create_texture(die_image_path, true); if (!DIETEX) return false;

	engine = irrklang::createIrrKlangDevice();
	if (!engine) return false;

	//add sound source from the sound file
	mp3_src = engine->addSoundSourceFromFile(mp3_path);
	attack_mp3_src = engine->addSoundSourceFromFile(attack_mp3_path);
	bell_mp3_src = engine->addSoundSourceFromFile(bell_mp3_path);
	falling_mp3_src = engine->addSoundSourceFromFile(falling_mp3_path);

	//set default volume
	mp3_src->setDefaultVolume(0.7f);
	attack_mp3_src->setDefaultVolume(0.5f);
	bell_mp3_src->setDefaultVolume(0.5f);
	falling_mp3_src->setDefaultVolume(0.5f);

	//play the sound file
	engine->play2D(mp3_src, true);

	return true;
}

void user_finalize()
{
	//모델들의 mesh pointer에 접근해서 자료 지우기
	delete_texture_cache();
	for (auto& model : models)
		delete model.pMesh;
	// close the engine
	engine->drop();
}

int main(int argc, char* argv[])
{
	// create window and initialize OpenGL extensions
	if (!(window = cg_create_window(window_name, window_size.x, window_size.y))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if (!(program = cg_create_program(vert_shader_path, frag_shader_path))) { glfwTerminate(); return 1; }	// create and compile shaders/program
	if (!user_init()) { printf("Failed to user_init()\n"); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback(window, reshape);	// callback for window resizing events
	glfwSetKeyCallback(window, keyboard);			// callback for keyboard events
	glfwSetMouseButtonCallback(window, mouse);	// callback for mouse click inputs
	glfwSetCursorPosCallback(window, motion);		// callback for mouse movement

	// enters rendering/event loop
	for (frame = 0; !glfwWindowShouldClose(window); frame++)
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		old_t = float(glfwGetTime());
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}