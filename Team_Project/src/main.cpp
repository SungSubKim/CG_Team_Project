#include "cgmath.h"			// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION
#include "cgut2.h"			// slee's OpenGL utility
#include "trackball.h"
#include "assimp_loader.h"
#include "model.h"
#include "area.h"
#include "particle.h"

//*************************************
// global constants
static const char*	window_name = "Team project - Woori's Adventure";
static const char*	vert_shader_path = "../bin/shaders/model.vert";
static const char*	frag_shader_path = "../bin/shaders/model.frag";
static const char* skybox_left_path = "../bin/images/negx.jpg";
static const char* skybox_down_path = "../bin/images/negy.jpg";
static const char* skybox_back_path = "../bin/images/negz.jpg";
static const char* skybox_right_path = "../bin/images/posx.jpg";
static const char* skybox_up_path = "../bin/images/posy.jpg";
static const char* skybox_front_path = "../bin/images/posz.jpg";
static const char* snow_image_path = "../bin/images/snow-flake.png";

std::vector<vertex>	unit_circle_vertices;	// host-side vertices
//*************************************
// common structures
struct camera
{
	vec3	eye = vec3(0 + MAP_X / 2, 100, 80 + MAP_Z / 2);
	vec3	at = vec3(-2 + MAP_X / 2, 0, 0 + MAP_Z / 2);
	vec3	up = vec3(0, 1, 0);
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f; // must be in radian
	float	aspect_ratio;
	float	dNear = 1.0f;
	float	dFar = 1000.0f;
	mat4	projection_matrix;
};

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

mat4 model_matrix_background=mat4::translate(0,0.0f,-250.0f)*mat4::scale(500.0f,500.0f,100.0f);

// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1280, 720); // cg_default_window_size(); // initial window size






//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program
GLuint vertex_buffer = 0;	// ID holder for vertex buffer
GLuint index_buffer = 0;		// ID holder for index buffer
GLuint	vertex_array = 0;	// ID holder for vertex array object*************************
GLuint	snow_vertex_array = 0;
GLuint	TEX_SKY = 0;
GLuint	SKY_LEFT = 0;
GLuint	SKY_DOWN = 0;
GLuint	SKY_BACK = 0;
GLuint	SKY_RIGHT = 0;
GLuint	SKY_UP = 0;
GLuint	SKY_FRONT = 0;
GLuint	SNOWTEX = 0;
GLuint	mode;
//*************************************
// global variables
int		frame = 0;		// index of rendering frames
bool	show_texcoord = false;
bool	b_wireframe = false;
int		direc = 0;
bool	b_space = false, character_stop = false;
std::vector<particle_t> particles;
int stage = 2;

//*************************************
// scene objects

camera		cam;
trackball	tb;
bool l = false, r = false, u = false, d = false; // 어느쪽으로 keyboard가 눌렸는지 flag
float old_t=0;					//update 함수에서 dt값 계산을 위해 쓰이는 old value
float theta0=0;
mat4 model_matrix0;
light_t		light;
material_t	material;

//*************************************
float min(float a, float b) {
	return a < b ? a : b;
}
	//float& theta = getModel("Character").theta;
void rotate_chracter(float t, float old_t, float ntheta) {
	// ntheta를 통하여 목표로 하는 각도 설정 , theta와 nthtea의 범위는 (0<= x < 2PI)
	// theta의 값만큼 오른쪽 보는 방향 default에서 반시계 방향으로 회전
	//printf("%f %f %f\n", theta, 2*PI, theta0);
	float& theta = getModel("Character").theta;
	int vel = 5;
	if (l || r || u || d) {
		//회전 중일때 ntheta와 theta값을 토대로 theta값 재설정
		if (abs(theta-ntheta) < 0.2f)
			theta = ntheta;
		else if (abs(abs(ntheta - theta) - PI) < 0.01f) {
			if (abs(theta - PI) < 0.01f || abs(theta - PI / 2 * 3) < 0.01f ||
				abs(theta) < 0.01f)
				theta += vel * (t - old_t);
			else //(theta<0.01f || 2*PI-theta<0.01f)
				theta -= vel * (t - old_t);
		}
		//좌<->우, 상<->하 이동시 일관성부여
		else if (theta < ntheta ) {
			//위에를 제외하고 ntheta가 더클때
			if (abs(abs(ntheta - theta) - PI) < 0.01f || ntheta - theta <= PI)
				theta += vel * (t - old_t);
			//증가하는게 최선
			else
				theta -= vel * (t - old_t);
			//감소하는게 최선
		}
		else if (theta >ntheta) {
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
void update()
{
	
	glUseProgram(program);

	// update projection matrix
	cam.aspect_ratio = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect_ratio, cam.dNear, cam.dFar);

	// build the model matrix for oscillating scale
	float t = float(glfwGetTime());
	int rate = 20; if (accel) rate *= 2;
	float ntheta=0, ds=0;
	//키보드에서 left control키를 누른 상태면 속력이 감소하게 해준다
	model& model_character = getModel("Character");
	vec3& s_center = model_character.center;
	if (!character_stop)
		ds = (t - old_t);
	if (l) {
		s_center.x -= ds * rate;
		ntheta = PI / 2*2;
		direc = 1;
	}
	else if (r) {
		s_center.x += ds * rate;
		ntheta = 0;
		direc = 2;
	}
	else if (u){
		s_center.z -= ds * rate;
		ntheta = PI / 2;
		direc = 3;
	}
	else if (d){
		s_center.z += ds * rate;
		ntheta = PI / 2*3;
		direc = 4;
	} 
	rotate_chracter(t, old_t,ntheta);
	old_t = t;
	switch (stage) {
		case 2:
			check_map2();
			check_to_enemy();
			stage = getTriangle();
			break;
		case 3:
			check_map3();
		default:
			break;
	}
	
	setStage(stage);
	CopyMemory(old_s_center, s_center, sizeof(vec3));
	// Map2의 다리위에 올라가 있는지를 체크, 이게 아니면 s_center의 xz값을 원래대로 되돌린다.
	model_character.update_matrix();
	//center와 theta의 정보를 매트릭스에 반영한다.
	
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

	
	glActiveTexture(GL_TEXTURE1);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, SKY_LEFT);
	glUniform1i(glGetUniformLocation(program, "SKY_LEFT"), 1);
	

	glActiveTexture(GL_TEXTURE2);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, SKY_DOWN);
	glUniform1i(glGetUniformLocation(program, "SKY_DOWN"), 2);
	

	glActiveTexture(GL_TEXTURE3);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, SKY_BACK);
	glUniform1i(glGetUniformLocation(program, "SKY_BACK"), 3);
	
	glActiveTexture(GL_TEXTURE4);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, SKY_RIGHT);
	glUniform1i(glGetUniformLocation(program, "SKY_RIGHT"), 4);
	

	glActiveTexture(GL_TEXTURE5);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, SKY_UP);
	glUniform1i(glGetUniformLocation(program, "SKY_UP"), 5);
	

	glActiveTexture(GL_TEXTURE6);								// select the texture slot to bind
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
	
	GLint uloc;
	uloc = glGetUniformLocation(program, "sky"); if (uloc > -1) glUniform1i(uloc, false); 
	uloc = glGetUniformLocation(program, "snow"); if (uloc > -1) glUniform1i(uloc, false);
	mode = 0;
	//false를 넣어준다.
	for (auto& model : models) {
		// 모든 models vector에 등록된 model을 돌며 렌더링한다.
		if (!model.visible)
			//model struct의 visible이 꺼져있으면 출력하지 않고 넘어간다.
			continue;
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
				glUniform1i(glGetUniformLocation(program, "TEX"), 0);	 // GL_TEXTURE0
				glUniform1i(glGetUniformLocation(program, "use_texture"), true);
			}
			else {
				glUniform4fv(glGetUniformLocation(program, "ambient"), 1, (const float*)(&g.mat->ambient));
				glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
				glUniform4fv(glGetUniformLocation(program, "specular"), 1, (const float*)(&g.mat->specular));
				glUniform4fv(glGetUniformLocation(program, "emissive"), 1, (const float*)(&g.mat->emissive));
				//glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
				glUniform1i(glGetUniformLocation(program, "use_texture"), false);
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
	model_matrix_background = mat4::translate(100,100,-250)*mat4::scale(300.0f, 300.0f, 100.0f); 
	//front
	mode = 6;
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background); //구 사용
	
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(100, 100, 350) * mat4::rotate(vec3(1, 0, 0), PI) * mat4::scale(300.0f, 300.0f, 100.0f);
	model_matrix_background = model_matrix_background * mat4::rotate(vec3(0, 0, 1), PI);
	//back
	
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background); //구 사용
	mode = 3;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(-200, 100, 50)* mat4::rotate(vec3(0,1,0),PI/2)* mat4::scale(300.0f, 300.0f, 100.0f);
	//left
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background); //구 사용
	mode = 1;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(400, 100, 50) * mat4::rotate(vec3(0, 1, 0), -PI / 2) * mat4::scale(300.0f, 300.0f, 100.0f);
	//right
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background); //구 사용
	mode = 4;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(100, -200, 50) * mat4::rotate(vec3(1,0,0),-PI/2)*mat4::scale(300.0f, 300.0f, 100.0f);
	//down
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background); //구 사용
	mode = 2;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	model_matrix_background = mat4::translate(100, 400, 50) * mat4::rotate(vec3(1, 0, 0), PI / 2) * mat4::scale(300.0f, 300.0f, 100.0f);
	//up
	
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background); //구 사용
	mode = 5;
	glUniform1i(glGetUniformLocation(program, "mode"), mode);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	mode = 0;
	//uloc = glGetUniformLocation(program, "sky");			if (uloc > -1) glUniform1i(uloc, false);

	glEnable(GL_BLEND);
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
	glDisable(GL_BLEND);
	uloc = glGetUniformLocation(program, "snow"); if (uloc > -1) glUniform1i(uloc, false);

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
	//printf("%d %d %d \n", action, mods, key);
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME)					cam = camera();
		else if (key == GLFW_KEY_T) {
			model& m = getModel("triangle");
			if (&m == &none)
				printf("not found: triangle\n");
			m.visible = !m.visible;
			//Triangle의 표시여부를 조정한다.
			show_texcoord = !show_texcoord;
		}
		else if (key == GLFW_KEY_D)
		{
			static bool is_obj = true;
			is_obj = !is_obj;

			glFinish();
			delete_texture_cache();
		}
		else if (key == GLFW_KEY_SPACE) b_space = true;
#ifndef GL_ES_VERSION_2_0
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
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
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS)			tb.begin(cam.view_matrix, npos);
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

	// create a src texture (lena texture)
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

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor(0 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f);	// set clear color
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);								// turn on depth tests
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	models.push_back({ "../bin/mesh/Map1.obj","Map1",
		vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.push_back({"../bin/mesh/Triangle.obj","triangle",
		vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.push_back({ "../bin/mesh/Map2_1.obj" ,"Map2_1",
		vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.push_back({ "../bin/mesh/Map2_2.obj" ,"Map2_2",
		vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.back().visible = false;
	models.push_back({ "../bin/mesh/Map2_3.obj" ,"Map2_3" ,
		vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.push_back({ "../bin/mesh/Map3.obj","Map3",
		vec3(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) });
	models.push_back({ "../bin/mesh/Character.obj","Character",vec3(0),0.4f});
	models.push_back({ "../bin/mesh/Enemy1.obj","Enemy1",
		vec3(96,0,16), (0.5) });
	models.push_back({ "../bin/mesh/Enemy2.obj","Enemy2",
		vec3(16,0,16), (0.5) });
	models.push_back({ "../bin/mesh/Enemy3.obj","Enemy3",
		vec3(8,0,32),(0.5) });
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

	return true;
}

void user_finalize()
{
	delete_texture_cache();
	for (auto& model : models)
		delete model.pMesh;
	//모델들의 mesh pointer에 접근해서 자료 지우기
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
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}